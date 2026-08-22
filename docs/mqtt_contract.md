# VibeSense MQTT Contract

## 1. Telemetry Topic

vibesense/node01/telemetry

## 2. Events Topic

vibesense/node01/events

## 3. Telemetry Message Format

```json
{
  "dev": "node01",                   #Device ID
  "ts": 1784800000,                  #Unix timestamp
  "cls": 0,                          #Numerical fault class
  "cls_name": "healthy",             #Fault/class name
  "state": "NORMAL",                 #Machine state
  "conf": 0.91,                      #ML confidence
  "rms": 0.081,                      #Vibration RMS
  "temp": 41.2,                      #Temperature
  "amps": 1.81,                      #Motor current
  "rpm": 2147,                       #Motor speed
  "anom": 2.3,                       #Anomaly score
  "mv": "v3"                         #ML model version~
}