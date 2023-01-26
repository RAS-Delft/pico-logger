import serial
import os
from json import loads
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS


def main():
    ser = serial.Serial("COM4", 115200, timeout=0.1)

    influx_host = os.environ.get("INFLUX_HOST", None)
    influx_user = os.environ.get("INFLUX_USER", "ras")
    influx_password = os.environ.get("INFLUX_PASSWORD", None)
    influx_org = os.environ.get("INFLUX_ORG", "RAS Lab")
    influx_bucket = os.environ.get("INFLUX_BUCKET", "ras")

    influx_client = InfluxDBClient(url=f"http://{influx_host}:8086", token=influx_password, org=influx_org)
    write_api = influx_client.write_api(write_options=SYNCHRONOUS)

    while True:
        line = ser.readline()
        if not line:
            continue
        d = loads(line)
        print(d)

        # p = Point("location").tag("vessel", name) \
        #     .field("latitude", data.latitude) \
        #     .field("longitude", data.longitude) \
        #     .field("altitude", data.altitude)
        # write_api.write(bucket=influx_bucket, org=influx_org, record=p)


if __name__ == "__main__":
    main()
