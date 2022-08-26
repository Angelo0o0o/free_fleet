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

#include <free_fleet/Console.hpp>
#include <free_fleet_mqtt/MqttMiddleware.hpp>

#include <mqtt/async_client.h>

namespace free_fleet {
namespace transport {

class MqttMiddleware::Implementation
{
public:
  std::unique_ptr<mqtt::async_client> cli;
};

MqttMiddleware::MqttMiddleware()
: _pimpl(rmf_utils::make_unique_impl<Implementation>())
{}

std::shared_ptr<MqttMiddleware> MqttMiddleware::make(
  const std::string& server_address,
  const std::string& client_id)
{
  auto conn_opts = mqtt::connect_options_builder()
    .clean_session(true)
    .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
    .finalize();
  auto cli = std::make_unique<mqtt::async_client>(server_address, client_id);
  auto rsp = cli->connect(conn_opts)->get_connect_response();
  ffinfo << "got it!\n";

  std::shared_ptr<MqttMiddleware> new_middleware(new MqttMiddleware);
  new_middleware->_pimpl->cli = std::move(cli);
  return new_middleware;
}

bool MqttMiddleware::publish(
  const std::string& topic,
  const std::string& payload,
  std::string& error)
{
  return true;
}

bool MqttMiddleware::subscribe(
  const std::string& topic,
  std::function<void(const std::string& payload)> callback,
  std::string& error)
{
  return true;
}

} // namespace transport
} // namespace free_fleet
