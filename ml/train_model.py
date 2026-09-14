import json
from pathlib import Path

import joblib
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

from api.database import query_api, INFLUX_BUCKET


# --------------------------------------------------
# 1. Load labeled sensor data from InfluxDB
# --------------------------------------------------

query = f'''
from(bucket: "{INFLUX_BUCKET}")
  |> range(start: -30d)
  |> filter(fn: (r) => r["_measurement"] == "machine_telemetry")
  |> filter(fn: (r) =>
      r["_field"] == "amps" or
      r["_field"] == "rms" or
      r["_field"] == "rpm" or
      r["_field"] == "temperature"
  )
  |> pivot(
      rowKey: ["_time"],
      columnKey: ["_field"],
      valueColumn: "_value"
  )
  |> filter(fn: (r) => exists r["class_name"])
  |> keep(columns: [
      "_time",
      "amps",
      "rms",
      "rpm",
      "temperature",
      "class_name"
  ])
'''

print("Reading training data from InfluxDB...")

tables = query_api.query(query)

rows = []

for table in tables:
    for record in table.records:
        values = record.values

        rows.append({
            "time": record.get_time(),
            "amps": values.get("amps"),
            "rms": values.get("rms"),
            "rpm": values.get("rpm"),
            "temperature": values.get("temperature"),
            "class_name": values.get("class_name")
        })


df = pd.DataFrame(rows)

print(f"Total rows loaded: {len(df)}")


# --------------------------------------------------
# 2. Clean the dataset
# --------------------------------------------------

features = [
    "amps",
    "rms",
    "rpm",
    "temperature"
]

target = "class_name"

df = df.dropna(subset=features + [target])

print(f"Rows after removing missing values: {len(df)}")

print("\nClass distribution:")
print(df[target].value_counts())


# --------------------------------------------------
# 3. Prepare X and y
# --------------------------------------------------

X = df[features]
y = df[target]


# --------------------------------------------------
# 4. Split training and testing data
# --------------------------------------------------

X_train, X_test, y_train, y_test = train_test_split(
    X,
    y,
    test_size=0.20,
    random_state=42,
    stratify=y
)


# --------------------------------------------------
# 5. Scale the sensor features
# --------------------------------------------------

scaler = StandardScaler()

X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)


# --------------------------------------------------
# 6. Train Random Forest classifier
# --------------------------------------------------

model = RandomForestClassifier(
    n_estimators=200,
    random_state=42,
    class_weight="balanced",
    n_jobs=-1
)

print("\nTraining Random Forest model...")

model.fit(X_train_scaled, y_train)


# --------------------------------------------------
# 7. Evaluate the model
# --------------------------------------------------

y_pred = model.predict(X_test_scaled)

accuracy = accuracy_score(y_test, y_pred)

print("\n==============================")
print("MODEL RESULTS")
print("==============================")

print(f"Accuracy: {accuracy:.4f}")

print("\nClassification Report:")
print(classification_report(y_test, y_pred))

print("Confusion Matrix:")
print(confusion_matrix(y_test, y_pred))


# --------------------------------------------------
# 8. Save model and scaler
# --------------------------------------------------

ml_folder = Path(__file__).resolve().parent

model_path = ml_folder / "model_v3.pkl"
scaler_path = ml_folder / "scaler.pkl"
schema_path = ml_folder / "feature_schema.json"

joblib.dump(model, model_path)
joblib.dump(scaler, scaler_path)

schema = {
    "features": features,
    "target": target,
    "classes": sorted(y.unique().tolist())
}

with open(schema_path, "w") as f:
    json.dump(schema, f, indent=4)


# --------------------------------------------------
# 9. Final information
# --------------------------------------------------

print("\n==============================")
print("FILES SAVED")
print("==============================")

print(f"Model : {model_path}")
print(f"Scaler: {scaler_path}")
print(f"Schema: {schema_path}")

print("\nML training completed successfully.")