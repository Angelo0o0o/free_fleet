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

from enum import Enum
from typing import Any, Dict, List, Optional, Union

from pydantic import BaseModel, Field, confloat, conlist

from dataclasses import dataclass
from pycdr2 import IdlStruct
from pycdr2.types import int8, int32, uint32, float64


@dataclass
class Vector3(IdlStruct, typename="Vector3"):
    x: float64
    y: float64
    z: float64

# @dataclass
# class Location2D(IdlStruct, typename="Location2D"):

class Location2D(BaseModel):
    map: str
    x: float
    y: float
    yaw: Optional[float] = None


class Waypoint2D(BaseModel):
    index: int
    location: Location2D
    wait_until: Optional[int] = None


class RobotState(Enum):
    uninitialized = "uninitialized"
    offline = "offline"
    shutdown = "shutdown"
    idle = "idle"
    charging = "charging"
    working = "working"
    error = "error"


class RobotState(BaseModel):
    name: str
    status: RobotState = RobotState.uninitialized
    task_id: Optional[str] = None
    task_history: Optional[
        conlist(item_type=str, max_items=10, unique_items=True)] = None
    time: Optional[int] = None
    battery_percent: Optional[confloat(ge=0.0, le=1.0)] = None
    location: Optional[Location2D] = None


class PauseRequest(BaseModel):
    task_id: str


class ResumeRequest(BaseModel):
    task_id: str


class NavigationRequest(BaseModel):
    task_id: str


class RelocalizeRequest(BaseModel):
    task_id: str


class ActionRequest(BaseModel):
    task_id: str
    action_name: str
    # TODO: action_parameters
