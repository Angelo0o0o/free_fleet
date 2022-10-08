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

#ifndef INCLUDE__FREE_FLEET__CLIENT__CLIENT_HPP
#define INCLUDE__FREE_FLEET__CLIENT__CLIENT_HPP

#include <memory>

#include <rmf_utils/impl_ptr.hpp>

#include <free_fleet/Worker.hpp>
#include <free_fleet/client/RobotHandler.hpp>
#include <free_fleet/transport/Middleware.hpp>

namespace free_fleet {

class Client : public Worker
{
public:

  static std::shared_ptr<Client> make(
    const std::string& robot_name,
    std::unique_ptr<RobotHandler> handler,
    std::unique_ptr<transport::Middleware> middleware);

  /// Running the operations of the client once.
  void run_once() override;

  class Implementation;
private:
  Client();
  rmf_utils::unique_impl_ptr<Implementation> _pimpl;
};

} // namespace free_fleet

#endif // INCLUDE__FREE_FLEET__CLIENT__CLIENT_HPP
