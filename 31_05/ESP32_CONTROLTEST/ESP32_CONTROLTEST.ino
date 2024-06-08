#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "LoRa_E32.h"
#include <Wire.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <Firebase_ESP_Client.h>
#include <addons/RTDBHelper.h>
#include <addons/RTDBHelper.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "esp_task_wdt.h"

#define WIFI_SSID "HUY QUANG"
#define WIFI_PASSWORD "khongbiet"

#define API_KEY "AIzaSyAZx6zK3WP8eXC6kAEJIonxL_Mtmxxi750"
#define DATABASE_URL "https://kltn-bd6e3-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "nguyenhao123210@gmail.com"
#define USER_PASSWORD "NguyenHao@123210"
FirebaseData fbdo;
FirebaseData stream;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
volatile bool dataChanged = false;
String parentPath = "/KLTN";
String childPath[2] = { "/node1", "/node3" };
size_t childPathSize = 2;


void printResult(FirebaseData &data);

unsigned long sendDataPrevMillis = 0;

LoRa_E32 e32ttl100(&Serial2, 15, 12, 13);
int ControlREV;
#define RLYPIN1 32
#define RLYPIN2 33

#define FPSerial Serial1
DFRobotDFPlayerMini myDFPlayer;

static int count = 0;
struct ControlLVR {
  byte Control1[4];
  byte Control2[4];
  byte ControlPMEL[4];
} controllvr;
int ct1 = 0;
int ct2 = 0;
int ct3 = 0;

struct SendWeather {
  char type[4] = "SWR";
  byte checkcn[1];
} sendweather;

struct ControlWeb {
  char type[4] = "CTW";
  byte ctlw1[1];
  byte ctlw2[1];
  byte ctlw3[1];
} controlweb;
uint8_t ctrolweb1;
uint8_t ctrolweb2;
uint8_t ctrolweb3;
uint8_t ctrolwebloa;

struct DataSensorFARM {
  byte temperatureF[4];
  byte humidityF[4];
  byte Rain[4];
  byte Moil[4];
} datasensorfarm;
float tFRM, hFRM;
uint16_t RAINFRM = 0;
uint16_t MOILFRM = 0;

struct DataSensorLVR {
  byte temperature[4];
  byte humidity[4];
  byte CO2[4];
  byte TVOC[4];
  byte ValFlameSensor[4];
  byte ValSmokeSensor[4];
} datasensorLVR;
float tLVR, hLVR;
uint16_t CO2LVR = 0;
uint16_t TVOCLVR = 0;
uint16_t FLMLVR = 0;
uint16_t SMKLVR = 0;

int checktemp = 0;
int checkco2 = 0;
int checkfire = 0;
int checksmoke = 0;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillis8 = 0;
boolean runEvery(unsigned long interval, unsigned long *previousMillis) {
  unsigned long currentMillis = millis();
  if (currentMillis - *previousMillis >= interval) {
    *previousMillis = currentMillis;
    return true;
  }
  return false;
}

SemaphoreHandle_t taskmutex;

int sttfire = 0;
int sttsmoke = 0;
int sttco2 = 0;
int stttemp = 0;
unsigned long previousMillis4 = 0;
unsigned long previousMillis5 = 0;
unsigned long previousMillis6 = 0;
unsigned long previousMillis7 = 0;
unsigned long audioStartMillis = 0;
unsigned long audioDuration = 0;
enum AlertState { NO_ALERT,
                  TEMP_ALERT,
                  FIRE_ALERT,
                  SMOKE_ALERT,
                  CO2_ALERT };
AlertState currentAlert = NO_ALERT;

void setup() {
  Serial.begin(9600);
  pinMode(RLYPIN1, OUTPUT);
  pinMode(RLYPIN2, OUTPUT);
  digitalWrite(RLYPIN1, HIGH);
  digitalWrite(RLYPIN2, HIGH);
  esp_task_wdt_deinit();

  FPSerial.begin(9600, SERIAL_8N1, 27, 26);
  myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true);
  // if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true)) {  //Use serial to communicate with mp3.
  //   Serial.println(F("Unable to begin:"));
  //   Serial.println(F("1.Please recheck the connection!"));
  //   Serial.println(F("2.Please insert the SD card!"));
  //   while (true) {
  //     delay(0);  // Code to compatible with ESP8266 watch dog.
  //   }
  // }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(29);  //Set volume value. From 0 to 30

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

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(2048, 1024);
  stream.setBSSLBufferSize(2048, 1024);
  Firebase.begin(&config, &auth);


  if (!Firebase.RTDB.beginMultiPathStream(&stream, parentPath))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.RTDB.setMultiPathStreamCallback(&stream, streamCallback, streamTimeoutCallback);

  e32ttl100.begin();

  taskmutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(TaskREVData, "TaskREVData", 17000, NULL, 4, NULL, 1);
  xTaskCreatePinnedToCore(SendConnectTask, "SendConnectTask", 8000, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(TaskAlert, "TaskAlert", 8000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskControlWeb, "TaskControlWeb", 12000, NULL, 3, NULL, 0);
  // xTaskCreatePinnedToCore(TaskDataFirebase, "TaskDataFirebase", 10000, NULL, 1, NULL, 0);
}

void loop() {
  // Empty. Everything is handled by tasks.
}
void TaskREVData(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    // while (xSemaphoreTake(taskmutex, portMAX_DELAY) != pdTRUE)
    //   ;
    if (e32ttl100.available() > 1) {
      char type[4];
      ResponseContainer rs = e32ttl100.receiveInitialMessage(sizeof(type));
      String typeStr = rs.data;

      // Serial.println(typeStr);
      if (typeStr == "CLV") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(ControlLVR));
        struct ControlLVR controllvr = *(ControlLVR *)rsc.data;

        // Kiểm tra giá trị của Control
        if (*(int *)(controllvr.Control1) == 1) {
          // Bật LED
          digitalWrite(RLYPIN1, LOW);
          Serial.println("Current1 Flowing");
          ct1 = 1;
        } else if (*(int *)(controllvr.Control1) == 0) {
          digitalWrite(RLYPIN1, HIGH);
          Serial.println("Current1 not Flowing");
          ct1 = 0;
        }
        if (*(int *)(controllvr.Control2) == 1) {
          digitalWrite(RLYPIN2, LOW);
          Serial.println("Current2 Flowing");
          ct2 = 1;
        } else if (*(int *)(controllvr.Control2) == 0) {
          digitalWrite(RLYPIN2, HIGH);
          Serial.println("Current2 not Flowing");
          ct2 = 0;
        }
        if (*(int *)(controllvr.ControlPMEL) == 1) {
          Serial.println("PUMP Flowing");
          ct3 = 1;
        } else if (*(int *)(controllvr.ControlPMEL) == 0) {
          Serial.println("PUMP NOT Flowing");
          ct3 = 0;
        }
        // Close the response struct container
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
      }
      if (typeStr == "LVR") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(DataSensorLVR));
        struct DataSensorLVR datasensorLVR = *(DataSensorLVR *)rsc.data;

        Serial.println(*(float *)(datasensorLVR.temperature));
        tLVR = *(float *)(datasensorLVR.temperature);
        Serial.println(*(float *)(datasensorLVR.humidity));
        hLVR = *(float *)(datasensorLVR.humidity);
        Serial.println(*(uint16_t *)(datasensorLVR.CO2));
        CO2LVR = *(uint16_t *)(datasensorLVR.CO2);
        Serial.println(*(uint16_t *)(datasensorLVR.TVOC));
        TVOCLVR = *(uint16_t *)(datasensorLVR.TVOC);
        Serial.println(*(uint16_t *)(datasensorLVR.ValFlameSensor));
        FLMLVR = *(uint16_t *)(datasensorLVR.ValFlameSensor);
        Serial.println(*(uint16_t *)(datasensorLVR.ValSmokeSensor));
        SMKLVR = *(uint16_t *)(datasensorLVR.ValSmokeSensor);

        // Close the response struct container
        rsc.close();
      } else {
      }
    }
    // xSemaphoreGive(taskmutex);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void TaskAlert(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    if (FLMLVR > 0 && FLMLVR < 50) {
      checkfire = 1;
    } else if (FLMLVR > 50) {
      checkfire = 0;
    }
    if (SMKLVR > 700) {
      checksmoke = 1;
    } else if (SMKLVR < 700) {
      checksmoke = 0;
    }
    if (tLVR > 37) {
      checktemp = 1;
    } else if (tLVR < 37) {
      checktemp = 0;
    }
    if (CO2LVR > 1500) {
      checkco2 = 1;
    } else if (CO2LVR < 1500) {
      checkco2 = 0;
    }

    if (currentAlert == NO_ALERT && runEvery(5000, &previousMillis3)) {
      if ((checkfire == 1) && (sttfire == 0)) {
        Serial.println("SendAlertFire...");
        myDFPlayer.playMp3Folder(1);
        currentAlert = FIRE_ALERT;
        audioDuration = 31000;
        audioStartMillis = millis();
        sttfire = 1;
      } else if ((checksmoke == 1) && (sttsmoke == 0)) {
        Serial.println("SendAlertSmoke...");
        myDFPlayer.playMp3Folder(2);
        currentAlert = SMOKE_ALERT;
        audioDuration = 31000;
        audioStartMillis = millis();
        sttsmoke = 1;
      } else if ((checkco2 == 1) && (sttco2 == 0)) {
        Serial.println("SendAlertCO2...");
        myDFPlayer.playMp3Folder(3);
        currentAlert = CO2_ALERT;
        audioDuration = 31000;
        audioStartMillis = millis();
        sttco2 = 1;
      } else if ((checktemp == 1) && (stttemp == 0)) {
        Serial.println("SendAlertTemp...");
        myDFPlayer.playMp3Folder(4);
        currentAlert = TEMP_ALERT;
        audioDuration = 31000;
        audioStartMillis = millis();
        stttemp = 1;
      }
    }

    if (currentAlert != NO_ALERT) {
      if (millis() - audioStartMillis >= audioDuration) {
        currentAlert = NO_ALERT;
      }
    }
    if ((sttfire == 1) && runEvery(150000, &previousMillis4)) {
      sttfire = 0;
    }
    if (sttsmoke == 1 && runEvery(150000, &previousMillis5)) {
      sttsmoke = 0;
    }
    if (sttco2 == 1 && runEvery(150000, &previousMillis6)) {
      sttco2 = 0;
    }
    if (stttemp == 1 && runEvery(150000, &previousMillis7)) {
      stttemp = 0;
    }
    vTaskDelay(350);
  }
}
void SendConnectTask(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    // while (xSemaphoreTake(taskmutex, portMAX_DELAY) != pdTRUE)
    //   ;
    if (runEvery(60000, &previousMillis8)) {
      strcpy(sendweather.type, "SWR");
      *(uint8_t *)(sendweather.checkcn) = 1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 1, 0xA, &sendweather, sizeof(SendWeather));
      Serial.println(rs.getResponseDescription());
      vTaskDelay(350);
    }
    // xSemaphoreGive(taskmutex);
    vTaskDelay(500);
  }
}
void streamCallback(MultiPathStream stream) {
  size_t numChild = sizeof(childPath) / sizeof(childPath[0]);

  for (size_t i = 0; i < numChild; i++) {
    if (stream.get(childPath[i])) {
      Serial.printf("path: %s, event: %s, type: %s, value: %s%s", stream.dataPath.c_str(), stream.eventType.c_str(), stream.type.c_str(), stream.value.c_str(), i < numChild - 1 ? "\n" : "");
    }
  }
  Serial.println();
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", stream.payloadLength(), stream.maxPayloadLength());
  dataChanged = true;
}
void streamTimeoutCallback(bool timeout) {
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
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
    if (strcmp(key, "Relay1_1") == 0 && kv.value().is<const char *>()) {
      const char *value = kv.value();
      if (strcmp(value, "ON") == 0) {
        ctrolweb1 = 1;
        Serial.println("DVC1ON");
      } else if (strcmp(value, "OFF") == 0) {
        ctrolweb1 = 0;
        Serial.println("DVC1OFF");
      }
    }
    if (strcmp(key, "Relay1_2") == 0 && kv.value().is<const char *>()) {
      const char *value = kv.value();
      if (strcmp(value, "ON") == 0) {
        ctrolweb2 = 1;
        Serial.println("DVC2ON");
      } else if (strcmp(value, "OFF") == 0) {
        ctrolweb2 = 0;
        Serial.println("DVC2OFF");
      }
    }
    if (strcmp(key, "Relay1_3") == 0 && kv.value().is<const char *>()) {
      const char *value = kv.value();
      if (strcmp(value, "ON") == 0) {
        Serial.println("DVC3ON");
      } else if (strcmp(value, "OFF") == 0) {
        Serial.println("DVC3OFF");
      }
    }
    if (strcmp(key, "Relay1_4") == 0 && kv.value().is<const char *>()) {
      const char *value = kv.value();
      if (strcmp(value, "ON") == 0) {
        Serial.println("DVC4ON");
      } else if (strcmp(value, "OFF") == 0) {
        Serial.println("DVC4OFF");
      }
    }
    if (strcmp(key, "Loa") == 0 && kv.value().is<const char *>()) {
      const char *value = kv.value();
      if (strcmp(value, "ON") == 0) {
        Serial.println("LOAON");
      } else if (strcmp(value, "OFF") == 0) {
        Serial.println("LOAOFF");
      }
    }
    if (strcmp(key, "Relay3_1") == 0 && kv.value().is<const char *>()) {
      const char *value = kv.value();
      if (strcmp(value, "ON") == 0) {
        ctrolweb3 = 1;
        Serial.println("DVCPMON");
      } else if (strcmp(value, "OFF") == 0) {
        ctrolweb3 = 0;
        Serial.println("DVCPMOFF");
      }
    }
  }
}

void TaskControlWeb(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    if (ctrolweb1 == 1) {
      ct1 = 1;
      Serial.println("123");
    } else if (ctrolweb1 == 0) {
      ct1 = 0;
      Serial.println("456");
    }
    if (ctrolweb2 == 1) {
      ct2 = 1;
      Serial.println("7");
    } else if (ctrolweb2 == 0) {
      ct2 = 0;
      Serial.println("8");
    }
    if (ctrolweb3 == 1) {
      ct3 = 1;
      Serial.println("9");
    } else if (ctrolweb1 == 0) {
      ct3 = 0;
      Serial.println("10");
    }
    // strcpy(controlweb.type, "CTW");
    // *(uint8_t *)(controlweb.ctlw1) = ctrolweb1;
    // ResponseStatus rs = e32ttl100.sendFixedMessage(0, 1, 0xA, &controlweb, sizeof(ControlWeb));
    // // Serial.println(rs.getResponseDescription());
    // String statusLight;
    // if (Firebase.getString(fbdo, "/DataSensor/LivingRoom/Status_Light")) {
    //   statusLight = fbdo.stringData();
    //   Serial.print("Status of Light in Living Room: ");
    //   // Serial.println(statusLight);
    //   if (statusLight == "ON"){
    //     Serial.println("ON");
    //   }
    //   if (statusLight == "OFF"){
    //     Serial.println("OFF");
    //   }
    // } else {
    //   Serial.println("Failed to get status of Light from Firebase!");
    //   Serial.println(fbdo.errorReason());
    // }
    vTaskDelay(1000);
  }
}

void TaskDataFirebase(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    // int co21 = 999;
    //   Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Temperature", tLVR);
    //   Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Humidity", hLVR);
    // Firebase.setInt(fbdo, "/KLTN/node1/sensor/CO", co21);
    //   Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/TVOC", TVOCLVR);
    //   Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Flame", FLMLVR);
    //   Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Smoke", SMKLVR);
    //   if ((FLMLVR > 0) && (FLMLVR < 50)) {
    //     Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FlameSensor", "Fire!!!");

    //   } else if (FLMLVR > 50) {
    //     Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FlameSensor", "Safe");
    //   }
    //   if (SMKLVR > 700) {
    //     Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_SmokeSensor", "Smoke detected!");
    //   } else if (SMKLVR < 700) {
    //     Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_SmokeSensor", "No Smoke");
    //   }

    //   Firebase.setFloat(fbdo, "/DataSensor/Farm/Temperature", tFRM);
    //   Firebase.setFloat(fbdo, "/DataSensor/Farm/Humidity", hFRM);
    //   Firebase.setFloat(fbdo, "/DataSensor/Farm/Moil", MOILFRM);
    //   if (RAINFRM == 0) {
    //     Firebase.setString(fbdo, "/DataSensor/Farm/Status_RainSensor", "Rain!!!");
    //   } else if (RAINFRM == 1) {
    //     Firebase.setString(fbdo, "/DataSensor/Farm/Status_RainSensor", "No Detect");
    //   }
    //   if (ct1 == 1) {
    //     Serial.println("1");
    //     Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_Light", "ON");
    //   }
    //   if (ct1 == 0) {
    //     Serial.println("0");
    //     Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_Light", "OFF");
    //   }
    //   if (ct2 == 1) {
    //     Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FAN", "ON");
    //   }
    //   if (ct2 == 0) {
    //     Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FAN", "OFF");
    //   }
    //   if (ct3 == 1) {
    //     Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump1", "ON");
    //   }
    //   if (ct3 == 0) {
    //     Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump1", "OFF");
    //   }

    vTaskDelay(350);
  }
}