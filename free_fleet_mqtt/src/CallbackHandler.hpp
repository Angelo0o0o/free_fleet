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

#ifndef SRC__CALLBACKHANLDER_HPP
#define SRC__CALLBACKHANLDER_HPP

#include <mutex>
#include <string>
#include <thread>
#include <functional>
#include <unordered_map>

#include <free_fleet/Console.hpp>

#include <mqtt/async_client.h>

namespace free_fleet {
namespace transport {

class CallbackHandler : public virtual mqtt::callback
{
public:
  CallbackHandler(
    std::shared_ptr<mqtt::async_client> cli,
    mqtt::connect_options& conn_opts)
  : _cli(cli), _conn_opts(conn_opts), _callback_map({})
  {
  }

  void add_subscription(
    const std::string& topic,
    std::function<void(const std::string& payload)> callback)
  {
    auto cli = _cli.lock();
    if (!cli)
      return;

    {
      std::lock_guard<std::mutex> lock(_callback_map_mutex);
      _callback_map[topic] = std::move(callback);
    }
    cli->subscribe(topic, 1);
    ffinfo << "Subscription added for topic: " << topic << "\n";
  }

  void remove_subscription(const std::string& topic)
  {
    std::lock_guard<std::mutex> lock(_callback_map_mutex);
    _callback_map.erase(topic);
  }

private:
  std::weak_ptr<mqtt::async_client> _cli;

  mqtt::connect_options _conn_opts;

  std::unordered_map
  <std::string, std::function<void(const std::string& payload)>> _callback_map;

  std::mutex _callback_map_mutex;

  void connected(const std::string& cause) final
  {
    (void)cause;

    auto cli = _cli.lock();
    if (!cli)
      return;

    ffinfo << "Connection success.\n";
    std::lock_guard<std::mutex> lock(_callback_map_mutex);
    for (auto sub : _callback_map)
    {
      cli->subscribe(sub.first, 1);
    }
  }

  // Callback for when the connection is lost.
  // This will initiate the attempt to manually reconnect.
  void connection_lost(const std::string& cause) final
  {
    fferr << "Connection lost: " << cause << ".\n";
    ffinfo << "Reconnecting...\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    auto cli = _cli.lock();
    try
    {
      cli->connect(_conn_opts);
    }
    catch (const mqtt::exception& exc)
    {
      fferr << "Failed to reconnect to MQTT server: " << exc.what() << "\n";
      exit(1);
    }
  }

  // Callback for when a message arrives.
  void message_arrived(mqtt::const_message_ptr msg) final
  {
    std::lock_guard<std::mutex> lock(_callback_map_mutex);
    auto it = _callback_map.find(msg->get_topic());
    if (it == _callback_map.end())
    {
      ffwarn << "Got an unsubscribed message on topic: " << msg->get_topic()
             << ", ignoring...\n";
      return;
    }
    it->second(msg->to_string());
  }

  void delivery_complete(mqtt::delivery_token_ptr token) final
  {
    (void)token;
  }
};

} // namespace transport
} // namespace free_fleet

#endif // SRC__CALLBACKHANLDER_HPP
