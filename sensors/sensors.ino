//all sensors data

// #include <Wire.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <DHT.h>

// // ================= PIN MAP =================
// // A0 -> ACS712 (current)
// // A1 -> MAX4466 (mic)
// // D2 -> DHT22
// // D3 -> DS18B20
// // D4 -> MPU6050 SDA
// // D5 -> MPU6050 SCL
// // D6 -> TCRT5000

// // ---- DHT22 ----
// #define DHTPIN D2
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// // ---- DS18B20 ----
// #define ONE_WIRE_BUS D3
// OneWire oneWire(ONE_WIRE_BUS);
// DallasTemperature sensors(&oneWire);

// // ---- MAX4466 mic (range-based, jaisa working code tha) ----
// #define MIC_PIN A1
// #define MIC_THRESHOLD 900
// int micMax = 0;
// int micMin = 4095;
// unsigned long micWindowStart = 0;
// int lastMicRange = 0;

// // ---- ACS712 ----
// #define ACS_PIN A0

// // ---- TCRT5000 ----
// #define TCRT_PIN D6
// volatile unsigned long pulseCount = 0;
// volatile unsigned long lastInterruptTime = 0;
// unsigned long lastRPMTime = 0;
// unsigned long currentRPM = 0;

// void IRAM_ATTR countPulse() {
//   unsigned long t = micros();
//   if (t - lastInterruptTime > 2000) {   // FIXED: 2000 microsec debounce
//     pulseCount++;
//     lastInterruptTime = t;
//   }
// }

// // ---- MPU6050 (raw register read) ----
// void setupMPU() {
//   Wire.beginTransmission(0x68);
//   Wire.write(0x6B); Wire.write(0);
//   Wire.endTransmission(true);
//   Wire.beginTransmission(0x68);
//   Wire.write(0x1C); Wire.write(0x18);   // +-16g range
//   Wire.endTransmission(true);
// }

// void readMPU(float &ax_g, float &ay_g, float &az_g) {
//   Wire.beginTransmission(0x68);
//   Wire.write(0x3B);
//   Wire.endTransmission(false);
//   Wire.requestFrom(0x68, 6);
//   int16_t axr = Wire.read() << 8 | Wire.read();
//   int16_t ayr = Wire.read() << 8 | Wire.read();
//   int16_t azr = Wire.read() << 8 | Wire.read();
//   ax_g = axr / 2048.0; ay_g = ayr / 2048.0; az_g = azr / 2048.0;
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   Wire.begin(D4, D5);
//   setupMPU();

//   dht.begin();
//   sensors.begin();
//   sensors.setWaitForConversion(false);  // DS18B20 blocking fix

//   pinMode(TCRT_PIN, INPUT);
//   attachInterrupt(digitalPinToInterrupt(TCRT_PIN), countPulse, FALLING);
//   lastRPMTime = millis();
//   micWindowStart = millis();

//   Serial.println("All 6 sensors initialized.");
// }

// void loop() {
//   // ---- MPU6050 ----
//   float ax, ay, az;
//   readMPU(ax, ay, az);
//   float magnitude = sqrt(ax * ax + ay * ay + az * az);

//   // ---- TCRT5000 RPM ----
//   if (millis() - lastRPMTime >= 1000) {
//     currentRPM = pulseCount * 60;
//     pulseCount = 0;
//     lastRPMTime = millis();
//   }

//   // ---- Mic range tracking ----
//   int micRaw = analogRead(MIC_PIN);
//   if (micRaw > micMax) micMax = micRaw;
//   if (micRaw < micMin) micMin = micRaw;

//   if (millis() - micWindowStart > 500) {
//     lastMicRange = micMax - micMin;
//     micMax = 0;
//     micMin = 4095;
//     micWindowStart = millis();
//   }

//   // ---- ACS712 ----
//   int acsRaw = analogRead(ACS_PIN);
//   float acsVoltage = acsRaw * (3.3 / 4095.0);

//   // ---- Print fast sensors ----
//   Serial.print("Vib:"); Serial.print(magnitude, 3);
//   Serial.print("g  RPM:"); Serial.print(currentRPM);
//   Serial.print("  MicRange:"); Serial.print(lastMicRange);
//   Serial.print(lastMicRange > MIC_THRESHOLD ? "(LOUD)" : "");
//   Serial.print("  Curr:"); Serial.print(acsVoltage, 3);
//   Serial.print("V");

//   // ---- Slow sensors: DHT22 + DS18B20 (every 2 sec) ----
//   static unsigned long lastSlow = 0;
//   if (millis() - lastSlow >= 2000) {
//     float h = dht.readHumidity();
//     float t = dht.readTemperature();
//     if (!isnan(h) && !isnan(t)) {
//       Serial.print("  | Temp:"); Serial.print(t);
//       Serial.print("C  Hum:"); Serial.print(h);
//       Serial.print("%");
//     }
//     sensors.requestTemperatures();
//     Serial.print("  MotorTemp:"); Serial.print(sensors.getTempCByIndex(0));
//     Serial.print("C");
//     lastSlow = millis();
//   }

//   Serial.println();
//   delay(100);
// }

//MOTOR A+B old

// #include <Wire.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <DHT.h>
// #include <Chirale_TensorFlowLite.h>
// #include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "tensorflow/lite/micro/micro_interpreter.h"
// #include "tensorflow/lite/micro/system_setup.h"
// #include "tensorflow/lite/schema/schema_generated.h"
// #include "rig_model_data.h"

// #define DHTPIN D2
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// #define ONE_WIRE_BUS D3
// OneWire oneWire(ONE_WIRE_BUS);
// DallasTemperature sensors(&oneWire);

// #define MIC_PIN A1
// #define MIC_THRESHOLD 900
// int micMax = 0;
// int micMin = 4095;
// unsigned long micWindowStart = 0;
// int lastMicRange = 0;

// #define ACS_PIN A0

// #define TCRT_PIN D6
// volatile unsigned long pulseCount = 0;
// volatile unsigned long lastInterruptTime = 0;
// unsigned long lastRPMTime = 0;
// unsigned long currentRPM = 0;

// void IRAM_ATTR countPulse() {
//   unsigned long t = micros();
//   if (t - lastInterruptTime > 2000) {
//     pulseCount++;
//     lastInterruptTime = t;
//   }
// }

// void setupMPU() {
//   Wire.beginTransmission(0x68);
//   Wire.write(0x6B); Wire.write(0);
//   Wire.endTransmission(true);
//   Wire.beginTransmission(0x68);
//   Wire.write(0x1C); Wire.write(0x18);
//   Wire.endTransmission(true);
// }

// void readMPU(float &ax_g, float &ay_g, float &az_g) {
//   Wire.beginTransmission(0x68);
//   Wire.write(0x3B);
//   Wire.endTransmission(false);
//   Wire.requestFrom(0x68, 6);
//   int16_t axr = Wire.read() << 8 | Wire.read();
//   int16_t ayr = Wire.read() << 8 | Wire.read();
//   int16_t azr = Wire.read() << 8 | Wire.read();
//   ax_g = axr / 2048.0; ay_g = ayr / 2048.0; az_g = azr / 2048.0;
// }

// #define WINDOW_SIZE 50
// float vibBuffer[WINDOW_SIZE];
// int micBuffer[WINDOW_SIZE];
// int rpmBuffer[WINDOW_SIZE];
// int bufferIndex = 0;

// // ===== VOTING HISTORY =====
// #define VOTE_HISTORY 5
// int predictionHistory[VOTE_HISTORY];
// int historyCount = 0;
// int historyIdx = 0;

// namespace {
//   const tflite::Model* model = nullptr;
//   tflite::MicroInterpreter* interpreter = nullptr;
//   TfLiteTensor* input = nullptr;
//   TfLiteTensor* output = nullptr;
//   constexpr int kTensorArenaSize = 20 * 1024;
//   uint8_t tensor_arena[kTensorArenaSize];
// }

// float feature_means[12] = {
//   1.918, 2.426, 2.754, 1.431, 0.498, 0.876, 2.754, 1.878,
//   1643.256, 581.083, 2696.312, 8154.175
// };
// float feature_stds[12] = {
//   0.368, 0.909, 0.596, 0.130, 0.172, 0.240, 0.596, 0.669,
//   423.326, 173.751, 652.413, 2370.653
// };
// const char* class_names[5] = {
//   "MotorA_Healthy", "MotorB_Healthy", "MotorB_Unbalance", 
//   "MotorB_Looseness", "MotorB_Overload"
// };

// void setupTFLite() {
//   model = tflite::GetModel(_content_drive_MyDrive_VibeSense_rig_data_rig_model_tflite);
//   static tflite::AllOpsResolver resolver;
//   static tflite::MicroInterpreter static_interpreter(
//     model, resolver, tensor_arena, kTensorArenaSize);
//   interpreter = &static_interpreter;
//   if (interpreter->AllocateTensors() != kTfLiteOk) {
//     Serial.println("AllocateTensors FAILED!");
//     return;
//   }
//   input = interpreter->input(0);
//   output = interpreter->output(0);
//   Serial.println("TF Lite Model loaded!");
// }

// void calculateFeaturesAndPredict() {
//   // ===== FIX 1: MOTOR OFF CHECK =====
//   float rpmSumCheck = 0;
//   for (int i = 0; i < WINDOW_SIZE; i++) rpmSumCheck += rpmBuffer[i];
//   float rpmMeanCheck = rpmSumCheck / WINDOW_SIZE;
  
//   if (rpmMeanCheck < 500) {  // Motor clearly OFF/inactive
//     Serial.println("  >>> STATUS: MOTOR INACTIVE (RPM too low)");
//     historyCount = 0;  // reset voting history bhi
//     return;  // PREDICT MAT KARO
//   }
  
//   // Feature calculation (same as before)
//   float sum = 0, sumSq = 0, maxV = -999, minV = 999;
//   for (int i = 0; i < WINDOW_SIZE; i++) {
//     sum += vibBuffer[i];
//     sumSq += vibBuffer[i] * vibBuffer[i];
//     if (vibBuffer[i] > maxV) maxV = vibBuffer[i];
//     if (vibBuffer[i] < minV) minV = vibBuffer[i];
//   }
//   float mean = sum / WINDOW_SIZE;
//   float rms = sqrt(sumSq / WINDOW_SIZE);
  
//   float sumSq4 = 0, sumSq2 = 0;
//   for (int i = 0; i < WINDOW_SIZE; i++) {
//     float diff = vibBuffer[i] - mean;
//     sumSq2 += diff * diff;
//     sumSq4 += diff * diff * diff * diff;
//   }
//   float variance = sumSq2 / WINDOW_SIZE;
//   float stdV = sqrt(variance);
//   float kurt = (sumSq4 / WINDOW_SIZE) / (variance * variance + 1e-9);
//   float peak = maxV;
//   float crest = peak / (rms + 1e-9);
//   float rangeV = maxV - minV;
  
//   float micSum = 0, micMaxVal = -999;
//   for (int i = 0; i < WINDOW_SIZE; i++) {
//     micSum += micBuffer[i];
//     if (micBuffer[i] > micMaxVal) micMaxVal = micBuffer[i];
//   }
//   float micMean = micSum / WINDOW_SIZE;
//   float micSumSq = 0;
//   for (int i = 0; i < WINDOW_SIZE; i++) {
//     float diff = micBuffer[i] - micMean;
//     micSumSq += diff * diff;
//   }
//   float micStd = sqrt(micSumSq / WINDOW_SIZE);
  
//   float raw_features[12] = {rms, kurt, peak, crest, stdV, minV, maxV, rangeV, micMean, micStd, micMaxVal, rpmMeanCheck};
  
//   for (int i = 0; i < 12; i++) {
//     float scaled = (raw_features[i] - feature_means[i]) / feature_stds[i];
//     input->data.int8[i] = (int8_t)(scaled / input->params.scale + input->params.zero_point);
//   }
  
//   interpreter->Invoke();
  
//   int best_class = 0;
//   int8_t best_score = output->data.int8[0];
//   for (int i = 1; i < 5; i++) {
//     if (output->data.int8[i] > best_score) {
//       best_score = output->data.int8[i];
//       best_class = i;
//     }
//   }
  
//   // ===== FIX 2: VOTING SYSTEM =====
//   predictionHistory[historyIdx] = best_class;
//   historyIdx = (historyIdx + 1) % VOTE_HISTORY;
//   if (historyCount < VOTE_HISTORY) historyCount++;
  
//   // Majority vote nikalo
//   int voteCounts[5] = {0,0,0,0,0};
//   for (int i = 0; i < historyCount; i++) {
//     voteCounts[predictionHistory[i]]++;
//   }
//   int votedClass = 0, maxVotes = voteCounts[0];
//   for (int i = 1; i < 5; i++) {
//     if (voteCounts[i] > maxVotes) {
//       maxVotes = voteCounts[i];
//       votedClass = i;
//     }
//   }
  
//   Serial.print("  >>> Raw: ");
//   Serial.print(class_names[best_class]);
//   Serial.print(" | VOTED (");
//   Serial.print(maxVotes);
//   Serial.print("/");
//   Serial.print(historyCount);
//   Serial.print("): ");
//   Serial.println(class_names[votedClass]);
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Wire.begin(D4, D5);
//   setupMPU();
//   dht.begin();
//   sensors.begin();
//   sensors.setWaitForConversion(false);
//   pinMode(TCRT_PIN, INPUT);
//   attachInterrupt(digitalPinToInterrupt(TCRT_PIN), countPulse, FALLING);
//   lastRPMTime = millis();
//   micWindowStart = millis();
//   setupTFLite();
//   Serial.println("All 6 sensors + TF Lite initialized.");
// }

// void loop() {
//   float ax, ay, az;
//   readMPU(ax, ay, az);
//   float magnitude = sqrt(ax * ax + ay * ay + az * az);

//   if (millis() - lastRPMTime >= 1000) {
//     currentRPM = pulseCount * 60;
//     pulseCount = 0;
//     lastRPMTime = millis();
//   }

//   int micRaw = analogRead(MIC_PIN);
//   if (micRaw > micMax) micMax = micRaw;
//   if (micRaw < micMin) micMin = micRaw;
//   if (millis() - micWindowStart > 500) {
//     lastMicRange = micMax - micMin;
//     micMax = 0;
//     micMin = 4095;
//     micWindowStart = millis();
//   }

//   int acsRaw = analogRead(ACS_PIN);
//   float acsVoltage = acsRaw * (3.3 / 4095.0);

//   vibBuffer[bufferIndex] = magnitude;
//   micBuffer[bufferIndex] = micRaw;
//   rpmBuffer[bufferIndex] = currentRPM;
//   bufferIndex++;

//   Serial.print("Vib:"); Serial.print(magnitude, 3);
//   Serial.print("g  RPM:"); Serial.print(currentRPM);
//   Serial.print("  MicRange:"); Serial.print(lastMicRange);
//   Serial.print(lastMicRange > MIC_THRESHOLD ? "(LOUD)" : "");
//   Serial.print("  Curr:"); Serial.print(acsVoltage, 3);
//   Serial.print("V");

//   static unsigned long lastSlow = 0;
//   if (millis() - lastSlow >= 2000) {
//     float h = dht.readHumidity();
//     float t = dht.readTemperature();
//     if (!isnan(h) && !isnan(t)) {
//       Serial.print("  | Temp:"); Serial.print(t);
//       Serial.print("C  Hum:"); Serial.print(h);
//       Serial.print("%");
//     }
//     sensors.requestTemperatures();
//     Serial.print("  MotorTemp:"); Serial.print(sensors.getTempCByIndex(0));
//     Serial.print("C");
//     lastSlow = millis();
//   }

//   if (bufferIndex >= WINDOW_SIZE) {
//     calculateFeaturesAndPredict();
//     bufferIndex = 0;
//   } else {
//     Serial.println();
//   }

//   delay(100);
// }


//motor A only main


// #include <Wire.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <DHT.h>
// #include <Chirale_TensorFlowLite.h>
// #include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "tensorflow/lite/micro/micro_interpreter.h"
// #include "tensorflow/lite/micro/system_setup.h"
// #include "tensorflow/lite/schema/schema_generated.h"
// #include "motorA_model_data.h"

// #define DHTPIN D2
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// #define ONE_WIRE_BUS D3
// OneWire oneWire(ONE_WIRE_BUS);
// DallasTemperature sensors(&oneWire);

// #define MIC_PIN A1
// #define MIC_THRESHOLD 900
// int micMax = 0;
// int micMin = 4095;
// unsigned long micWindowStart = 0;
// int lastMicRange = 0;

// #define ACS_PIN A0

// #define TCRT_PIN D6
// volatile unsigned long pulseCount = 0;
// volatile unsigned long lastInterruptTime = 0;
// unsigned long lastRPMTime = 0;
// unsigned long currentRPM = 0;

// void IRAM_ATTR countPulse() {
//   unsigned long t = micros();
//   if (t - lastInterruptTime > 2000) {
//     pulseCount++;
//     lastInterruptTime = t;
//   }
// }

// void setupMPU() {
//   Wire.beginTransmission(0x68);
//   Wire.write(0x6B); Wire.write(0);
//   Wire.endTransmission(true);
//   Wire.beginTransmission(0x68);
//   Wire.write(0x1C); Wire.write(0x18);
//   Wire.endTransmission(true);
// }

// void readMPU(float &ax_g, float &ay_g, float &az_g) {
//   Wire.beginTransmission(0x68);
//   Wire.write(0x3B);
//   Wire.endTransmission(false);
//   Wire.requestFrom(0x68, 6);
//   int16_t axr = Wire.read() << 8 | Wire.read();
//   int16_t ayr = Wire.read() << 8 | Wire.read();
//   int16_t azr = Wire.read() << 8 | Wire.read();
//   ax_g = axr / 2048.0; ay_g = ayr / 2048.0; az_g = azr / 2048.0;
// }

// #define WINDOW_SIZE 50
// float vibBuffer[WINDOW_SIZE];
// int micBuffer[WINDOW_SIZE];
// int rpmBuffer[WINDOW_SIZE];
// int bufferIndex = 0;

// #define VOTE_HISTORY 10
// int predictionHistory[VOTE_HISTORY];
// int historyCount = 0;
// int historyIdx = 0;

// namespace {
//   const tflite::Model* model = nullptr;
//   tflite::MicroInterpreter* interpreter = nullptr;
//   TfLiteTensor* input = nullptr;
//   TfLiteTensor* output = nullptr;
//   constexpr int kTensorArenaSize = 20 * 1024;
//   uint8_t tensor_arena[kTensorArenaSize];
// }

// // ===== NAYE SCALER VALUES (Motor A, 4-class) =====
// float feature_means[12] = {
//   1.233, 2.733, 1.664, 1.338, 0.213, 0.876, 1.664, 0.789,
//   1391.715, 500.363, 2316.287, 11683.863
// };
// float feature_stds[12] = {
//   0.136, 1.038, 0.342, 0.157, 0.133, 0.119, 0.342, 0.427,
//   248.921, 157.345, 553.383, 1583.246
// };

// // ===== NAYE 4 CLASSES =====
// const char* class_names[4] = {
//   "MotorA_Healthy", "MotorA_Unbalance", "MotorA_Looseness", "MotorA_Overload"
// };

// void setupTFLite() {
//   // ARRAY NAAM CONFIRM KARKE YAHAN DAALNA - file khol ke check kar
//   model = tflite::GetModel(_content_drive_MyDrive_VibeSense_rig_data_motorA_model_tflite);
  
//   static tflite::AllOpsResolver resolver;
//   static tflite::MicroInterpreter static_interpreter(
//     model, resolver, tensor_arena, kTensorArenaSize);
//   interpreter = &static_interpreter;
  
//   if (interpreter->AllocateTensors() != kTfLiteOk) {
//     Serial.println("AllocateTensors FAILED!");
//     return;
//   }
  
//   input = interpreter->input(0);
//   output = interpreter->output(0);
//   Serial.println("TF Lite Model loaded!");
// }

// void calculateFeaturesAndPredict() {
//   float rpmSumCheck = 0;
//   for (int i = 0; i < WINDOW_SIZE; i++) rpmSumCheck += rpmBuffer[i];
//   float rpmMeanCheck = rpmSumCheck / WINDOW_SIZE;
  
//   if (rpmMeanCheck < 200) {
//     Serial.println("  >>> STATUS: MOTOR INACTIVE");
//     historyCount = 0;
//     return;
//   }
  
//   float sum = 0, sumSq = 0, maxV = -999, minV = 999;
//   for (int i = 0; i < WINDOW_SIZE; i++) {
//     sum += vibBuffer[i];
//     sumSq += vibBuffer[i] * vibBuffer[i];
//     if (vibBuffer[i] > maxV) maxV = vibBuffer[i];
//     if (vibBuffer[i] < minV) minV = vibBuffer[i];
//   }
//   float mean = sum / WINDOW_SIZE;
//   float rms = sqrt(sumSq / WINDOW_SIZE);
  
//   float sumSq4 = 0, sumSq2 = 0;
//   for (int i = 0; i < WINDOW_SIZE; i++) {
//     float diff = vibBuffer[i] - mean;
//     sumSq2 += diff * diff;
//     sumSq4 += diff * diff * diff * diff;
//   }
//   float variance = sumSq2 / WINDOW_SIZE;
//   float stdV = sqrt(variance);
//   float kurt = (sumSq4 / WINDOW_SIZE) / (variance * variance + 1e-9);
//   float peak = maxV;
//   float crest = peak / (rms + 1e-9);
//   float rangeV = maxV - minV;
  
//   float micSum = 0, micMaxVal = -999;
//   for (int i = 0; i < WINDOW_SIZE; i++) {
//     micSum += micBuffer[i];
//     if (micBuffer[i] > micMaxVal) micMaxVal = micBuffer[i];
//   }
//   float micMean = micSum / WINDOW_SIZE;
//   float micSumSq = 0;
//   for (int i = 0; i < WINDOW_SIZE; i++) {
//     float diff = micBuffer[i] - micMean;
//     micSumSq += diff * diff;
//   }
//   float micStd = sqrt(micSumSq / WINDOW_SIZE);
  
//   float raw_features[12] = {rms, kurt, peak, crest, stdV, minV, maxV, rangeV, micMean, micStd, micMaxVal, rpmMeanCheck};
  
//   for (int i = 0; i < 12; i++) {
//     float scaled = (raw_features[i] - feature_means[i]) / feature_stds[i];
//     input->data.int8[i] = (int8_t)(scaled / input->params.scale + input->params.zero_point);
//   }
  
//   interpreter->Invoke();
  
//   int best_class = 0;
//   int8_t best_score = output->data.int8[0];
//   for (int i = 1; i < 4; i++) {  // 4 classes ab
//     if (output->data.int8[i] > best_score) {
//       best_score = output->data.int8[i];
//       best_class = i;
//     }
//   }
  
//   predictionHistory[historyIdx] = best_class;
//   historyIdx = (historyIdx + 1) % VOTE_HISTORY;
//   if (historyCount < VOTE_HISTORY) historyCount++;
  
//   int voteCounts[4] = {0,0,0,0};
//   for (int i = 0; i < historyCount; i++) {
//     voteCounts[predictionHistory[i]]++;
//   }
//   int votedClass = 0, maxVotes = voteCounts[0];
//   for (int i = 1; i < 4; i++) {
//     if (voteCounts[i] > maxVotes) {
//       maxVotes = voteCounts[i];
//       votedClass = i;
//     }
//   }
  
//   Serial.print("  >>> Raw: ");
//   Serial.print(class_names[best_class]);
//   Serial.print(" | VOTED: ");
//   Serial.println(class_names[votedClass]);
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Wire.begin(D4, D5);
//   setupMPU();
//   dht.begin();
//   sensors.begin();
//   sensors.setWaitForConversion(false);
//   pinMode(TCRT_PIN, INPUT);
//   attachInterrupt(digitalPinToInterrupt(TCRT_PIN), countPulse, FALLING);
//   lastRPMTime = millis();
//   micWindowStart = millis();
//   setupTFLite();
//   Serial.println("Motor A model + 6 sensors initialized.");
// }

// void loop() {
//   float ax, ay, az;
//   readMPU(ax, ay, az);
//   float magnitude = sqrt(ax * ax + ay * ay + az * az);

//   if (millis() - lastRPMTime >= 1000) {
//     currentRPM = pulseCount * 60;
//     pulseCount = 0;
//     lastRPMTime = millis();
//   }

//   int micRaw = analogRead(MIC_PIN);
//   if (micRaw > micMax) micMax = micRaw;
//   if (micRaw < micMin) micMin = micRaw;
//   if (millis() - micWindowStart > 500) {
//     lastMicRange = micMax - micMin;
//     micMax = 0;
//     micMin = 4095;
//     micWindowStart = millis();
//   }

//   int acsRaw = analogRead(ACS_PIN);
//   float acsVoltage = acsRaw * (3.3 / 4095.0);

//   vibBuffer[bufferIndex] = magnitude;
//   micBuffer[bufferIndex] = micRaw;
//   rpmBuffer[bufferIndex] = currentRPM;
//   bufferIndex++;

//   Serial.print("Vib:"); Serial.print(magnitude, 3);
//   Serial.print("g  RPM:"); Serial.print(currentRPM);
//   Serial.print("  MicRange:"); Serial.print(lastMicRange);
//   Serial.print(lastMicRange > MIC_THRESHOLD ? "(LOUD)" : "");
//   Serial.print("  Curr:"); Serial.print(acsVoltage, 3);
//   Serial.print("V");

//   static unsigned long lastSlow = 0;
//   if (millis() - lastSlow >= 2000) {
//     float h = dht.readHumidity();
//     float t = dht.readTemperature();
//     if (!isnan(h) && !isnan(t)) {
//       Serial.print("  | Temp:"); Serial.print(t);
//       Serial.print("C  Hum:"); Serial.print(h);
//       Serial.print("%");
//     }
//     sensors.requestTemperatures();
//     Serial.print("  MotorTemp:"); Serial.print(sensors.getTempCByIndex(0));
//     Serial.print("C");
//     lastSlow = millis();
//   }

//   if (bufferIndex >= WINDOW_SIZE) {
//     calculateFeaturesAndPredict();
//     bufferIndex = 0;
//   } else {
//     Serial.println();
//   }

//   delay(100);
// }

#include "secrets.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Chirale_TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "motorA_model_data.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// ================= WiFi + MQTT =================

const char* mqtt_topic = "vibesense/node01/telemetry";

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

void setupWiFi() {
  WiFi.begin(ssid, password_wifi);
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());  // ADD KIYA - status code print karega
    attempts++;
  }
  

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi FAILED - continuing without connectivity");
  }
  espClient.setInsecure();
}

void reconnectMQTT() {
  int attempts = 0;
  while (!mqttClient.connected() && attempts < 5) {
    Serial.println("Connecting to HiveMQ...");
    if (mqttClient.connect("XIAO_VibeSense_Node01", mqtt_user, mqtt_pass)) {
      Serial.println("MQTT connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(mqttClient.state());
      delay(2000);
      attempts++;
    }
  }
}

void publishPrediction(const char* className, float vib, float rpm, float mic, float curr) {
  if (!mqttClient.connected()) return;
  char payload[300];
  snprintf(payload, sizeof(payload),
    "{\"dev\":\"node01\",\"ts\":%lu,\"cls_name\":\"%s\",\"vib\":%.3f,\"rpm\":%.1f,\"mic\":%.1f,\"curr\":%.3f}",
    millis(), className, vib, rpm, mic, curr);
  mqttClient.publish(mqtt_topic, payload);
  Serial.print("Published: ");
  Serial.println(payload);
}

// ================= SENSORS =================
#define DHTPIN D2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define ONE_WIRE_BUS D3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define MIC_PIN A1
#define MIC_THRESHOLD 900
int micMax = 0;
int micMin = 4095;
unsigned long micWindowStart = 0;
int lastMicRange = 0;

#define ACS_PIN A0

#define TCRT_PIN D6
volatile unsigned long pulseCount = 0;
volatile unsigned long lastInterruptTime = 0;
unsigned long lastRPMTime = 0;
unsigned long currentRPM = 0;

void IRAM_ATTR countPulse() {
  unsigned long t = micros();
  if (t - lastInterruptTime > 2000) {
    pulseCount++;
    lastInterruptTime = t;
  }
}

void setupMPU() {
  Wire.beginTransmission(0x68);
  Wire.write(0x6B); Wire.write(0);
  Wire.endTransmission(true);
  Wire.beginTransmission(0x68);
  Wire.write(0x1C); Wire.write(0x18);
  Wire.endTransmission(true);
}

void readMPU(float &ax_g, float &ay_g, float &az_g) {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6);
  int16_t axr = Wire.read() << 8 | Wire.read();
  int16_t ayr = Wire.read() << 8 | Wire.read();
  int16_t azr = Wire.read() << 8 | Wire.read();
  ax_g = axr / 2048.0; ay_g = ayr / 2048.0; az_g = azr / 2048.0;
}

#define WINDOW_SIZE 50
float vibBuffer[WINDOW_SIZE];
int micBuffer[WINDOW_SIZE];
int rpmBuffer[WINDOW_SIZE];
int bufferIndex = 0;

#define VOTE_HISTORY 5
int predictionHistory[VOTE_HISTORY];
int historyCount = 0;
int historyIdx = 0;

namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  constexpr int kTensorArenaSize = 20 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
}

float feature_means[12] = {
  1.233, 2.733, 1.664, 1.338, 0.213, 0.876, 1.664, 0.789,
  1391.715, 500.363, 2316.287, 11683.863
};
float feature_stds[12] = {
  0.136, 1.038, 0.342, 0.157, 0.133, 0.119, 0.342, 0.427,
  248.921, 157.345, 553.383, 1583.246
};
const char* class_names[4] = {
  "MotorA_Healthy", "MotorA_Unbalance", "MotorA_Looseness", "MotorA_Overload"
};

void setupTFLite() {
  model = tflite::GetModel(_content_drive_MyDrive_VibeSense_rig_data_motorA_model_tflite);
  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;
  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("AllocateTensors FAILED!");
    return;
  }
  input = interpreter->input(0);
  output = interpreter->output(0);
  Serial.println("TF Lite Model loaded!");
}

void calculateFeaturesAndPredict() {
  float rpmSumCheck = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) rpmSumCheck += rpmBuffer[i];
  float rpmMeanCheck = rpmSumCheck / WINDOW_SIZE;
  
  if (rpmMeanCheck < 200) {
    Serial.println("  >>> STATUS: MOTOR INACTIVE");
    historyCount = 0;
    return;
  }
  
  float sum = 0, sumSq = 0, maxV = -999, minV = 999;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    sum += vibBuffer[i];
    sumSq += vibBuffer[i] * vibBuffer[i];
    if (vibBuffer[i] > maxV) maxV = vibBuffer[i];
    if (vibBuffer[i] < minV) minV = vibBuffer[i];
  }
  float mean = sum / WINDOW_SIZE;
  float rms = sqrt(sumSq / WINDOW_SIZE);
  
  float sumSq4 = 0, sumSq2 = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    float diff = vibBuffer[i] - mean;
    sumSq2 += diff * diff;
    sumSq4 += diff * diff * diff * diff;
  }
  float variance = sumSq2 / WINDOW_SIZE;
  float stdV = sqrt(variance);
  float kurt = (sumSq4 / WINDOW_SIZE) / (variance * variance + 1e-9);
  float peak = maxV;
  float crest = peak / (rms + 1e-9);
  float rangeV = maxV - minV;
  
  float micSum = 0, micMaxVal = -999;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    micSum += micBuffer[i];
    if (micBuffer[i] > micMaxVal) micMaxVal = micBuffer[i];
  }
  float micMean = micSum / WINDOW_SIZE;
  float micSumSq = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    float diff = micBuffer[i] - micMean;
    micSumSq += diff * diff;
  }
  float micStd = sqrt(micSumSq / WINDOW_SIZE);
  
  float raw_features[12] = {rms, kurt, peak, crest, stdV, minV, maxV, rangeV, micMean, micStd, micMaxVal, rpmMeanCheck};
  
  for (int i = 0; i < 12; i++) {
    float scaled = (raw_features[i] - feature_means[i]) / feature_stds[i];
    input->data.int8[i] = (int8_t)(scaled / input->params.scale + input->params.zero_point);
  }
  
  interpreter->Invoke();
  
  int best_class = 0;
  int8_t best_score = output->data.int8[0];
  for (int i = 1; i < 4; i++) {
    if (output->data.int8[i] > best_score) {
      best_score = output->data.int8[i];
      best_class = i;
    }
  }
  
  predictionHistory[historyIdx] = best_class;
  historyIdx = (historyIdx + 1) % VOTE_HISTORY;
  if (historyCount < VOTE_HISTORY) historyCount++;
  
  int voteCounts[4] = {0,0,0,0};
  for (int i = 0; i < historyCount; i++) {
    voteCounts[predictionHistory[i]]++;
  }
  int votedClass = 0, maxVotes = voteCounts[0];
  for (int i = 1; i < 4; i++) {
    if (voteCounts[i] > maxVotes) {
      maxVotes = voteCounts[i];
      votedClass = i;
    }
  }
  
  Serial.print("  >>> Raw: ");
  Serial.print(class_names[best_class]);
  Serial.print(" | VOTED: ");
  Serial.println(class_names[votedClass]);
  
  // ===== MQTT PUBLISH =====
  publishPrediction(class_names[votedClass], mean, rpmMeanCheck, (float)lastMicRange, 0.0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  setupWiFi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  
  Wire.begin(D4, D5);
  setupMPU();
  dht.begin();
  sensors.begin();
  sensors.setWaitForConversion(false);
  pinMode(TCRT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(TCRT_PIN), countPulse, FALLING);
  lastRPMTime = millis();
  micWindowStart = millis();
  setupTFLite();
  Serial.println("Motor A model + 6 sensors + MQTT initialized.");
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  
  float ax, ay, az;
  readMPU(ax, ay, az);
  float magnitude = sqrt(ax * ax + ay * ay + az * az);

  if (millis() - lastRPMTime >= 1000) {
    currentRPM = pulseCount * 60;
    pulseCount = 0;
    lastRPMTime = millis();
  }

  int micRaw = analogRead(MIC_PIN);
  if (micRaw > micMax) micMax = micRaw;
  if (micRaw < micMin) micMin = micRaw;
  if (millis() - micWindowStart > 500) {
    lastMicRange = micMax - micMin;
    micMax = 0;
    micMin = 4095;
    micWindowStart = millis();
  }

  int acsRaw = analogRead(ACS_PIN);
  float acsVoltage = acsRaw * (3.3 / 4095.0);

  vibBuffer[bufferIndex] = magnitude;
  micBuffer[bufferIndex] = micRaw;
  rpmBuffer[bufferIndex] = currentRPM;
  bufferIndex++;

  Serial.print("Vib:"); Serial.print(magnitude, 3);
  Serial.print("g  RPM:"); Serial.print(currentRPM);
  Serial.print("  MicRange:"); Serial.print(lastMicRange);
  Serial.print(lastMicRange > MIC_THRESHOLD ? "(LOUD)" : "");
  Serial.print("  Curr:"); Serial.print(acsVoltage, 3);
  Serial.print("V");

  static unsigned long lastSlow = 0;
  if (millis() - lastSlow >= 2000) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
      Serial.print("  | Temp:"); Serial.print(t);
      Serial.print("C  Hum:"); Serial.print(h);
      Serial.print("%");
    }
    sensors.requestTemperatures();
    Serial.print("  MotorTemp:"); Serial.print(sensors.getTempCByIndex(0));
    Serial.print("C");
    lastSlow = millis();
  }

  if (bufferIndex >= WINDOW_SIZE) {
    calculateFeaturesAndPredict();
    bufferIndex = 0;
  } else {
    Serial.println();
  }

  delay(100);
}