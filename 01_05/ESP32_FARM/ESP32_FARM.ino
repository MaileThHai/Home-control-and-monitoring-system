#include "Arduino.h"
#include "LoRa_E32.h"
#include <Wire.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <SHT3x.h>

#define FIREBASE_HOST "https://esp32-data-fb2d5-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "y9AzB0SNwyy4kReO3ot7wX3r2ZtWGPqrmFTD7ciZ"
FirebaseData fbdo;
#define WIFI_SSID "HUY QUANG"
#define WIFI_PASSWORD "khongbiet"

SHT3x Sensor;
LoRa_E32 e32ttl100(&Serial2, 15, 25, 33);

#define RAINSS 27
#define RelayBom 14
#define RelayBom1 12
#define MOIL 34
// const int dry = 13160;  // value for dry sensor
// const int wet = 5100;   // value for wet sensor

float t, h;
int rainvalue, moilvalue, percentmoil;

void TaskReadDataSSFARM(void *pvParameters);
void TaskSendDataFARM(void *pvParameters);
void TaskREVControlFARM(void *pvParameters);

struct DataSensorFARM {
  char typeF[4] = "FRM";
  byte temperature[4];
  byte humidity[4];
  byte Rain[4];
  byte Moil[4];
} datasensorfarm;

struct ControlPump {
  byte Control1[4];
  byte Control2[4];
} controlpump;

boolean runEvery(unsigned long interval, unsigned long *previousMillis) {
  unsigned long currentMillis = millis();
  if (currentMillis - *previousMillis >= interval) {
    *previousMillis = currentMillis;
    return true;
  }
  return false;
}
unsigned long previousMillis1 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(RelayBom, OUTPUT);
  pinMode(RelayBom1, OUTPUT);
  digitalWrite(RelayBom, HIGH);
  digitalWrite(RelayBom1, HIGH);

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

  xTaskCreatePinnedToCore(TaskReadDataSSFARM, "TaskReadDataSSFARM", 10000, NULL, 1, NULL, 0);

  xTaskCreatePinnedToCore(TaskREVControlFARM, "TaskREVControlFARM", 20000, NULL, 2, NULL, 1);
}

void loop() {
}

void TaskReadDataSSFARM(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    //SHT31//
    Sensor.UpdateData();
    // t = sht31.readTemperature();
    // h = sht31.readHumidity();
    t = Sensor.GetTemperature();
    Serial.println(Sensor.GetTemperature());
    h = Sensor.GetRelHumidity();
    Serial.println(Sensor.GetRelHumidity());
    //RAIN//
    rainvalue = digitalRead(RAINSS);
    //MOIL//
    moilvalue = analogRead(MOIL);
    // moilvalue = 600;
    percentmoil = map(moilvalue, 0, 1023, 0, 100);

    if (runEvery(10000, &previousMillis1)) {
      xTaskCreatePinnedToCore(TaskDataFirebaseFARM, "TaskDataFirebaseFARM", 20000, NULL, 1, NULL, 0);
    }

    vTaskDelay(350 / portTICK_PERIOD_MS);
  }
}

void TaskSendDataFARM(void *pvParameters) {
  (void)pvParameters;
  ///SHT31////
  strcpy(datasensorfarm.typeF, "FRM");
  // Gửi dữ liệu nhiệt độ
  *(float *)(datasensorfarm.temperature) = t;
  // Gửi dữ liệu độ ẩm
  *(float *)(datasensorfarm.humidity) = h;
  ///RAIN////
  *(uint16_t *)(datasensorfarm.Rain) = rainvalue;
  // Gửi dữ liệu TVOC
  *(uint16_t *)(datasensorfarm.Moil) = percentmoil;

  ResponseStatus rs = e32ttl100.sendFixedMessage(0, 1, 0xA, &datasensorfarm, sizeof(DataSensorFARM));
  Serial.println(rs.getResponseDescription());

  vTaskDelete(NULL);
}

void TaskDataFirebaseFARM(void *pvParameters) {
  (void)pvParameters;
  Firebase.setFloat(fbdo, "/DataSensor/Farm/Temperature", t);
  Firebase.setFloat(fbdo, "/DataSensor/Farm/Humidity", h);
  Firebase.setFloat(fbdo, "/DataSensor/Farm/Moil", percentmoil);
  if (rainvalue == 0) {
    Firebase.setString(fbdo, "/DataSensor/Farm/Status_RainSensor", "Rain!!!");

  } else if (rainvalue == 1) {
    Firebase.setString(fbdo, "/DataSensor/Farm/Status_RainSensor", "No Detect");
  }
  // }
  vTaskDelete(NULL);
}

void TaskREVControlFARM(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    // Serial.print("TaskControlLight: ");
    // Serial.println(xPortGetCoreID());

    if (e32ttl100.available() > 1) {
      char type[4];
      ResponseContainer rs = e32ttl100.receiveInitialMessage(sizeof(type));
      String typeStr = rs.data;


      Serial.println(typeStr);
      if (typeStr == "CPM") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(ControlPump));
        struct ControlPump controlpump = *(ControlPump *)rsc.data;

        Serial.println(*(int *)(controlpump.Control1));
        Serial.println(*(int *)(controlpump.Control2));

        // Kiểm tra giá trị của Control
        if (*(int *)(controlpump.Control1) == 1) {
          // Bật LED
          digitalWrite(RelayBom, LOW);
          Serial.println("Current not Flowing");
          Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump1", "ON");
        } else if (*(int *)(controlpump.Control1) == 0) {
          digitalWrite(RelayBom, HIGH);
          Serial.println("Current Flowing");
          Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump1", "OFF");
        } else if (*(int *)(controlpump.Control2) == 1) {
          digitalWrite(RelayBom1, LOW);
          Serial.println("Current not Flowing");
          Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump2", "OFF");
        } else if (*(int *)(controlpump.Control2) == 0) {
          digitalWrite(RelayBom1, HIGH);
          Serial.println("Current Flowing");
          Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump2", "ON");
        }
        // Close the response struct container
        rsc.close();
      }
      if (typeStr == "SSS") {
        vTaskDelay(500);
        Serial.println("SSSSSSSSSSSSSSSSSSSSSS");
        xTaskCreatePinnedToCore(TaskSendDataFARM, "TaskSendDataFARM", 10000, NULL, 4, NULL, 1);
      } else {
        // Serial.println("Something goes wrong!!");
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}


