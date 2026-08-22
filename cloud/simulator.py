import json
import time
import random
import paho.mqtt.client as mqtt

# HiveMQ Configuration

BROKER_HOST = "9dfd4467e7294b6484f873164993a9f4.s1.eu.hivemq.cloud"
BROKER_PORT = 8883

USERNAME = "VibeSense2026"
PASSWORD = "VibeSense@2026"

TOPIC_TELEMETRY = "vibesense/node01/telemetry"
TOPIC_EVENTS = "vibesense/node01/events"

# MQTT Client

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

# Machine State

state = "NORMAL"
fault_step = 0

# Keep False for the first test
in_fault = True


print("Simulator running. Press Ctrl+C to stop.")


# Main Loop

try:

    while True:

        # Generate vibration
        
        if not in_fault:

            rms = round(
                random.uniform(0.07, 0.09),
                3
            )

        else:

            fault_step += 1

            rms = round(
                0.09 + fault_step * 0.05,
                3
            )

            if fault_step == 5:
                state = "SUSPECT"

            if fault_step >= 15:
                state = "CONFIRMED"


        # Create telemetry payload
        
        payload = {

            "dev": "node01",

            "ts": int(time.time()),

            "cls": 1 if in_fault else 0,

            "cls_name":
                "inner_race"
                if in_fault
                else "healthy",

            "state": state,

            "conf": round(
                random.uniform(0.85, 0.97),
                2
            ),

            "rms": rms,

            "temp": round(
                41 + random.uniform(-0.3, 0.3),
                1
            ),

            "amps": round(
                1.8 + random.uniform(-0.1, 0.1),
                2
            ),

            "rpm":
                2140 + random.randint(-20, 20),

            "anom": round(
                random.uniform(0.5, 5.0),
                1
            ),

            "mv": "v3"
        }

        # Publish JSON to HiveMQ
        
        client.publish(
            TOPIC_TELEMETRY,
            json.dumps(payload)
        )

        # Show data in terminal
        print(json.dumps(payload))


        # Send one message per second
        time.sleep(1)


except KeyboardInterrupt:

    print("Stopping simulator.")

    client.loop_stop()
    client.disconnect()