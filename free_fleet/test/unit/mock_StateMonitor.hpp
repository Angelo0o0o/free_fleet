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

#ifndef TEST__UNIT__MOCK_STATEMONITOR_HPP
#define TEST__UNIT__MOCK_STATEMONITOR_HPP

#include <chrono>
#include <free_fleet/client/StateMonitor.hpp>
#include <rmf_api_msgs/schemas/robot_state.hpp>

namespace free_fleet {

class MockStateMonitor : public StateMonitor
{
public:

  MockStateMonitor()
  {}

  bool current_state(
    nlohmann::json& state,
    std::string& error) const final
  {
    state = rmf_api_msgs::schemas::robot_state;
    (void)error;
    return true;
  }
};

} // namespace free_fleet

#endif // TEST__UNIT__MOCK_STATEMONITOR_HPP
