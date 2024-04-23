#include "Arduino.h"
#include "LoRa_E32.h"
#include <Wire.h>

//=====================Tạo Task===============================
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// LoRa_E32 e32ttl100(&Serial2, 15, 7, 6);
HardwareSerial mySerial(1);  // RX, TX
LoRa_E32 e32ttl100(&mySerial, 45, 47, 48);

int control;

void TaskControlLight(void *pvParameters);
void TaskREVDataSensor(void *pvParameters);

//STRUCT SEND CONTROL////////////////////////
struct ControlLight {
  char type[20] = "ControlLight";
  char message[8];
  byte Control[4];
  char end[15];
} controllight;

//STRUCT REV DATA SENSOR/////////////////////
struct SensorSHT31 {
  // char type[10] = "SHT31DATA";
  char message[8];
  byte temperature[4];
  byte humidity[4];
  char end[15];
} sht31data;

struct FlameSensor {
  // char type[12] = "FLAMESENSOR";
  // char message[8];
  // char notify[5];
  byte ValFlameSensor[8];
  char end[15];
} flamesensor;

struct SensorCCS811 {
  // char type[20] = "CCS811DATA";
  byte CO2[5];
  byte TVOC[4];
  char end[15];
} sensorccs811;

struct SmokeSensor {
  // char type[20] = "SMOKESENSOR";
  byte ValSmokeSensor[6];
  char end[15];
} smokesensor;


void setup() {
  Serial.begin(9600);

  mySerial.begin(9600, SERIAL_8N1, 19, 20);

  while (!Serial) {
    ;
  }
  delay(100);

  e32ttl100.begin();

  xTaskCreatePinnedToCore(TaskControlLight, "TaskControlLight", 2046, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(TaskREVDataSensor, "TaskREVDataSensor", 7000, NULL, 1, NULL, 0);
}

void loop() {
}

void TaskControlLight(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    Serial.print("TaskControlLight: ");
    Serial.println(xPortGetCoreID());

    // Gửi control = 1
    control = 1;

    strcpy(controllight.type, "TaskControlLight");
    strcpy(controllight.message, "Living");
    strcpy(controllight.end, "==============");
    *(int *)(controllight.Control) = control;

    ResponseStatus rs = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllight, sizeof(ControlLight));
    Serial.println(rs.getResponseDescription());
    Serial.println("=================================================");
    vTaskDelay(5000);

    // Gửi control = 0 sau khi delay 5000ms
    control = 0;

    strcpy(controllight.type, "TaskControlLight");
    strcpy(controllight.message, "Living");
    strcpy(controllight.end, "==============");
    *(int *)(controllight.Control) = control;

    rs = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllight, sizeof(ControlLight));
    Serial.println(rs.getResponseDescription());
    Serial.println("=================================================");
    vTaskDelay(5000);
  }
}

void TaskREVDataSensor(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    Serial.print("TaskREVDataSensor: ");
    Serial.println(xPortGetCoreID());

    if (e32ttl100.available() > 1) {
      char type[20];  // increase size to accommodate "SHT31DATA"
      ResponseContainer rs = e32ttl100.receiveInitialMessage(sizeof(type));
      String typeStr = rs.data;

      Serial.println(typeStr);
      if (typeStr == "SHT31DATA") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(SensorSHT31));
        struct SensorSHT31 sht31data = *(SensorSHT31 *)rsc.data;

        Serial.println(sht31data.message);
        Serial.println(*(float *)(sht31data.temperature));
        // Print humidity as well
        Serial.println(*(float *)(sht31data.humidity));
        Serial.println(sht31data.end);
        // Close the response struct container
        rsc.close();
      } else if (typeStr == "CCS811DATA") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(SensorCCS811));
        struct SensorCCS811 sensorccs811 = *(SensorCCS811 *)rsc.data;

        // Serial.println(sht31data.message);
        Serial.println(*(float *)(sensorccs811.CO2));
        // Print humidity as well
        Serial.println(*(float *)(sensorccs811.TVOC));
        Serial.println(sensorccs811.end);
        // Close the response struct container
        rsc.close();
      } else if (typeStr == "FLAMESENSOR") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(FlameSensor));
        struct FlameSensor flamesensor = *(FlameSensor *)rsc.data;

        Serial.println(*(float *)(flamesensor.ValFlameSensor));

        Serial.println(flamesensor.end);
        //			free(rsc.data);
        rsc.close();
      } else if (typeStr == "SMOKESENSOR") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(SmokeSensor));
        struct SmokeSensor smokesensor = *(SmokeSensor *)rsc.data;

        Serial.println(*(float *)(smokesensor.ValSmokeSensor));

        Serial.println(smokesensor.end);
        //			free(rsc.data);
        rsc.close();
      } else {
        Serial.println("Something goes wrong!!");
      }
    }
    Serial.println("=================================================");
    vTaskDelay(6000);
  }
}
