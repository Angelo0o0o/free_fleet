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

#include <rmf_utils/catch.hpp>

#include <free_fleet_mqtt/MqttMiddleware.hpp>

SCENARIO("Verify that an MqttMiddleware can be created")
{
  using namespace free_fleet;
  using namespace transport;

  GIVEN("Valid server")
  {
    auto middleware = MqttMiddleware::make(
      "tcp://localhost:1883",
      "test_free_fleet_mqtt__make_MqttMiddleware");
    CHECK(middleware);
  }

  GIVEN("Invalid server")
  {
    auto middleware = MqttMiddleware::make(
      "tcp://localhostfail:1883",
      "test_free_fleet_mqtt__make_MqttMiddleware");
    CHECK(!middleware);
  }
}
