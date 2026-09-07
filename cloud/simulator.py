import json
import time
import random
import paho.mqtt.client as mqtt

# ============================================================
# HiveMQ Configuration
# ============================================================

BROKER_HOST = "9dfd4467e7294b6484f873164993a9f4.s1.eu.hivemq.cloud"
BROKER_PORT = 8883

USERNAME = "VibeSense2026"
PASSWORD = "VibeSense@2026"

TOPIC_TELEMETRY = "vibesense/node01/telemetry"

# ============================================================
# MQTT Client
# ============================================================

client = mqtt.Client(
    mqtt.CallbackAPIVersion.VERSION2,
    client_id="sim01"
)

client.username_pw_set(USERNAME, PASSWORD)

# Secure TLS connection
client.tls_set()

# Connect to HiveMQ
client.connect(BROKER_HOST, BROKER_PORT)

# Start MQTT network loop
client.loop_start()

print("Simulator running. Press Ctrl+C to stop.")

# ============================================================
# Main Loop
# ============================================================

try:

    while True:

        # ----------------------------------------------------
        # Generate telemetry
        # ----------------------------------------------------

        vibration = round(
            random.uniform(0.9, 1.2),
            3
        )

        rpm = 12800 + random.randint(-100, 100)

        current = round(
            random.uniform(1.7, 1.9),
            2
        )

        temperature = round(
            random.uniform(32.0, 33.0),
            1
        )

        humidity = round(
            random.uniform(80.0, 86.0),
            1
        )

        mic_range = random.randint(800, 900)

        motor_temperature = round(
            random.uniform(33.5, 35.0),
            1
        )

        confidence = round(
            random.uniform(0.90, 0.97),
            2
        )

        # ----------------------------------------------------
        # Complete VibeSense telemetry contract
        # ----------------------------------------------------

        payload = {

            "dev": "node01",

            "ts": int(time.time()),

            "state": "NORMAL",

            "cls_name": "healthy",

            "conf": confidence,

            "vib": vibration,

            "rpm": rpm,

            "mic_range": mic_range,

            "curr": current,

            "temp": temperature,

            "hum": humidity,

            "motor_temp": motor_temperature
        }

        # ----------------------------------------------------
        # Publish telemetry
        # ----------------------------------------------------

        client.publish(
            TOPIC_TELEMETRY,
            json.dumps(payload)
        )

        # ----------------------------------------------------
        # Terminal output
        # ----------------------------------------------------

        print(json.dumps(payload))

        # One message per second
        time.sleep(1)

except KeyboardInterrupt:

    print("\nStopping simulator...")

    client.loop_stop()
    client.disconnect()