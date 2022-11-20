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

#ifndef INCLUDE__FREE_FLEET_MQTT__MQTTMIDDLEWARE_HPP
#define INCLUDE__FREE_FLEET_MQTT__MQTTMIDDLEWARE_HPP

#include <memory>

#include <rmf_utils/impl_ptr.hpp>
#include <free_fleet/transport/Middleware.hpp>

namespace free_fleet {
namespace transport {

class MqttMiddleware : public Middleware
{
public:

  static std::unique_ptr<MqttMiddleware> make(
    const std::string& server_address,
    const std::string& client_id);

  bool publish(
    const std::string& topic,
    const std::string& payload,
    std::string& error) final;

  bool subscribe(
    const std::string& topic,
    std::function<void(const std::string& payload)> callback,
    std::string& error) final;

  ~MqttMiddleware();

  class Implementation;
private:
  MqttMiddleware();
  rmf_utils::unique_impl_ptr<Implementation> _pimpl;
};

} // namespace transport
} // namespace free_fleet

#endif // INCLUDE__FREE_FLEET_MQTT__MQTTMIDDLEWARE_HPP