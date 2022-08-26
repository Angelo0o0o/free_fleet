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

#include <rmf_utils/catch.hpp>

#include <free_fleet/client/Client.hpp>

#include "mock_Middleware.hpp"
#include "mock_StateMonitor.hpp"

SCENARIO("Verify that a Client can be created")
{
  using namespace free_fleet;

  std::unique_ptr<StateMonitor> sm(new MockStateMonitor());
  std::unique_ptr<transport::Middleware> m(new transport::MockMiddleware());

  GIVEN("Valid client")
  {
    auto client = free_fleet::Client::make(
      "mock_client",
      std::move(sm),
      std::move(m));
    CHECK(client);
  }
}
