#!/usr/bin/env python3
# Copyright 2023 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from typing import Callable, List
from dataclasses import dataclass

import spdlog
import zenoh
from zenoh import config, Reliability, Sample

from free_fleet.transport import Middleware


# @dataclass
# class ZenohInstance:
#     session: zenoh.Session
#     subscriptions: dict[str, zenoh.Subscriber] = {}


# ZENOH_INSTANCES: List[ZenohInstance] = []


class ZenohMiddleware(Middleware):
    session = None
    subscriptions: dict[str, zenoh.Subscriber] = {}

    # @staticmethod
    # def subscribe(topic: str, callback


    def __init__(self, config_path: str = None):
        self.logger = spdlog.ConsoleLogger('ZenohMiddleware', True, True, True)
        self.config = zenoh.Config()
        if config_path is not None:
            self.logger.info(
                f'Using custom zenoh configuration file: {config_path}')
            self.config = zenoh.Config.from_file(config_path)

        zenoh.init_logger()
        self.session = zenoh.open(self.config)
        self.subscriptions: dict[str, zenoh.Subscriber] = {}

    def __del__(self):
        for _, sub in self.subscriptions.items():
            sub.undeclare()
        self.session.close()

    def publish(self, topic: str, payload: str) -> bool:
        self.session.put(topic, payload)
        return True

    def test_subscribe(self, topic: str) -> bool:
        def new_sub_func(sample: Sample) -> None:
            sample_str = sample.payload.decode('utf-8')
            print(sample_str)

        self.subscriptions[topic] = self.session.declare_subscriber(
            topic, new_sub_func, reliability=Reliability.RELIABLE())
        return True

    def subscribe(self, topic: str, callback: Callable[[str], None]) -> bool:
        # def new_sub_func(sample: Sample) -> None:
        #     # inside_topic = topic
        #     # self.logger.info(f'topic: {inside_topic}')
        #     sample_str = sample.payload.decode('utf-8')
        #     print(sample_str)
        #     # inside_callback = callback
        #     # inside_callback(sample_str)


        # self.subscriptions[topic] = self.session.declare_subscriber(
        #     topic, new_sub_func, reliability=Reliability.RELIABLE())
        self.subscriptions[topic] = self.session.declare_subscriber(
            topic, callback, reliability=Reliability.RELIABLE())
        return True
