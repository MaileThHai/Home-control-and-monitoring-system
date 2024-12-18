#include "Arduino.h"
#include "LoRa_E32.h"
#include <Wire.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <UniversalTelegramBot.h>
#include "Adafruit_SHT31.h"
#include "Adafruit_CCS811.h"

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

// Initialize Telegram BOT
String BOTtoken = "6483101599:AAEceP-UHR8ne3owkRkMmjfB9BPPvPPNybQ";  // your Bot Token (Get from Botfather)
String CHAT_ID = "6999992513";                                       // Your chat ID
WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

#define Fire_GPIO 35
#define MP4_GPIO 34

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_CCS811 ccs;
LoRa_E32 e32ttl100(&Serial2, 15, 25, 33);

float t, h;
int co2, tvoc, sensorValue, MP4Value;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
String typeStr;

void TaskReadDataSSLVR(void *pvParameters);
void TaskSendDataLVR(void *pvParameters);
void TaskREVPASSWORD(void *pvParameters);
void TaskDataFirebase(void *pvParameters);
void TaskSendAlert(void *pvParameters);

struct DataSensorLVR {
  char type[4] = "LVR";
  byte temperature[4];
  byte humidity[4];
  byte CO2[4];
  byte TVOC[4];
  byte ValFlameSensor[4];
  byte ValSmokeSensor[4];
} datasensorLVR;

struct SuccessLVF {
  char type[4] = "SSS";
} succsesslvf;

struct SendTelegram {
  byte AlertPassword[6];
  byte AlertTemp[6];
  byte AlertFire[6];
  byte AlertSmoke[6];
  byte AlertCO2[6];
} sendtelegram;

boolean runEvery(unsigned long interval, unsigned long *previousMillis) {
  unsigned long currentMillis = millis();
  if (currentMillis - *previousMillis >= interval) {
    *previousMillis = currentMillis;
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  delay(100);

  pinMode(Fire_GPIO, INPUT);

  //Check SHT31 is running
  if (!sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connect Wi-Fi");
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
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

  xTaskCreatePinnedToCore(TaskSendDataLVR, "TaskSendDataLVR", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskReadDataSSLVR, "TaskReadDataSSLVR", 4096, NULL, 1, NULL, 0);
  //
  xTaskCreatePinnedToCore(TaskREVPASSWORD, "TaskREVPASSWORD", 21000, NULL, 2, NULL, 0);
  // xTaskCreatePinnedToCore(TaskSendAlert, "TaskSendAlert", 20000, NULL, 1, NULL, 0);
}

void loop() {
}

void TaskReadDataSSLVR(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    //SHT31//
    t = sht31.readTemperature();
    h = sht31.readHumidity();
    //CCS811//
    co2 = ccs.geteCO2();
    tvoc = ccs.getTVOC();
    co2 = 897;
    tvoc = 67;
    //FLAME//
    sensorValue = analogRead(Fire_GPIO);
    //SMOKE//
    MP4Value = analogRead(MP4_GPIO);

    if (runEvery(60000, &previousMillis1)) {
      xTaskCreatePinnedToCore(TaskDataFirebase, "TaskDataFirebase", 20000, NULL, 1, NULL, 0);

      // xTaskCreatePinnedToCore(TaskSendAlert, "TaskSendAlert", 20000, NULL, 4, NULL, 0);
    }

    vTaskDelay(350 / portTICK_PERIOD_MS);
  }
}
void TaskSendDataLVR(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    ///SHT31////
    strcpy(datasensorLVR.type, "LVR");
    // Gửi dữ liệu nhiệt độ
    *(float *)(datasensorLVR.temperature) = t;
    // Gửi dữ liệu độ ẩm
    *(float *)(datasensorLVR.humidity) = h;
    ///CCS811////
    // Gửi dữ liệu CO2
    *(uint16_t *)(datasensorLVR.CO2) = co2;
    // Gửi dữ liệu TVOC
    *(uint16_t *)(datasensorLVR.TVOC) = tvoc;
    ///FLAME//
    *(uint16_t *)(datasensorLVR.ValFlameSensor) = sensorValue;
    ///SMOKE//
    *(uint16_t *)(datasensorLVR.ValSmokeSensor) = MP4Value;

    vTaskDelay(500);
    ResponseStatus rs = e32ttl100.sendFixedMessage(0, 1, 0xA, &datasensorLVR, sizeof(DataSensorLVR));
    Serial.println(rs.getResponseDescription());

    vTaskDelay(500);
    ResponseStatus rss = e32ttl100.sendFixedMessage(0, 4, 0xF, &succsesslvf, sizeof(SuccessLVF));

    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

void TaskDataFirebase(void *pvParameters) {
  (void)pvParameters;
  // for (;;)  // A Task shall never return or exit.
  // {
  Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Temperature", t);
  Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Humidity", h);
  Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/CO2", co2);
  Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/TVOC", tvoc);
  Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Flame", sensorValue);
  Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Smoke", MP4Value);
  if (sensorValue <= 50) {
    Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FlameSensor", "Fire!!!");

  } else if (sensorValue > 50) {
    Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FlameSensor", "Safe");
  }
  if (MP4Value > 500) {
    Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_SmokeSensor", "Smoke detected!");
  } else if (MP4Value < 500) {
    Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_SmokeSensor", "No Smoke");
  }
  // }
  vTaskDelete(NULL);
}

void TaskREVPASSWORD(void *pvParameters) {
  (void)pvParameters;
  for (;;)  // A Task shall never return or exit.
  {
    if (e32ttl100.available() > 1) {
      char type[4];
      ResponseContainer rs = e32ttl100.receiveInitialMessage(sizeof(type));
      typeStr = rs.data;

      Serial.println(typeStr);
      if (typeStr == "SAL") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(SendTelegram));
        struct SendTelegram sendtelegram = *(SendTelegram *)rsc.data;

        if (*(int *)(sendtelegram.AlertPassword) == 1) {
          Serial.println("SendPassWord...");
          String message = "Password mới của bạn là: 165970";
          bot.sendMessage(CHAT_ID, message, "");
        }

        else if (*(int *)(sendtelegram.AlertTemp) == 1) {
          Serial.println("SendAlertTemp...");
          String messageTemp = "Cảnh báo: Nhiệt độ vượt quá 35°C! Nhiệt độ hiện tại: " + String(t) + "°C";
          bot.sendMessage(CHAT_ID, messageTemp, "");
        } else if (*(int *)(sendtelegram.AlertCO2) == 1) {
          Serial.println("SendAlertCO2...");
          String messageCO2 = "Cảnh báo: Nồng độ khí CO2 cao!!! Nồng độ khí CO2 hiện tại: " + String(co2) + "ppm";
          bot.sendMessage(CHAT_ID, messageCO2, "");
        } else if (*(int *)(sendtelegram.AlertFire) == 1) {
          Serial.println("SendAlertFire...");
          String messageFire = "Cảnh báo: Phát hiện có lửa!!!!";
          bot.sendMessage(CHAT_ID, messageFire, "");
        }

        else if (*(int *)(sendtelegram.AlertSmoke) == 1) {
          Serial.println("SendAlertSmoke...");
          String messageSmoke = "Cảnh báo: Phát hiện có khói!!!!";
          bot.sendMessage(CHAT_ID, messageSmoke, "");
        }

        rsc.close();
      } else {
        Serial.println("Something goes wrong!!");
      }
    }
    vTaskDelay(350 / portTICK_PERIOD_MS);
  }
}
