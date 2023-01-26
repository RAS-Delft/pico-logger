import serial
import os
import warnings
from json import loads
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
from time import sleep

import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray, Float32


VESSEL_ID = os.environ.get("VESSEL_ID", "unknown vessel")
if VESSEL_ID == "unknown vessel":
    warnings.warn("VESSEL_ID is not set in environment variables!")


class PicoLogger(Node):
    def __init__(self):
        super(PicoLogger, self).__init__(f"{VESSEL_ID}_pico_logger")

        self.get_logger().info("Start setting up subscribers and publishers")
        self.pub = self.create_publisher(Float32, f'/{VESSEL_ID}/electric-measurements', 10)

        self.get_logger().info("Done setting up subscribers and publishers")
        self.get_logger().info("Running main loop now")

        self.ser = serial.Serial("COM4", 115200, timeout=0.1)

        self._influx_host = os.environ.get("INFLUX_HOST", None)
        self._influx_user = os.environ.get("INFLUX_USER", "ras")
        self._influx_password = os.environ.get("INFLUX_PASSWORD", None)
        self._influx_org = os.environ.get("INFLUX_ORG", "RAS Lab")
        self._influx_bucket = os.environ.get("INFLUX_BUCKET", "ras")

        influx_client = InfluxDBClient(url=f"http://{self._influx_host}:8086", token=self._influx_password, org=self._influx_org)
        self.write_api = influx_client.write_api(write_options=SYNCHRONOUS)

        """
        End of initialisation, start of program
        """

        self.timer = self.create_timer(1/200, self.loop_body)

    def loop_body(self):
        line = self.ser.readline()
        if not line:
            return
        d = loads(line)
        print(d)

        l = [v for v in d['voltages']] + [v for v in d['currents']]
        msg_to_send = Float32MultiArray()
        msg_to_send.data = l
        self.pub.publish(msg_to_send)

        fields = {f"voltage_{i}": val for i, val in enumerate(d['voltages'])} | {f"current_{i}": val for i, val in enumerate(d['currents'])}

        point = {
            "measurement": "vessel_electric_measurement",
            "tags": {"vessel": VESSEL_ID},
            "fields": fields
        }

        # p = Point("location").tag("vessel", name) \
        #     .field("latitude", data.latitude) \
        #     .field("longitude", data.longitude) \
        #     .field("altitude", data.altitude)

        p = Point.from_dict(point, WritePrecision.NS)
        self.write_api.write(bucket=self._influx_bucket, org=self._influx_org, record=p)

        sleep(0.05)

    def exit_loop(self):
        self.timer.destroy()
        self.destroy_node()
        rclpy.shutdown()
        exit()


def main(args=None):
    print("Initializing ROS node...")
    rclpy.init(args=args)

    node = PicoLogger()

    print("Done initializing ROS node")

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print('\nNode stopped cleanly')
    except BaseException:
        import sys
        print('Exception in node:', file=sys.stderr)
        raise
    finally:
        # Destroy the node explicitly
        # (optional - otherwise it will be done automatically
        # when the garbage collector destroys the node object)
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
