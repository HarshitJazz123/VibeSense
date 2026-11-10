# Backend and Dashboard

Full description with figures: [VibeSense_Pro_Full_Report.pdf](VibeSense_Pro_Full_Report.pdf), section "Cloud Backend & Live Dashboard". This file is the short how-to.

```
XIAO ESP32-S3 --MQTT/TLS--> HiveMQ Cloud --> Python subscriber --> InfluxDB 2.x --> Grafana
```

## Components
| Component | Role | Details |
|---|---|---|
| HiveMQ Cloud | MQTT broker | TLS port 8883, topic `vibesense/node01/telemetry`, about every 5 s while RPM > 200 |
| `cloud/subscriber.py` | ingestion | parses JSON, writes points to InfluxDB |
| `cloud/simulator.py` | testing | publishes fake telemetry (no hardware needed) |
| InfluxDB 2.x | storage | org `vibesense-org`, bucket `vibesense`, measurement `telemetry`, local (port 8086) |
| Grafana | dashboard | "VibeSense Dashboard", Flux queries, 5 s refresh (port 3000) |

## Data model
- Tags: `device` (node01), `class_name` (for example `MotorA_Healthy`, `MotorA_Overload`)
- Fields: `vibration`, `rpm`, `current`, `temperature`, `humidity` (the bucket also has `motor_temperature`, `mic_range`)
- Timestamp: server receive time (device `ts` is relative to boot)

## Dashboard panels
Vibration Level (time series), Motor Speed RPM (gauge per class), Current Draw (time series), Motor Temperature (time series), Humidity (stat). All filter on `device = node01`.

## Run it
1. Start InfluxDB and Grafana locally; create org, bucket `vibesense` and an API token.
2. Add InfluxDB to Grafana (Flux) and import the JSON from `dashboard/`.
3. Set broker host, username, password and the InfluxDB URL/token as environment variables, then run `python cloud/subscriber.py`.
4. No hardware: run `python cloud/simulator.py`.

## Security
No credentials in this repo. They are shared privately and rotated when needed.

## Known limitations
- Runs on a laptop (local), not hosted.
- Current is reserved at 0.0 in firmware, so the Current Draw panel is a placeholder.
- Temperature / humidity panels need those readings added to the MQTT payload.
- No alert rules, retention policy or dedicated fault-state panel yet.
