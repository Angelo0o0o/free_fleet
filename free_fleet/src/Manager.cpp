/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <iostream>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include <rmf_traffic/Time.hpp>

#include <free_fleet/Manager.hpp>
#include <free_fleet/agv/RobotInfo.hpp>

#include "agv/internal_RobotInfo.hpp"

#include "requests/ModeRequestInfo.hpp"
#include "requests/NavigationRequestInfo.hpp"
#include "requests/RelocalizationRequestInfo.hpp"

#include "internal_Manager.hpp"

namespace free_fleet {

//==============================================================================
bool Manager::Implementation::connected() const
{
  return _middleware && _graph && _to_robot_transform && _time_now_fn
    && _started && _thread.joinable();
}

//==============================================================================
void Manager::Implementation::start(uint32_t frequency)
{
  std::cout << "Starting manager thread..." << std::endl;
  _thread_frequency = frequency;
  _thread =
    std::thread(
      std::bind(&free_fleet::Manager::Implementation::thread_fn, this));
  _started = true;
}

//==============================================================================
void Manager::Implementation::run_once()
{
  // get states
  auto states = _middleware->read_states();
  std::lock_guard<std::mutex> lock(_mutex);
  for (const auto s : states)
  {
    messages::RobotState transformed_state{
      s.name,
      s.model,
      s.task_id,
      s.mode,
      s.battery_percent,
      _to_robot_transform->backward_transform(s.location),
      s.path_target_index};

    const auto r_it = _robots.find(s.name);
    bool new_robot = r_it == _robots.end();
    if (new_robot)
    {
      _robots[s.name] = agv::RobotInfo::Implementation::make(
        transformed_state,
        _graph,
        _time_now_fn());
      std::cout << "Registered new robot: [" << s.name << "]..."
        << std::endl;
    }
    else
    {
      agv::RobotInfo::Implementation::get(*r_it->second).update_state(
        transformed_state, _time_now_fn());
    }

    // Updates external uses of the robot's information
    if (_robot_updated_callback_fn)
      _robot_updated_callback_fn(r_it->second);

    // for each robot figure out whether any tasks were not received yet
    const auto t_it = _unacknowledged_tasks.find(s.task_id);
    if (t_it != _unacknowledged_tasks.end())
    {
      t_it->second->acknowledged_time(_time_now_fn());
      _unacknowledged_tasks.erase(t_it);
    }
  }
  
  // Send out all unreceived tasks again
  for (const auto t_it : _unacknowledged_tasks)
  {
    t_it.second->send_request();
  }
}

//==============================================================================
void Manager::Implementation::thread_fn()
{
  const double seconds_per_iteration = 1.0 / _thread_frequency;
  const rmf_traffic::Duration duration_per_iteration =
    rmf_traffic::time::from_seconds(seconds_per_iteration);
  rmf_traffic::Time t_prev = _time_now_fn();

  while (connected())
  {
    if (_time_now_fn() - t_prev < duration_per_iteration)
      continue;
    t_prev = _time_now_fn();

    run_once();
  }
}

//==============================================================================
Manager::SharedPtr Manager::make(
  const std::string& fleet_name,
  std::shared_ptr<rmf_traffic::agv::Graph> graph,
  std::shared_ptr<transport::Middleware> middleware,
  std::shared_ptr<CoordinateTransformer> to_robot_transform,
  TimeNow time_now_fn,
  RobotUpdatedCallback robot_updated_callback_fn)
{
  auto make_error_fn = [](const std::string& error_msg)
  {
    std::cerr << "[Error]: " << error_msg << std::endl;
    return nullptr;
  };

  if (fleet_name.empty())
    return make_error_fn("Provided fleet name must not be empty.");
  if (!graph)
    return make_error_fn("Provided traffic graph is invalid.");
  if (!middleware)
    return make_error_fn("Provided free fleet middleware is invalid.");
  if (!to_robot_transform)
    return make_error_fn(
      "Provided free fleet CoordinateTransformer is invalid.");
  if (!time_now_fn)
    return make_error_fn("Provided time function is invalid.");

  SharedPtr manager_ptr(new Manager);
  manager_ptr->_pimpl->_fleet_name = fleet_name;
  manager_ptr->_pimpl->_graph = std::move(graph);
  manager_ptr->_pimpl->_middleware = std::move(middleware);
  manager_ptr->_pimpl->_to_robot_transform = std::move(to_robot_transform);
  manager_ptr->_pimpl->_time_now_fn = std::move(time_now_fn);
  manager_ptr->_pimpl->_robot_updated_callback_fn =
    std::move(robot_updated_callback_fn);
  return manager_ptr;
}

//==============================================================================
Manager::Manager()
: _pimpl(rmf_utils::make_impl<Implementation>(Implementation()))
{}

//==============================================================================
void Manager::start(uint32_t frequency)
{
  if (frequency == 0)
    throw std::range_error("[Error]: Frequency has to be greater than 0.");
  if (_pimpl->_started)
    throw std::runtime_error("[Error]: Manager has already been started.");

  _pimpl->start(frequency);
}

//==============================================================================
auto Manager::robot_names() -> std::vector<std::string>
{
  std::lock_guard<std::mutex> lock(_pimpl->_mutex);
  std::vector<std::string> result;
  result.reserve(_pimpl->_robots.size());
  for (const auto it : _pimpl->_robots)
  {
    result.push_back(it.first);
  }
  return result;
}

//==============================================================================
auto Manager::robot(const std::string& robot_name) 
  -> std::shared_ptr<agv::RobotInfo>
{
  std::lock_guard<std::mutex> lock(_pimpl->_mutex);
  const auto it = _pimpl->_robots.find(robot_name);
  if (it == _pimpl->_robots.end())
    return nullptr;
  return it->second;
}

//==============================================================================
auto Manager::all_robots() -> std::vector<std::shared_ptr<agv::RobotInfo>>
{
  std::lock_guard<std::mutex> lock(_pimpl->_mutex);
  std::vector<std::shared_ptr<agv::RobotInfo>> infos;
  infos.reserve(_pimpl->_robots.size());
  for (const auto it : _pimpl->_robots)
  {
    infos.push_back(it.second);
  }
  return infos;
}

//==============================================================================
auto Manager::send_mode_request(
  const std::string& robot_name,
  const messages::RobotMode& mode,
  std::vector<messages::ModeParameter> parameters)
  -> rmf_utils::optional<std::size_t>
{
  std::lock_guard<std::mutex> lock(_pimpl->_mutex);
  if (_pimpl->_robots.find(robot_name) == _pimpl->_robots.end())
  {
    // std::cerr << "No such robot." << std::endl;
    return rmf_utils::nullopt;
  }

  if ((++_pimpl->_current_task_id) == _pimpl->_idle_task_id)
    ++_pimpl->_current_task_id;

  messages::ModeRequest request{
    robot_name,
    _pimpl->_current_task_id,
    mode,
    parameters};
  auto mode_request_info = std::make_shared<requests::ModeRequestInfo>(
    requests::ModeRequestInfo(
      request,
      [this](const messages::ModeRequest& request_msg)
    {
      this->_pimpl->_middleware->send_mode_request(request_msg);
    },
      _pimpl->_time_now_fn()));
  
  auto request_info =
    std::dynamic_pointer_cast<requests::RequestInfo>(mode_request_info);
  _pimpl->_tasks[request.task_id] = request_info;
  _pimpl->_unacknowledged_tasks[request.task_id] = request_info;

  _pimpl->_middleware->send_mode_request(request);
  return _pimpl->_current_task_id;
}

//==============================================================================
auto Manager::send_navigation_request(
  const std::string& robot_name,
  const std::vector<messages::Waypoint>& path)
  -> rmf_utils::optional<std::size_t>
{
  std::lock_guard<std::mutex> lock(_pimpl->_mutex);
  if (path.empty() ||
    _pimpl->_robots.find(robot_name) == _pimpl->_robots.end())
  {
    // std::cerr << "Path is empty or no such robot." << std::endl;
    return rmf_utils::nullopt;
  }

  const std::size_t num_wp = _pimpl->_graph->num_waypoints();
  std::vector<messages::Waypoint> transformed_path;
  for (const auto wp : path)
  {
    std::size_t wp_index = static_cast<std::size_t>(wp.index);
    if (wp_index >= num_wp)
    {
      // std::cerr << "No such waypoint." << std::endl;
      return rmf_utils::nullopt;
    }
    
    const auto g_wp = _pimpl->_graph->get_waypoint(wp_index);
    const Eigen::Vector2d provided_loc {wp.location.x, wp.location.y};
    if (g_wp.get_map_name() != wp.location.level_name ||
      (provided_loc - g_wp.get_location()).norm() > 1e-3)
      return rmf_utils::nullopt;

    transformed_path.push_back(
      messages::Waypoint{
        wp.index,
        _pimpl->_to_robot_transform->forward_transform(wp.location)});
  }

  if ((++_pimpl->_current_task_id) == _pimpl->_idle_task_id)
    ++_pimpl->_current_task_id;

  messages::NavigationRequest request{
    robot_name,
    _pimpl->_current_task_id,
    transformed_path};
  auto navigation_request_info =
    std::make_shared<requests::NavigationRequestInfo>(
      request,
      [this](const messages::NavigationRequest& request_msg)
  {
    this->_pimpl->_middleware->send_navigation_request(request_msg);
  },
      _pimpl->_time_now_fn());

  auto request_info =
    std::dynamic_pointer_cast<requests::RequestInfo>(navigation_request_info);
  _pimpl->_tasks[request.task_id] = request_info;
  _pimpl->_unacknowledged_tasks[request.task_id] = request_info;

  _pimpl->_middleware->send_navigation_request(request);
  return _pimpl->_current_task_id;
}

//==============================================================================
auto Manager::send_relocalization_request(
  const std::string& robot_name,
  const messages::Location& location,
  std::size_t last_visited_waypoint_index)
  -> rmf_utils::optional<std::size_t>
{
  std::lock_guard<std::mutex> lock(_pimpl->_mutex);
  if (_pimpl->_robots.find(robot_name) == _pimpl->_robots.end() ||
    last_visited_waypoint_index >= _pimpl->_graph->num_waypoints())
  {
    // std::cerr << "No such robot or waypoint." << std::endl;
    return rmf_utils::nullopt;
  }

  auto wp =
    _pimpl->_graph->get_waypoint(last_visited_waypoint_index);
  const double dist_from_wp =
    (Eigen::Vector2d{location.x, location.y} - wp.get_location()).norm();
  if (dist_from_wp >= 10.0)
  {
    // std::cerr << "Last visited waypoint is too far away." << std::endl;
    return rmf_utils::nullopt;
  }

  messages::Location transformed_location =
    _pimpl->_to_robot_transform->forward_transform(location);

  if ((++_pimpl->_current_task_id) == _pimpl->_idle_task_id)
    ++_pimpl->_current_task_id;

  messages::RelocalizationRequest request{
    robot_name,
    _pimpl->_current_task_id,
    transformed_location,
    last_visited_waypoint_index};
  auto relocalization_request_info =
    std::make_shared<requests::RelocalizationRequestInfo>(
      request,
      [this](const messages::RelocalizationRequest& request_msg)
  {
    this->_pimpl->_middleware->send_relocalization_request(request_msg);
  },
      _pimpl->_time_now_fn());

  auto request_info =
    std::dynamic_pointer_cast<requests::RequestInfo>(
      relocalization_request_info);
  _pimpl->_tasks[request.task_id] = request_info;
  _pimpl->_unacknowledged_tasks[request.task_id] = request_info;

  _pimpl->_middleware->send_relocalization_request(request);
  return _pimpl->_current_task_id;
}

//==============================================================================
} // namespace free_fleet
