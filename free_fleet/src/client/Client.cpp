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

#include <free_fleet/Console.hpp>
#include <free_fleet/client/Client.hpp>
// #include <rmf_api_msgs/schemas/location.hpp>
#include <rmf_api_msgs/schemas/robot_state.hpp>

namespace free_fleet {

//==============================================================================
class Client::Implementation :
public std::enable_shared_from_this<Client::Implementation>
{
public:

  std::string robot_name;

  std::unique_ptr<RobotHandler> handler;

  std::unique_ptr<transport::Middleware> middleware;

  std::shared_ptr<std::unordered_map<std::string, nlohmann::json>>
  schema_dictionary = {};

  nlohmann::json_schema::json_validator make_validator(
    const nlohmann::json& schema) const;

private:
  void _schema_loader(
    const nlohmann::json_uri& id, nlohmann::json& value) const;
};

//==============================================================================
void Client::Implementation::_schema_loader(
  const nlohmann::json_uri& id, nlohmann::json& value) const
{
  const auto it = schema_dictionary->find(id.url());
  if (it == schema_dictionary->end())
  {
    fferr << "url: " << id.url() << " not found in schema dictionary.\n";
    return;
  }

  value = it->second;
}

//==============================================================================
auto Client::Implementation::make_validator(const nlohmann::json& schema) const
-> nlohmann::json_schema::json_validator
{
  return nlohmann::json_schema::json_validator(
    schema,
    [w = weak_from_this()](const nlohmann::json_uri& id,
    nlohmann::json& value)
    {
      const auto self = w.lock();
      if (!self)
        return;
      self->_schema_loader(id, value);
    });
}

//==============================================================================
auto Client::make(
  const std::string& robot_name,
  std::unique_ptr<RobotHandler> handler,
  std::unique_ptr<transport::Middleware> middleware)
-> std::shared_ptr<Client>
{
  auto make_error_fn = [](const std::string& error_msg)
    {
      fferr << error_msg << "\n";
      return nullptr;
    };

  if (robot_name.empty())
  {
    return make_error_fn("Provided robot name must not be empty.");
  }
  if (!handler)
  {
    return make_error_fn("Provided robot handler cannot be null.");
  }
  if (!middleware)
  {
    return make_error_fn("Provided middleware cannot be null.");
  }

  auto schema_dictionary =
    std::make_shared<std::unordered_map<std::string, nlohmann::json>>();
  const std::vector<nlohmann::json> schemas = {
    rmf_api_msgs::schemas::robot_state
  };
  for (const auto& schema : schemas)
  {
    const auto json_uri = nlohmann::json_uri{schema["$id"]};
    schema_dictionary->insert({json_uri.url(), schema});
  }

  std::shared_ptr<Client> new_client(new Client);
  new_client->_pimpl->robot_name = robot_name;
  new_client->_pimpl->handler = std::move(handler);
  new_client->_pimpl->middleware = std::move(middleware);
  new_client->_pimpl->schema_dictionary = std::move(schema_dictionary);
  return new_client;
}

//==============================================================================
Client::Client()
: _pimpl(rmf_utils::make_unique_impl<Implementation>())
{
}

//==============================================================================
void Client::run_once()
{
  auto new_state = rmf_api_msgs::schemas::robot_state;
  std::string error{};
  if (!_pimpl->handler->current_state(new_state, error))
  {
    fferr << "Failed to obtain new state: " << error << "\n";
  }
  else
  {
    error.clear();

    try
    {
      static const auto validator =
        _pimpl->make_validator(rmf_api_msgs::schemas::robot_state);
      validator.validate(new_state);
    }
    catch (const std::exception& e)
    {
      fferr << "Malformed outgoing robot state json message: "
            << e.what() << "\nMessage:\n" << new_state.dump() << "\n";
      return;
    }

    if (!_pimpl->middleware->publish("state_topic_name",
      new_state.dump(), error))
    {
      fferr << "Failed to publish new state: " << error << "\n";
    }
  }
}

//==============================================================================
} // namespace free_fleet
