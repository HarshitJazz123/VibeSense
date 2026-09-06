from pathlib import Path
import json

import joblib
from fastapi import FastAPI, Query
from pydantic import BaseModel

from api.database import query_api, INFLUX_BUCKET


# --------------------------------------------------
# APP
# --------------------------------------------------

app = FastAPI(
    title="VibeSense API",
    description="Machine condition monitoring and fault detection API",
    version="1.0.0"
)


# --------------------------------------------------
# ML MODEL
# --------------------------------------------------

BASE_DIR = Path(__file__).resolve().parent.parent
ML_DIR = BASE_DIR / "ml"

MODEL_PATH = ML_DIR / "model_v3.pkl"
SCALER_PATH = ML_DIR / "scaler.pkl"
SCHEMA_PATH = ML_DIR / "feature_schema.json"

model = joblib.load(MODEL_PATH)
scaler = joblib.load(SCALER_PATH)

with open(SCHEMA_PATH, "r") as f:
    feature_schema = json.load(f)

FEATURES = feature_schema["features"]
CLASSES = feature_schema["classes"]


# --------------------------------------------------
# PREDICTION REQUEST
# --------------------------------------------------

class PredictionRequest(BaseModel):
    amps: float
    rms: float
    rpm: float
    temperature: float


# --------------------------------------------------
# HEALTH
# --------------------------------------------------

@app.get("/health")
def health():
    return {
        "status": "ok",
        "service": "VibeSense API"
    }


# --------------------------------------------------
# DEVICES
# --------------------------------------------------

@app.get("/devices")
def get_devices():

    query = f'''
    from(bucket: "{INFLUX_BUCKET}")
      |> range(start: -30d)
      |> filter(fn: (r) => r["_measurement"] == "machine_telemetry")
      |> filter(fn: (r) => exists r["device"])
      |> keep(columns: ["device"])
      |> limit(n: 100)
    '''

    tables = query_api.query(query)

    devices = []

    for table in tables:
        for record in table.records:
            device = record.values.get("device")

            if device is not None and device not in devices:
                devices.append(device)

    return {
        "count": len(devices),
        "devices": devices
    }


# --------------------------------------------------
# TELEMETRY
# --------------------------------------------------

@app.get("/telemetry/{dev}")
def get_telemetry(
    dev: str,
    minutes: int = Query(default=30, ge=1),
    limit: int = Query(default=100, ge=1, le=500)
):

    query = f'''
    from(bucket: "{INFLUX_BUCKET}")
      |> range(start: -{minutes}m)
      |> filter(fn: (r) => r["_measurement"] == "machine_telemetry")
      |> filter(fn: (r) => r["device"] == "{dev}")
      |> filter(fn: (r) =>
          r["_field"] == "amps" or
          r["_field"] == "rms" or
          r["_field"] == "rpm" or
          r["_field"] == "temperature" or
          r["_field"] == "anomaly_score" or
          r["_field"] == "confidence" or
          r["_field"] == "class" or
          r["_field"] == "model_version" or
          r["_field"] == "state" or
          r["_field"] == "class_name"
      )
      |> sort(columns: ["_time"], desc: true)
      |> limit(n: {limit})
      |> pivot(
          rowKey: ["_time"],
          columnKey: ["_field"],
          valueColumn: "_value"
      )
      |> sort(columns: ["_time"], desc: true)
    '''

    tables = query_api.query(query)

    data = []

    for table in tables:
        for record in table.records:

            values = record.values

            row = {
                "time": str(record.get_time()),
                "device": values.get("device")
            }

            for field in [
                "amps",
                "rms",
                "rpm",
                "temperature",
                "anomaly_score",
                "confidence",
                "class",
                "model_version",
                "state",
                "class_name"
            ]:
                if field in values:
                    row[field] = values[field]

            data.append(row)

    return {
        "device": dev,
        "minutes": minutes,
        "limit": limit,
        "count": len(data),
        "data": data
    }

# --------------------------------------------------
# EVENTS
# --------------------------------------------------

@app.get("/events/{dev}")
def get_events(
    dev: str,
    minutes: int = Query(default=43200, ge=1)
):

    query = f'''
    from(bucket: "{INFLUX_BUCKET}")
      |> range(start: -{minutes}m)
      |> filter(fn: (r) => r["_measurement"] == "machine_telemetry")
      |> filter(fn: (r) => r["device"] == "{dev}")
      |> pivot(
          rowKey: ["_time"],
          columnKey: ["_field"],
          valueColumn: "_value"
      )
    '''

    tables = query_api.query(query)

    records = []

    for table in tables:
        for record in table.records:
            records.append(record)

    # Sort all records globally by time
    records.sort(key=lambda r: r.get_time())

    events = []
    previous_state = None

    for record in records:

        values = record.values
        current_state = values.get("state")

        if current_state is None:
            continue

        if current_state != previous_state:

            events.append({
                "time": str(record.get_time()),
                "device": values.get("device"),
                "state": current_state,
                "class_name": values.get("class_name"),
                "class": values.get("class"),
                "confidence": values.get("confidence"),
                "anomaly_score": values.get("anomaly_score")
            })

            previous_state = current_state

    return {
        "device": dev,
        "minutes": minutes,
        "count": len(events),
        "events": events
    }


# --------------------------------------------------
# PREDICTION
# --------------------------------------------------

@app.post("/predict")
def predict(request: PredictionRequest):

    input_data = {
        "amps": request.amps,
        "rms": request.rms,
        "rpm": request.rpm,
        "temperature": request.temperature
    }

    # Build features in exactly the order used during training
    features = [[input_data[feature] for feature in FEATURES]]

    # Scale using the saved scaler
    scaled_features = scaler.transform(features)

    # Prediction
    prediction = model.predict(scaled_features)[0]

    # Prediction probabilities
    probabilities_array = model.predict_proba(scaled_features)[0]

    probabilities = {
        class_name: round(float(probability), 4)
        for class_name, probability in zip(CLASSES, probabilities_array)
    }

    confidence = probabilities[prediction]

    return {
        "prediction": prediction,
        "confidence": confidence,
        "probabilities": probabilities
    }

