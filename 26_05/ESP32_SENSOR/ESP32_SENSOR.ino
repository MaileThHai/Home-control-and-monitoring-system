#include "Arduino.h"
#include "LoRa_E32.h"
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#include <UniversalTelegramBot.h>
#include "Adafruit_SHT31.h"
#include "Adafruit_CCS811.h"

String Web_App_URL = "https://script.google.com/macros/s/AKfycbw03K3sj-Mc9EMVFMTxFLtdgOBjXnlEUAYZAf_7jK9s2POrI8B9wznknU6m9sdXwjKE/exec";
SemaphoreHandle_t TaskMutex;
#define WIFI_SSID "iPhone 11"
#define WIFI_PASSWORD "1234567800"

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
int checktemp = 0;
int checkco2 = 0;
int checkfire = 0;
int checksmoke = 0;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillis4 = 0;

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

String stt1 = "Off";
String stt2 = "Off";
String stt3 = "Off";
String sttfire;
String sttsmoke;
String sttrain;

struct SendTelegram {
  byte AlertPassword[4];
} sendtelegram;

struct ControlLVR {
  byte Control1[4];
  byte Control2[4];
  byte ControlPMEL[4];
} controllvr;

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

  pinMode(Fire_GPIO, INPUT);

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
  delay(5000);

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

  e32ttl100.begin();

  TaskMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(TaskReadDataSSLVR, "TaskReadDataSSLVR", 8000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskSendDataLVR, "TaskSendDataLVR", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(SendDataSheetTask, "SendDataSheetTask", 20000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskRevDataLVR, "TaskRevDataLVR", 10000, NULL, 1, NULL, 1);
}
void loop() {
}

void TaskReadDataSSLVR(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    if (runEvery(2000, &previousMillis2)) {
      //SHT31//
      t = sht31.readTemperature();
      h = sht31.readHumidity();
      Serial.println(t);
      Serial.println(h);
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
      // co2 = 759;
      // tvoc = 15;

      //FLAME//
      sensorValue = analogRead(Fire_GPIO);
      Serial.println(sensorValue);
      //SMOKE//
      MP4Value = analogRead(MP4_GPIO);
      Serial.println(MP4Value);

      if (sensorValue > 0 && sensorValue < 50) {
        sttfire = "Detect";
        checkfire = 1;
      } else if (sensorValue > 50) {
        sttfire = "NoDetect";
        checkfire = 0;
      }
      if (MP4Value > 700) {
        sttsmoke = "Detect";
        checksmoke = 1;
      } else if (MP4Value < 700) {
        sttsmoke = "NoDetect";
        checksmoke = 0;
      }
      if (t > 37) {
        checktemp = 1;
      } else if (t < 37) {
        checktemp = 0;
      }
      if (co2 > 1500) {
        checkco2 = 1;
      } else if (co2 < 1500) {
        checkco2 = 0;
      }
    }
    if (runEvery(40000, &previousMillis4)) {
      if (checktemp == 1) {
        Serial.println("SendAlertTemp...");
        String messageTemp = "Cảnh báo: Nhiệt độ vượt quá 37°C! Nhiệt độ hiện tại: " + String(t) + "°C";
        bot.sendMessage(CHAT_ID, messageTemp, "");
      } else if (checkfire == 1) {
        Serial.println("SendAlertFire...");
        String messageFire = "Cảnh báo: Phát hiện có lửa!!!!";
        bot.sendMessage(CHAT_ID, messageFire, "");
      } else if (checksmoke == 1) {
        Serial.println("SendAlertSmoke...");
        String messageSmoke = "Cảnh báo: Phát hiện có khói!!!!";
        bot.sendMessage(CHAT_ID, messageSmoke, "");
      } else if (checkco2 == 1) {
        Serial.println("SendAlertCO2...");
        String messageCO2 = "Cảnh báo: Nồng độ khí CO2 cao!!! Nồng độ khí CO2 hiện tại: " + String(co2) + "ppm";
        bot.sendMessage(CHAT_ID, messageCO2, "");
      }
    }
    vTaskDelay(350);
  }
}

void TaskSendDataLVR(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    if (runEvery(25000, &previousMillis3)) {
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
      vTaskDelay(500);
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 3, 0xA, &datasensorLVR, sizeof(DataSensorLVR));
    }
    vTaskDelay(350);
  }
}

void TaskRevDataLVR(void *pvParameters) {
  (void)pvParameters;
  for (;;)  // A Task shall never return or exit.
  {
    while (xSemaphoreTake(TaskMutex, portMAX_DELAY) != pdTRUE)
      ;
    if (e32ttl100.available() > 1) {
      char type[4];
      ResponseContainer rs = e32ttl100.receiveInitialMessage(sizeof(type));
      String typeStr = rs.data;
      // Serial.println(typeStr);
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

        if (RAINFRM == 0) {
          sttrain = "Detect";
        } else if (RAINFRM == 1) {
          sttrain = "NoDetect";
        }
        // Close the response struct container
        rsc.close();
      } else if (typeStr == "SAL") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(SendTelegram));
        struct SendTelegram sendtelegram = *(SendTelegram *)rsc.data;

        if (*(int *)(sendtelegram.AlertPassword) == 1) {
          Serial.println("SendPassWord...");
          String message = "Password mới của bạn là: 165970";
          bot.sendMessage(CHAT_ID, message, "");
        }
        rsc.close();
      } else if (typeStr == "CLV") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(ControlLVR));
        struct ControlLVR controllvr = *(ControlLVR *)rsc.data;

        // Kiểm tra giá trị của Control
        if (*(int *)(controllvr.Control1) == 1) {
          Serial.println("Current1 Flowing");
          stt1 = "On";
          Serial.println(stt1);
        } else if (*(int *)(controllvr.Control1) == 0) {
          Serial.println("Current1 not Flowing");
          stt1 = "Off";
        }
        if (*(int *)(controllvr.Control2) == 1) {
          Serial.println("Current2 Flowing");
          stt2 = "On";
        } else if (*(int *)(controllvr.Control2) == 0) {
          Serial.println("Current2 not Flowing");
          stt2 = "Off";
        }
        if (*(int *)(controllvr.ControlPMEL) == 1) {
          Serial.println("PUMP Flowing");
          stt3 = "On";
        } else if (*(int *)(controllvr.ControlPMEL) == 0) {
          Serial.println("PUMP NOT Flowing");
          stt3 = "Off";
        }
        // Close the response struct container
        rsc.close();
      } else {
        // Serial.println("Something goes wrong!!");
      }
    }
    xSemaphoreGive(TaskMutex);
    vTaskDelay(350 / portTICK_PERIOD_MS);
  }
}

void SendDataSheetTask(void *parameter) {
  (void)parameter;
  for (;;) {
    if (runEvery(60000, &previousMillis1)) {
      while (xSemaphoreTake(TaskMutex, portMAX_DELAY) != pdTRUE)
        ;
      if (WiFi.status() == WL_CONNECTED) {
        // Create a URL for sending or writing data to Google Sheets.
        String Send_Data_URL = Web_App_URL + "?sts=write";
        Send_Data_URL += "&temp=" + String(t);
        Send_Data_URL += "&hum=" + String(h);
        Send_Data_URL += "&co2=" + String(co2);
        Send_Data_URL += "&tvoc=" + String(tvoc);
        Send_Data_URL += "&fire=" + String(sttfire);
        Send_Data_URL += "&smoke=" + String(sttsmoke);
        Send_Data_URL += "&dvc1=" + String(stt1);
        Send_Data_URL += "&dvc2=" + String(stt2);
        Send_Data_URL += "&tempf=" + String(tFRM);
        Send_Data_URL += "&humf=" + String(hFRM);
        Send_Data_URL += "&soil=" + String(MOILFRM);
        Send_Data_URL += "&rain=" + String(sttrain);
        Send_Data_URL += "&pump=" + String(stt3);

        Serial.println();
        Serial.println("-------------");
        Serial.println("Send data to Google Spreadsheet...");
        Serial.print("URL : ");
        Serial.println(Send_Data_URL);

        //::::::::::::::::::The process of sending or writing data to Google Sheets.
        // Initialize HTTPClient as "http".
        HTTPClient http;

        // HTTP GET Request.
        http.begin(Send_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        // Gets the HTTP status code.
        int httpCode = http.GET();
        Serial.print("HTTP Status Code : ");
        Serial.println(httpCode);

        // Getting response from google sheets.
        String payload;
        if (httpCode > 0) {
          payload = http.getString();
          Serial.println("Payload : " + payload);
        }

        http.end();
        //::::::::::::::::::
        Serial.println("-------------");
      }
      xSemaphoreGive(TaskMutex);
    }
  }
  vTaskDelay(350);
}