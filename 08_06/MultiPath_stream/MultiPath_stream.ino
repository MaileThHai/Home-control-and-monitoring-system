#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>

#define WIFI_SSID "HUY QUANG"
#define WIFI_PASSWORD "khongbiet"

#define FIREBASE_HOST "https://kltn-bd6e3-default-rtdb.asia-southeast1.firebasedatabase.app/"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "83bxHzf3auUunfMHuU2FUKuPK11FOJJRgCgZDsQg"

//Define FirebaseESP8266 data object
FirebaseData fbdo1;
FirebaseData fbdo2;

unsigned long sendDataPrevMillis = 0;

String parentPath = "/KLTN";
String childPath[2] = { "/node1", "/node3" };
size_t childPathSize = 2;

uint16_t count = 0;

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
  StaticJsonDocument<5120> doc;
  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  for (JsonPair kv : doc.as<JsonObject>()) {
    const char *key = kv.key().c_str();
    Serial.println(key);
    if (strcmp(key, "Relay1") == 0 && kv.value().is<const char *>()) {
      const char *value = kv.value();
      if (strcmp(value, "ON") == 0) {
        Serial.println("DVC1ON");
      } else if (strcmp(value, "OFF") == 0) {
        Serial.println("DVC1OFF");
      }
    }
  }
}

void setup() {

  Serial.begin(115200);

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



  if (!Firebase.beginMultiPathStream(fbdo1, parentPath, childPath, childPathSize)) {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + fbdo1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  //Set the reserved size of stack memory in bytes for internal stream callback processing RTOS task.
  //8192 is the minimum size.
  Firebase.setMultiPathStreamCallback(fbdo1, streamCallback, streamTimeoutCallback, 8192);
}

void loop() {
}