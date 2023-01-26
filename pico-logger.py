import serial
import os
import warnings
from json import loads
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
from time import sleep


def main():
    ser = serial.Serial("COM4", 115200, timeout=0.1)

    VESSEL_ID = os.environ.get("VESSEL_ID", "unknown vessel")
    if VESSEL_ID == "unknown vessel":
        warnings.warn("VESSEL_ID is not set in environment variables!")

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
        write_api.write(bucket=influx_bucket, org=influx_org, record=p)

        sleep(0.05)


if __name__ == "__main__":
    main()
