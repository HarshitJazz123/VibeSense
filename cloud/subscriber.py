import json
import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS
from influxdb_client.client.write_api import WritePrecision

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
INFLUXDB_TOKEN = "g0325lri35pr71Df7AoF7PvA55lOm83Oqn-O7TBpMWsCF5vxarqYzG-JKzZVAA8sf4CnMZN4GZWTJWhV3yv8xg=="
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

        # ----------------------------------------------------
        # Decode MQTT message
        # ----------------------------------------------------

        payload = json.loads(msg.payload.decode())

        print("\nReceived telemetry:")
        print(json.dumps(payload, indent=2))

        # ----------------------------------------------------
        # Validate required telemetry
        # ----------------------------------------------------

        required_fields = [
            "dev",
            "ts",
            "vib",
            "rpm",
            "curr",
            "temp",
            "hum"
        ]

        for field in required_fields:

            if field not in payload:
                raise ValueError(
                    f"Missing required field: {field}"
                )

        # ----------------------------------------------------
        # Create InfluxDB data point
        # ----------------------------------------------------

        point = (
            Point("telemetry")

            # Tags
            .tag("device", str(payload["dev"]))
            .tag("state", str(payload["state"]))
            .tag("class_name", str(payload["cls_name"]))

            # Main Grafana metrics
            .field("vibration", float(payload["vib"]))
            .field("rpm", float(payload["rpm"]))
            .field("current", float(payload["curr"]))
            .field("temperature", float(payload["temp"]))
            .field("humidity", float(payload["hum"]))

            # Additional data retained for future use
            .field("confidence", float(payload["conf"]))
            .field("mic_range", float(payload["mic_range"]))
            .field("motor_temperature", float(payload["motor_temp"]))

            # Use sensor timestamp
            .time(
                int(payload["ts"]),
                WritePrecision.S
            )
        )

        # ----------------------------------------------------
        # Write to InfluxDB
        # ----------------------------------------------------

        write_api.write(
            bucket=INFLUXDB_BUCKET,
            org=INFLUXDB_ORG,
            record=point
        )

        print("✓ Written to InfluxDB")

    except json.JSONDecodeError:

        print("ERROR: Invalid JSON received")

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