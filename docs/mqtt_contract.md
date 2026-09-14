# VibeSense MQTT Contract

## 1. Telemetry Topic

`vibesense/node01/telemetry`

## 2. Events Topic

`vibesense/node01/events`

## 3. Telemetry Message Format

```json
{
  "dev": "node01",
  "ts": 1788700000,
  "state": "NORMAL",
  "cls_name": "healthy",
  "conf": 0.94,
  "vib": 1.05,
  "rpm": 12800,
  "mic_range": 850,
  "curr": 1.85,
  "temp": 32.5,
  "hum": 85.0,
  "motor_temp": 34.2
}