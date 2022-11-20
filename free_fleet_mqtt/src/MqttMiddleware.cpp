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

#include <optional>
#include <unordered_map>

#include <free_fleet/Console.hpp>
#include <free_fleet_mqtt/MqttMiddleware.hpp>

#include <mqtt/async_client.h>

#include "CallbackHandler.hpp"

namespace free_fleet {
namespace transport {

class MqttMiddleware::Implementation : public virtual mqtt::callback
{
public:
  std::shared_ptr<mqtt::async_client> cli;

  std::unique_ptr<CallbackHandler> cb_handler;
};

MqttMiddleware::MqttMiddleware()
: _pimpl(rmf_utils::make_unique_impl<Implementation>())
{
}

MqttMiddleware::~MqttMiddleware()
{
  if (_pimpl->cli->is_connected())
  {
    _pimpl->cli->disconnect()->wait();
    ffinfo << "Disconnected from MQTT server.\n";
  }
}

std::unique_ptr<MqttMiddleware> MqttMiddleware::make(
  const std::string& server_address,
  const std::string& client_id)
{
  auto conn_opts = mqtt::connect_options_builder()
    .clean_session(true)
    .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
    .finalize();
  auto cli = std::make_shared<mqtt::async_client>(server_address, client_id);
  auto cb_handler = std::make_unique<CallbackHandler>(cli, conn_opts);
  cli->set_callback(*cb_handler);

  try
  {
    if (!cli->connect(conn_opts)->wait_for(5000))
    {
      fferr << "Unable to connect to MQTT server: " << server_address << ".\n";
      return nullptr;
    }
  }
  catch (const mqtt::exception& exc)
  {
    fferr << "Unable to connect to MQTT server: " << server_address << ": "
          << exc << ".\n";
    return nullptr;
  }
  ffinfo << "Connected to MQTT server: " << server_address << ".\n";

  std::unique_ptr<MqttMiddleware> new_middleware(new MqttMiddleware);
  new_middleware->_pimpl->cli = std::move(cli);
  new_middleware->_pimpl->cb_handler = std::move(cb_handler);
  return new_middleware;
}

bool MqttMiddleware::publish(
  const std::string& topic,
  const std::string& payload,
  std::string& error)
{
  (void)error;
  _pimpl->cli->publish(topic, payload, 1, false);
  return true;
}

bool MqttMiddleware::subscribe(
  const std::string& topic,
  std::function<void(const std::string& payload)> callback,
  std::string& error)
{
  if (!callback)
  {
    error = std::string("Attempted to subscribe with nullopt callback.");
    fferr << error << "\n";
    return false;
  }

  _pimpl->cb_handler->add_subscription(topic, std::move(callback));
  return true;
}

} // namespace transport
} // namespace free_fleet
