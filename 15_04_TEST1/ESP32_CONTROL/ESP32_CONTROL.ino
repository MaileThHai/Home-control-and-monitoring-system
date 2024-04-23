#include "Arduino.h"
#include "LoRa_E32.h"
#include <Wire.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

//=====================Tạo Task===============================
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define FIREBASE_HOST "https://esp32-data-fb2d5-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "y9AzB0SNwyy4kReO3ot7wX3r2ZtWGPqrmFTD7ciZ"
FirebaseData fbdo;
#define WIFI_SSID "HUY QUANG"
#define WIFI_PASSWORD "khongbiet"

LoRa_E32 e32ttl100(&Serial2, 15, 25, 33);

// const int relay = 26;
int ControlREV;
#define RLYPIN1 26
#define RLYPIN2 27

void TaskREVControlLight(void *pvParameters);
// void TaskControlFB(void *pvParameters);

struct ControlLVR {
  byte Control1[4];
  byte Control2[4];
} controllvr;

void setup() {
  Serial.begin(9600);
  // pinMode(relay, OUTPUT);
  pinMode(RLYPIN1, OUTPUT);
  pinMode(RLYPIN2, OUTPUT);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
  delay(100);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connect Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("WiFi IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Firebase.setReadTimeout(fbdo, 1000 * 60);
  Firebase.setwriteSizeLimit(fbdo, "tiny");

  e32ttl100.begin();

  xTaskCreatePinnedToCore(TaskREVControlLight, "TaskREVControlLight", 10000, NULL, 1, NULL, 0);
}

void loop() {
}

void TaskREVControlLight(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    // Serial.print("TaskControlLight: ");
    // Serial.println(xPortGetCoreID());

    if (e32ttl100.available() > 1) {
      char type[4];
      ResponseContainer rs = e32ttl100.receiveInitialMessage(sizeof(type));
      String typeStr = rs.data;

      Serial.println(typeStr);
      if (typeStr == "CLV") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(ControlLVR));
        struct ControlLVR controllvr = *(ControlLVR *)rsc.data;

        Serial.println(*(int *)(controllvr.Control1));
        Serial.println(*(int *)(controllvr.Control2));

        // Kiểm tra giá trị của Control
        if (*(int *)(controllvr.Control1) == 1) {
          // Bật LED
          digitalWrite(RLYPIN1, LOW);
          Serial.println("Current not Flowing");
          Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_Light", "OFF");
        } else if (*(int *)(controllvr.Control1) == 0) {
          digitalWrite(RLYPIN1, HIGH);
          Serial.println("Current Flowing");
          Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_Light", "ON");
        } else if (*(int *)(controllvr.Control2) == 1) {
          digitalWrite(RLYPIN2, LOW);
          Serial.println("Current not Flowing");
          Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FAN", "OFF");
        } else if (*(int *)(controllvr.Control2) == 0) {
          digitalWrite(RLYPIN2, HIGH);
          Serial.println("Current Flowing");
          Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FAN", "ON");
        }
        // Close the response struct container
        rsc.close();
      } else {
        Serial.println("Something goes wrong!!");
      }
    }
    Serial.println("=================================================");
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
