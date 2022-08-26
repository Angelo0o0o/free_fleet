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

#ifndef TEST__UNIT__MOCK_MIDDLEWARE_HPP
#define TEST__UNIT__MOCK_MIDDLEWARE_HPP

#include <chrono>
#include <free_fleet/transport/Middleware.hpp>

namespace free_fleet {
namespace transport {

class MockMiddleware : public Middleware
{
public:

  MockMiddleware()
  {}

  bool publish(
    const std::string& topic,
    const std::string& payload,
    std::string& error) final
  {
    (void)topic;
    (void)payload;
    (void)error;
    return true;
  }

  bool subscribe(
    const std::string& topic,
    std::function<void(const std::string& payload)> callback,
    std::string& error) final
  {
    (void)topic;
    (void)callback;
    (void)error;
    return true;
  }
};

} // namespace transport
} // namespace free_fleet

#endif // TEST__UNIT__MOCK_MIDDLEWARE_HPP
