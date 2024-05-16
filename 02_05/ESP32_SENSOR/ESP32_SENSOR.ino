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
#include "esp_task_wdt.h"

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

struct DataSensorFARM {
  byte temperatureF[4];
  byte humidityF[4];
  byte Rain[4];
  byte Moil[4];
} datasensorfarm;
float tFRM, hFRM;
uint16_t RAINFRM = 0;
uint16_t MOILFRM = 0;

struct STATUSFB {
  byte dvc1[4];
  byte dvc2[4];
  byte pumps[1];
} statusfb;

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
  esp_task_wdt_deinit();
  //Check SHT31 is running
  if (!sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  if (!ccs.begin()) {
    Serial.println("Failed to start sensor! Please check your wiring.");
    while (1)
      ;
  }
  // Wait for the sensor to be ready6
  while (!ccs.available())
    ;

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


  xTaskCreatePinnedToCore(TaskReadDataSSLVR, "TaskReadDataSSLVR", 8000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskSendDataLVR, "TaskSendDataLVR", 10000, NULL, 1, NULL, 1);
  //
  xTaskCreatePinnedToCore(TaskREVPASSWORD, "TaskREVPASSWORD", 21000, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskFB, "TaskFB", 21000, NULL, 2, NULL, 0);
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

    if (ccs.available()) {
      if (!ccs.readData()) {
        co2 = ccs.geteCO2();
        tvoc = ccs.getTVOC();
        Serial.print("CO2: ");
        Serial.print(co2);
        Serial.print("ppm, TVOC: ");
        Serial.println(tvoc);
      }
    }

    //FLAME//
    sensorValue = analogRead(Fire_GPIO);
    //SMOKE//
    MP4Value = analogRead(MP4_GPIO);
    Serial.print("SMoke: ");
    Serial.println(MP4Value);

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
void TaskFB(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    if (runEvery(60000, &previousMillis1)) {
      xTaskCreatePinnedToCore(TaskDataFirebase, "TaskDataFirebase", 20000, NULL, 1, NULL, 0);
    }
    vTaskDelay(350);
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

    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}
void TaskDataFirebase(void *pvParameters) {
  (void)pvParameters;
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

  Firebase.setFloat(fbdo, "/DataSensor/Farm/Temperature", tFRM);
  Firebase.setFloat(fbdo, "/DataSensor/Farm/Humidity", hFRM);
  Firebase.setFloat(fbdo, "/DataSensor/Farm/Moil", MOILFRM);
  if (RAINFRM == 0) {
    Firebase.setString(fbdo, "/DataSensor/Farm/Status_RainSensor", "Rain!!!");

  } else if (RAINFRM == 1) {
    Firebase.setString(fbdo, "/DataSensor/Farm/Status_RainSensor", "No Detect");
  }
  vTaskDelete(NULL);
}
void TaskREVPASSWORD(void *pvParameters) {
  (void)pvParameters;
  for (;;)  // A Task shall never return or exit.
  {
    if (e32ttl100.available() > 1) {
      char type[4];
      ResponseContainer rs = e32ttl100.receiveInitialMessage(sizeof(type));
      String typeStr = rs.data;

      Serial.println(typeStr);
      if (typeStr == "STB") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(statusfb));
        struct STATUSFB statusfb = *(STATUSFB *)rsc.data;
        if (*(int *)(statusfb.dvc1) == 1) {
          Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_Light", "ON");
        }
        if (*(int *)(statusfb.dvc1) == 0) {
          Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_Light", "OFF");
        }
        if (*(int *)(statusfb.dvc2) == 1) {
          Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FAN", "ON");
        }
        if (*(int *)(statusfb.dvc2) == 0) {
          Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FAN", "OFF");
        }
        if (*(int *)(statusfb.pumps) == 1) {
          Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump1", "ON");
        }
        if (*(int *)(statusfb.pumps) == 0) {
          Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump1", "OFF");
        }
      }
      if (typeStr == "SAL") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(SendTelegram));
        struct SendTelegram sendtelegram = *(SendTelegram *)rsc.data;

        if (*(int *)(sendtelegram.AlertPassword) == 1) {
          Serial.println("SendPassWord...");
          String message = "Password mới của bạn là: 165970";
          bot.sendMessage(CHAT_ID, message, "");
        }

        if (*(int *)(sendtelegram.AlertTemp) == 1) {
          Serial.println("SendAlertTemp...");
          String messageTemp = "Cảnh báo: Nhiệt độ vượt quá 35°C! Nhiệt độ hiện tại: " + String(t) + "°C";
          bot.sendMessage(CHAT_ID, messageTemp, "");
        }
        if (*(int *)(sendtelegram.AlertFire) == 1) {
          Serial.println("SendAlertFire...");
          String messageFire = "Cảnh báo: Phát hiện có lửa!!!!";
          bot.sendMessage(CHAT_ID, messageFire, "");
        }
        if (*(int *)(sendtelegram.AlertSmoke) == 1) {
          Serial.println("SendAlertSmoke...");
          String messageSmoke = "Cảnh báo: Phát hiện có khói!!!!";
          bot.sendMessage(CHAT_ID, messageSmoke, "");
        }
        if (*(int *)(sendtelegram.AlertCO2) == 1) {
          Serial.println("SendAlertCO2...");
          String messageCO2 = "Cảnh báo: Nồng độ khí CO2 cao!!! Nồng độ khí CO2 hiện tại: " + String(co2) + "ppm";
          bot.sendMessage(CHAT_ID, messageCO2, "");
        }

        rsc.close();
      }
      if (typeStr == "FRM") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(DataSensorFARM));
        struct DataSensorFARM datasensorfarm = *(DataSensorFARM *)rsc.data;

        Serial.println(*(float *)(datasensorfarm.temperatureF));
        tFRM = *(float *)(datasensorfarm.temperatureF);
        Serial.println(*(float *)(datasensorfarm.humidityF));
        hFRM = *(float *)(datasensorfarm.humidityF);
        Serial.println(*(uint16_t *)(datasensorfarm.Rain));
        RAINFRM = *(uint16_t *)(datasensorfarm.Rain);
        Serial.println(*(uint16_t *)(datasensorfarm.Moil));
        MOILFRM = *(uint16_t *)(datasensorfarm.Moil);
        // Close the response struct container
        rsc.close();
      } else {
        // Serial.println("Something goes wrong!!");
      }
    }
    vTaskDelay(350 / portTICK_PERIOD_MS);
  }
}
