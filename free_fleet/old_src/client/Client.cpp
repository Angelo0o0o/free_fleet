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

#include <rmf_utils
#include <free_fleet/Console.hpp>
#include <free_fleet/client/Client.hpp>

namespace free_fleet {

//==============================================================================
class Client::Implementation
{
public:

  std::string robot_name;

  std::shared_ptr<client::StateMonitor> state_monitor;

  std::unique_ptr<transport::Middleware> middleware;  
}

//==============================================================================
std::shared_ptr<Client> Client::make(
  const std::string& robot_name,
  std::unique_ptr<client::StateMonitor> state_monitor,
  std::unique_ptr<transport::Middleware> middleware)
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
  if (!state_monitor)
  {
    return make_error_fn("Provided state monitor cannot be null.");
  }
  if (!middleware)
  {
    return make_error_fn("Provided middleware cannot be null.");
  }

  std::shared_ptr<Client> new_client(new Client);
  new_client->_pimpl = rmf_utils::make_impl<Implementation>(
    Implementation{
      robot_name,
      std::move(state_monitor),
      std::move(middleware)});
  return new_client;
}

//==============================================================================
Client::Client()
{
}

//==============================================================================
void Client::run_once()
{

}

//==============================================================================
} // namespace free_fleet
