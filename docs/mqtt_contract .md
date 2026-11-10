# MQTT Contract: VibeSense Pro node

Credentials are **not** stored in this repo. They are shared privately with the backend owner and kept in `sensors/secrets.h` (git-ignored) on the device.

## Broker
| Item | Value |
|---|---|
| Broker | HiveMQ Cloud (`<your-cluster>.s1.eu.hivemq.cloud`) |
| Port | 8883 (MQTT over TLS) |
| Username / Password | shared privately |
| Client ID (device) | `XIAO_VibeSense_Node01` |

## Topic
`vibesense/node01/telemetry`

Published about every 5 s while the motor is running (mean RPM over the window > 200). Nothing is published while the motor is inactive.

## Payload (JSON)
```json
{"dev":"node01","ts":123456,"cls_name":"MotorA_Healthy","vib":1.234,"rpm":9840.0,"mic":1100.0,"curr":0.0}
```

| Field | Type | Meaning |
|---|---|---|
| `dev` | string | Device id |
| `ts` | integer | Device `millis()` since boot (not epoch). Backend should use server receive time. |
| `cls_name` | string | Voted class: `MotorA_Healthy`, `MotorA_Unbalance`, `MotorA_Looseness`, `MotorA_Overload` |
| `vib` | float | Mean vibration magnitude over the window (g) |
| `rpm` | float | Mean RPM over the window |
| `mic` | float | Sound range over the last 500 ms window (ADC counts) |
| `curr` | float | Reserved. Currently always `0.0` (ACS712 unreliable at this load) |

## Notes
- `MotorA_Unbalance` is part of the model's label set, but it is not reliable in live use.
- Temperature, humidity and motor temperature are read on the device but are not yet part of this payload.
