/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#ifndef INCLUDE__FREE_FLEET__TYPES_HPP
#define INCLUDE__FREE_FLEET__TYPES_HPP

#include <string>
#include <cstdint>

namespace free_fleet {

/// Commnad Ids are represented by an unsigned 32-bit integer. This means that
/// the manager and clients can identify over 4 billion commands, which will
/// most likely be more than each server or robot can store.
///
/// TODO(AA): Handle command culling or exporting in both manager and client.
using CommandId = uint32_t;

struct Location
{
  std::string map_name;
  double x;
  double y;
  double yaw;
};

struct Goal
{
  Location location;
  double speed_limit;
};

} // namespace free_fleet

#endif // INCLUDE__FREE_FLEET__TYPES_HPP
