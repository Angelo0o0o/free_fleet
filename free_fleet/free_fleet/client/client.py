from abc import ABC, abstractmethod
from typing import Any, Tuple, Optional

import rclpy
from rclpy.node import Node

from tf2_ros import TransformException
from tf2_ros.buffer import Buffer
from tf2_ros.transform_listener import TransformListener

# ==============================================================================
# Abstract Interfaces

class State(ABC):
    @property
    @abstractmethod
    def name(self) -> str:
        raise NotImplementedError

    @property
    @abstractmethod
    def value(self) -> Any:
        raise NotImplementedError

    @value.setter
    @abstractmethod
    def value(self, value: Any) -> bool:
        raise NotImplementedError

    # TODO(AA): figure out how this can be implemented. Perhaps it uses the time
    # of last received state. Every state has a different interval hence it
    # needs to be implemented separately.
    # @abstractmethod
    # def health(self) -> float:
    #     raise NotImplementedError

# ==============================================================================
# State implementations

class Pose(State):
    def __init__(self, ros_node: rclpy.node.Node, robot_frame: str,
            map_frame: str, interval_sec: float):
        self.logger = ros_node.get_logger()
        self.robot_frame = robot_frame
        self.map_frame = map_frame

        self.tf_buffer = Buffer()
        self.transform_listener = TransformListener(self.tf_buffer, ros_node)
        self.timer = ros_node.create_timer(interval_sec, self.on_timer)
        self.trans = None

    def on_timer(self) -> None:
        try:
            now = rclpy.time.Time()
            trans = self.tf_buffer.lookup_transform(
                self.robot_frame, self.map_frame, now)
        except TransformException as ex:
            self.logger.info(
                f'Could not transform {self.robot_frame} to {self.map_frame}: {ex}')
            return

    @property
    def name(self):
        return 'pose'

    @property
    def value(self) -> Optional[Tuple[float, float, float]]:
        # return None
        return 1.0, 2.0, 3.0

    @value.setter
    def value(self, value: Tuple[float, float, float]) -> bool:
        self.value = value
        return True


class BatteryLevel(State):

    def __init__(self):
        # self.name = 'BatteryLevel'
        self.battery_level = None

    @property
    def name(self) -> str:
        # return self.name
        return 'rando'

    @property
    def value(self) -> Optional[float]:
        return self.battery_level

    @value.setter
    def value(self, battery_level: float) -> bool:
        if battery_level is None:
            return False
        self.battery_level = battery_level
        return True

# ==============================================================================
class Client:
    def __init__(self, name, ros_node, states, features):
        # during each run_once, run all features once, passing the states into
        # them
        raise NotImplementedError

    def run_async(self):
        raise NotImplementedError

    def run(self):
        raise NotImplementedError

# ==============================================================================
def main():
    # test = State('empty') # this fails duh
    # test = BatteryLevel()
    # print(test.name)
    # print(test.value)
    # test.value = 123.123
    # print(test.value)
    rclpy.init()
    node = Node('test_client')
    pose = Pose(node, 'robot_frame', 'map_frame', 0.5)

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    rclpy.shutdown()


if __name__ == '__main__':
    main()
