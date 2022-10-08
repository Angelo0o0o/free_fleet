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

#ifndef INCLUDE__FREE_FLEET__CLIENT__STATEMONITOR_HPP
#define INCLUDE__FREE_FLEET__CLIENT__STATEMONITOR_HPP

#include <string>
#include <optional>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

namespace free_fleet {

class StateMonitor
{
public:

  virtual bool current_state(
    nlohmann::json& state,
    std::string& error) const = 0;
};

} // namespace free_fleet

#endif // INCLUDE__FREE_FLEET__CLIENT__STATEMONITOR_HPP
