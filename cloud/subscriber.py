import json
import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

# ============================================================
# HiveMQ Configuration
# ============================================================

BROKER_HOST = "9dfd4467e7294b6484f873164993a9f4.s1.eu.hivemq.cloud"
BROKER_PORT = 8883

USERNAME = "VibeSense2026"
PASSWORD = "VibeSense@2026"

TOPIC_TELEMETRY = "vibesense/node01/telemetry"

# ============================================================
# InfluxDB Configuration
# ============================================================

INFLUXDB_URL = "http://localhost:8086"
INFLUXDB_TOKEN = "OnMRoIq7kbM8IZmDCNcsrlEmEdyJcC93o9eDH0fe5VVRHUjBKBSARFYBjSaJGTyq6Uv2cNpNMPKxqV0VLPhLrw=="
INFLUXDB_ORG = "vibesense-org"
INFLUXDB_BUCKET = "vibesense"

# ============================================================
# InfluxDB Client
# ============================================================

influx_client = InfluxDBClient(
    url=INFLUXDB_URL,
    token=INFLUXDB_TOKEN,
    org=INFLUXDB_ORG
)

write_api = influx_client.write_api(
    write_options=SYNCHRONOUS
)


# ============================================================
# MQTT Callbacks
# ============================================================

def on_connect(client, userdata, flags, reason_code, properties):
    print("Connected to HiveMQ.")
    print("Subscribing to:", TOPIC_TELEMETRY)

    client.subscribe(TOPIC_TELEMETRY)

    print("Waiting for telemetry messages...")


def on_message(client, userdata, msg):

    try:

        # Decode MQTT message
        payload = json.loads(msg.payload.decode())

        print("\nReceived telemetry:")
        print(json.dumps(payload, indent=2))

        # ----------------------------------------------------
        # Create InfluxDB data point
        # ----------------------------------------------------

        point = (
            Point("machine_telemetry")

            # Tags
            .tag("device", payload["dev"])
            .tag("state", payload["state"])
            .tag("class_name", payload["cls_name"])

            # Fields
            .field("class", payload["cls"])
            .field("confidence", payload["conf"])
            .field("rms", payload["rms"])
            .field("temperature", payload["temp"])
            .field("amps", payload["amps"])
            .field("rpm", payload["rpm"])
            .field("anomaly_score", payload["anom"])
            .field("model_version", payload["mv"])

        )

        # Write to InfluxDB
        write_api.write(
            bucket=INFLUXDB_BUCKET,
            org=INFLUXDB_ORG,
            record=point
        )

        print("✓ Written to InfluxDB")

    except Exception as e:

        print("ERROR:", e)


# ============================================================
# MQTT Client
# ============================================================

client = mqtt.Client(
    mqtt.CallbackAPIVersion.VERSION2,
    client_id="subscriber01"
)

client.username_pw_set(
    USERNAME,
    PASSWORD
)

# TLS
client.tls_set()

# MQTT callbacks
client.on_connect = on_connect
client.on_message = on_message


# ============================================================
# Connect
# ============================================================

print("Connecting to HiveMQ...")

client.connect(
    BROKER_HOST,
    BROKER_PORT
)


# ============================================================
# Start MQTT loop
# ============================================================

try:

    client.loop_forever()

except KeyboardInterrupt:

    print("\nStopping subscriber...")

    client.disconnect()
    influx_client.close()