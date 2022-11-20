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

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <thread>
#include <chrono>

#include <free_fleet/Console.hpp>
#include <free_fleet_mqtt/MqttMiddleware.hpp>

volatile sig_atomic_t stop;

void inthand(int signum)
{
    stop = 1;
}

int main(int argc, char** argv)
{
  signal(SIGINT, inthand);

  if (argc != 2)
  {
    fferr << "no topic provided.\n";
    return 1;
  }

  auto middleware = free_fleet::transport::MqttMiddleware::make(
    "tcp://localhost:1883",
    "MqttMiddleware_sub");

  auto sub_callback = [](const std::string& payload)
  {
    ffinfo << payload << "\n";
  };

  std::string error;
  if (!middleware->subscribe(std::string(argv[1]), sub_callback, error))
  {
    fferr << error << "\n";
    return 1;
  }

  while(!stop)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  return 0;
}
