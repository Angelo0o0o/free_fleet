import rclpy
from rclpy.node import Node

from std_msgs.msg import String
from typing import Callable


class Poop:
    def __init__(self):
        self.msg = 'nothing'

    def update_msg(self, data: str):
        self.msg = data

    def print_msg(self) -> None:
        print(self.msg)


class MinimalSubscriber(Node):
    def __init__(self):
        super().__init__('minimal_subscriber')
        self.subscription = self.create_subscription(
            String,
            'topic',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning
        self.cb = None
        self.print_cb = None

        period = 1.0
        self.timer = self.create_timer(period, self.timer_callback)

    def set_cb(self, cb: Callable[[str], None]) -> None:
        self.cb = cb

    def set_print_cb(self, cb: Callable[[], None]) -> None:
        self.print_cb = cb

    def listener_callback(self, msg) -> None:
        # self.get_logger().info('I heard: "%s"' % msg.data)
        if self.cb is not None:
            self.cb(msg.data)

    def timer_callback(self) -> None:
        if self.print_cb is not None:
            self.print_cb()


def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = MinimalSubscriber()

    poop = Poop()
    minimal_subscriber.set_cb(poop.update_msg)
    minimal_subscriber.set_print_cb(poop.print_msg)

    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_subscriber.obs.on_completed()
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
