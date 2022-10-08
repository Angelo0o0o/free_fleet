/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#ifndef INCLUDE__FREE_FLEET__CLIENT__ROBOTHANDLER_HPP
#define INCLUDE__FREE_FLEET__CLIENT__ROBOTHANDLER_HPP

#include <string>
#include <vector>
#include <optional>

#include <Eigen/Geometry>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

#include <free_fleet/Types.hpp>

namespace free_fleet {

class RobotHandler
{
public:

  virtual bool current_state(
    nlohmann::json& state,
    std::string& error) const = 0;

  using RequestCompleted = std::function<void()>;

  virtual bool relocalize(
    const Location& new_location,
    RequestCompleted relocalize_finished_callback,
    std::string& error) = 0;

  virtual bool follow_new_path(
    const std::vector<Location>& path,
    RequestCompleted path_finished_callback,
    std::string& error) = 0;

  virtual bool stop(
    RequestCompleted stopped_callback,
    std::string& error) = 0;

  virtual bool resume(
    RequestCompleted resumed_callback,
    std::string& error) = 0;

  virtual bool custom_command(
    const nlohmann::json& details,
    RequestCompleted custom_command_finished_callback,
    std::string& error) = 0;
};

} // namesace free_fleet

#endif // INCLUDE__FREE_FLEET__CLIENT__ROBOTHANDLER_HPP
