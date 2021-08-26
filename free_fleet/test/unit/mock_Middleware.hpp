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

#ifndef TEST__UNIT__MOCK_MIDDLEWARE_HPP
#define TEST__UNIT__MOCK_MIDDLEWARE_HPP

#include <free_fleet/transport/Middleware.hpp>

namespace free_fleet {

class MockMiddleware : public transport::Middleware
{
public:

  MockMiddleware()
  {}

  void send_state(const messages::RobotState&) final
  {}

  std::vector<messages::RobotState> read_states() override
  {
    return {};
  }

  void send_mode_request(const messages::ModeRequest& request) final
  {
    _prev_mode_request = request;
  }

  rmf_utils::optional<messages::ModeRequest> read_mode_request() final
  {
    return rmf_utils::nullopt;
  }

  void send_navigation_request(const messages::NavigationRequest& request) final
  {
    _prev_nav_request = request;
  }

  rmf_utils::optional<messages::NavigationRequest>
    read_navigation_request() final
  {
    return rmf_utils::nullopt;
  }

  void send_relocalization_request(
    const messages::RelocalizationRequest& request) final
  {
    _prev_reloc_request = request;
  }

  rmf_utils::optional<messages::RelocalizationRequest>
    read_relocalization_request() final
  {
    return rmf_utils::nullopt;
  }

  messages::ModeRequest _prev_mode_request;
  messages::NavigationRequest _prev_nav_request;
  messages::RelocalizationRequest _prev_reloc_request;
};

} // namespace free_fleet

#endif // TEST__UNIT__MOCK_MIDDLEWARE_HPP
