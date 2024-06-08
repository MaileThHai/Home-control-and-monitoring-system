#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>

#define WIFI_SSID "A12.10"
#define WIFI_PASSWORD "anhem121"

#define FIREBASE_HOST "https://kltn-bd6e3-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "83bxHzf3auUunfMHuU2FUKuPK11FOJJRgCgZDsQg"

FirebaseData fbdo1;

String parentPath = "/KLTN";
String childPath[2] = { "/node1", "/node3" };
size_t childPathSize = 2;

void printResult(FirebaseData &data);

void streamCallback(MultiPathStreamData stream) {
  Serial.println();
  Serial.println("Stream Data1 available...");

  size_t numChild = sizeof(childPath) / sizeof(childPath[0]);

  for (size_t i = 0; i < numChild; i++) {
    if (stream.get(childPath[i])) {
      Serial.println("path: " + stream.dataPath + ", type: " + stream.type + ", value: " + stream.value);
      parseAndPrintJson(stream.value.c_str());
    }
  }

  Serial.println();
}

void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }
}

void parseAndPrintJson(const char *jsonString) {
  DynamicJsonBuffer jsonBuffer(5120);
  JsonObject &root = jsonBuffer.parseObject(jsonString);
  
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  for (JsonObject::iterator it = root.begin(); it != root.end(); ++it) {
    const char *key = it->key;
    Serial.println(key);
    if (strcmp(key, "Relay1_1") == 0 && it->value.is<const char *>()) {
      const char *value = it->value.as<const char *>();
      if (strcmp(value, "ON") == 0) {
        Serial.println("DVC1ON");
      } else if (strcmp(value, "OFF") == 0) {
        Serial.println("DVC1OFF");
      }
    }
  }
}

void initFirebase(void * parameter) {
  if (!Firebase.beginMultiPathStream(fbdo1, parentPath, childPath, childPathSize)) {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + fbdo1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setMultiPathStreamCallback(fbdo1, streamCallback, streamTimeoutCallback, 8192);

  while (true) {
    // Firebase tasks can be added here
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {

  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  xTaskCreatePinnedToCore(
    initFirebase,     // Task function
    "Init Firebase",  // Name of the task (for debugging)
    8192,             // Stack size (in bytes)
    NULL,             // Task input parameter
    1,                // Priority of the task
    NULL,             // Task handle
    0                 // Core where the task should run
  );        // Task handle
}

void loop() {
}
