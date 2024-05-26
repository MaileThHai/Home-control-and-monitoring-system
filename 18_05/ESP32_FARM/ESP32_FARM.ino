#include "Arduino.h"
#include "LoRa_E32.h"
#include <Wire.h>
#include <SHT3x.h>

SHT3x Sensor;
LoRa_E32 e32ttl100(&Serial2, 15, 25, 33);

#define RAINSS 27
#define RelayBom 14
#define RelayBom1 12
#define MOIL 34

float t, h;
int rainvalue, moilvalue, percentmoil;

void TaskReadDataSSFARM(void *pvParameters);
void TaskSendDataFARM(void *pvParameters);
void TaskREVControlFARM(void *pvParameters);

struct DataSensorFARM {
  char type[4] = "FRM";
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

    if (runEvery(30000, &previousMillis1)) {
      xTaskCreatePinnedToCore(TaskSendDataFARM, "TaskSendDataFARM", 10000, NULL, 4, NULL, 1);
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void TaskSendDataFARM(void *pvParameters) {
  (void)pvParameters;
  ///SHT31////
  strcpy(datasensorfarm.type, "FRM");
  // Gửi dữ liệu nhiệt độ
  *(float *)(datasensorfarm.temperature) = t;
  // Gửi dữ liệu độ ẩm
  *(float *)(datasensorfarm.humidity) = h;
  ///RAIN////
  *(uint16_t *)(datasensorfarm.Rain) = rainvalue;
  // Gửi dữ liệu TVOC
  *(uint16_t *)(datasensorfarm.Moil) = percentmoil;

  vTaskDelay(500 / portTICK_PERIOD_MS);
  ResponseStatus rs = e32ttl100.sendFixedMessage(0, 1, 0xA, &datasensorfarm, sizeof(DataSensorFARM));
  Serial.println(rs.getResponseDescription());
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &datasensorfarm, sizeof(DataSensorFARM));
  Serial.println(rs1.getResponseDescription());
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ResponseStatus rs2 = e32ttl100.sendFixedMessage(0, 3, 0xA, &datasensorfarm, sizeof(DataSensorFARM));
  Serial.println(rs1.getResponseDescription());

  vTaskDelete(NULL);
}

void TaskREVControlFARM(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
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
          digitalWrite(RelayBom, HIGH);
          Serial.println("Current Flowing");
        } else if (*(int *)(controlpump.Control1) == 0) {
          digitalWrite(RelayBom, LOW);
          Serial.println("Current not Flowing");
        } else if (*(int *)(controlpump.Control2) == 1) {
          digitalWrite(RelayBom1, HIGH);
          Serial.println("Current Flowing");
        } else if (*(int *)(controlpump.Control2) == 0) {
          digitalWrite(RelayBom1, LOW);
          Serial.println("Current not Flowing");
        }
        // Close the response struct container
        rsc.close();
      }
      else {
        // Serial.println("Something goes wrong!!");
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
