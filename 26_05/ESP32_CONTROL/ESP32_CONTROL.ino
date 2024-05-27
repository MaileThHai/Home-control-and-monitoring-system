#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "LoRa_E32.h"
#include <Wire.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <FirebaseESP32.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "esp_task_wdt.h"

const char *ssid = "HUY QUANG";
const char *password = "khongbiet";

#define FIREBASE_HOST "https://esp32-data-fb2d5-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "y9AzB0SNwyy4kReO3ot7wX3r2ZtWGPqrmFTD7ciZ"
FirebaseData fbdo;

LoRa_E32 e32ttl100(&Serial2, 15, 12, 13);
// const int relay = 26;
int ControlREV;
#define RLYPIN1 32
#define RLYPIN2 33

#define FPSerial Serial1
DFRobotDFPlayerMini myDFPlayer;

String openWeatherMapApiKey = "8abc699afb41ebfb14b3913e4d627245";
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
  byte mintempday1[4];
  byte maxtempday1[4];
  byte maxhumday1[2];
  byte maxpresday1[2];
  byte maxwindday1[4];

  byte mintempday2[4];
  byte maxtempday2[4];
  byte maxhumday2[2];
  byte maxpresday2[2];
  byte maxwindday2[4];

  byte mintempday3[4];
  byte maxtempday3[4];
  byte maxhumday3[2];
  byte maxpresday3[2];
  byte maxwindday3[4];

  byte checkcn[2];
} sendweather;

// struct SendCheck {
//   char type[4] = "SCK";
//   byte checkcn[2];
// } sendcheck;

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

String strjsonBuffer;
DynamicJsonBuffer jsonBuffer;

String lat = "11.8317";     // vĩ độ
String lon = "106.9883";    // kinh độ
int desiredHourCount = 24;  // Số lượng giờ bạn muốn lấy

// Biến lưu trữ thông tin về thời tiết cho mỗi ngày
double minTemp1;
double maxTemp1;
uint16_t maxHumidity1;
uint16_t maxPressure1;
double maxWindSpeed1;

double minTemp2;
double maxTemp2;
uint16_t maxHumidity2;
uint16_t maxPressure2;
double maxWindSpeed2;

double minTemp3;
double maxTemp3;
uint16_t maxHumidity3;
uint16_t maxPressure3;
double maxWindSpeed3;

// Hàm khởi tạo giá trị ban đầu cho các biến thời tiết
void initializeWeatherData() {
  minTemp1 = INFINITY;
  maxTemp1 = -INFINITY;
  maxHumidity1 = 0;
  maxPressure1 = 0;
  maxWindSpeed1 = 0;

  minTemp2 = INFINITY;
  maxTemp2 = -INFINITY;
  maxHumidity2 = 0;
  maxPressure2 = 0;
  maxWindSpeed2 = 0;

  minTemp3 = INFINITY;
  maxTemp3 = -INFINITY;
  maxHumidity3 = 0;
  maxPressure3 = 0;
  maxWindSpeed3 = 0;
}

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

String httpGETRequest(const char *serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    // Serial.print("HTTP Response code: ");
    // Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
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
  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(29);  //Set volume value. From 0 to 30

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Firebase.setReadTimeout(fbdo, 1000 * 60);
  Firebase.setwriteSizeLimit(fbdo, "tiny");

  e32ttl100.begin();

  // Khởi tạo giá trị ban đầu cho biến weatherDataFor3Days
  initializeWeatherData();
  taskmutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(SendopenWeatherTask, "SendopenWeatherTask", 15000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskREVData, "TaskREVData", 17000, NULL, 4, NULL, 1);
  xTaskCreatePinnedToCore(SendConnectTask, "SendConnectTask", 8000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskAlert, "TaskAlert", 8000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskDataFirebase, "TaskDataFirebase", 10000, NULL, 1, NULL, 0);
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

void SendopenWeatherTask(void *parameter) {
  (void)parameter;
  for (;;) {
    if (count < 3) {

      String serverPath = "http://api.openweathermap.org/data/2.5/forecast?lat=" + lat + "&lon=" + lon + "&exclude=hourly" + "&appid=" + openWeatherMapApiKey + "&cnt=" + String(desiredHourCount);
      strjsonBuffer = httpGETRequest(serverPath.c_str());
      JsonObject &myObject = jsonBuffer.parseObject(strjsonBuffer);

      // Test if parsing succeeds.
      if (!myObject.success()) {
        Serial.println("parseObject() failed");
        delay(500);
        return;
      }
      // Tạo một danh sách các ngày duy nhất trong dữ liệu JSON
      String uniqueDates[3];
      int uniqueDateCount = 0;

      for (int i = 0; i < desiredHourCount; ++i) {
        JsonObject &forecast = myObject["list"][i];

        // Lấy ngày từ dt_txt
        String dt_txt = forecast["dt_txt"];
        String currentDate = dt_txt.substring(0, 10);

        // Kiểm tra xem ngày đã tồn tại trong danh sách chưa
        bool found = false;
        for (int j = 0; j < uniqueDateCount; j++) {
          if (uniqueDates[j] == currentDate) {
            found = true;
            break;
          }
        }

        // Nếu ngày chưa tồn tại, thêm vào danh sách
        if (!found) {
          uniqueDates[uniqueDateCount++] = currentDate;
        }
      }

      // Khai báo biến đếm cho mỗi ngày
      int dayIndex = 0;

      // Lặp qua mỗi mục trong dữ liệu JSON và cập nhật thông tin thời tiết
      for (int i = 0; i < desiredHourCount; ++i) {
        JsonObject &forecast = myObject["list"][i];

        // Lấy ngày từ dt_txt
        String dt_txt = forecast["dt_txt"];
        String currentDate = dt_txt.substring(0, 10);

        // Nếu là một ngày mới, tăng chỉ số ngày
        if (i > 0 && currentDate != uniqueDates[dayIndex]) {
          dayIndex++;
        }

        // Lấy thông tin thời tiết từ dữ liệu JSON
        double tempMin = forecast["main"]["temp_min"];
        double tempMax = forecast["main"]["temp_max"];
        int humidityT = forecast["main"]["humidity"];
        int pressureT = forecast["main"]["pressure"];
        double windSpeedT = forecast["wind"]["speed"];

        // Cập nhật thông tin thời tiết cho mỗi ngày tương ứng
        switch (dayIndex) {
          case 0:
            if (tempMin < minTemp1) {
              minTemp1 = tempMin;
            }
            if (tempMax > maxTemp1) {
              maxTemp1 = tempMax;
            }
            if (humidityT > maxHumidity1) {
              maxHumidity1 = humidityT;
            }
            if (pressureT > maxPressure1) {
              maxPressure1 = pressureT;
            }
            if (windSpeedT > maxWindSpeed1) {
              maxWindSpeed1 = windSpeedT;
            }
            break;
          case 1:
            if (tempMin < minTemp2) {
              minTemp2 = tempMin;
            }
            if (tempMax > maxTemp2) {
              maxTemp2 = tempMax;
            }
            if (humidityT > maxHumidity2) {
              maxHumidity2 = humidityT;
            }
            if (pressureT > maxPressure2) {
              maxPressure2 = pressureT;
            }
            if (windSpeedT > maxWindSpeed2) {
              maxWindSpeed2 = windSpeedT;
            }
            // Tương tự cho ngày thứ 2
            break;
          case 2:
            // Tương tự cho ngày thứ 3
            if (tempMin < minTemp3) {
              minTemp3 = tempMin;
            }
            if (tempMax > maxTemp3) {
              maxTemp3 = tempMax;
            }
            if (humidityT > maxHumidity3) {
              maxHumidity3 = humidityT;
            }
            if (pressureT > maxPressure3) {
              maxPressure3 = pressureT;
            }
            if (windSpeedT > maxWindSpeed3) {
              maxWindSpeed3 = windSpeedT;
            }
            break;
          default:
            break;
        }
      }

      jsonBuffer.clear();  // Giải phóng bộ nhớ của ArduinoJSON

      // In ra thông tin thời tiết của từng ngày
      Serial.println("Ngày 1:");
      Serial.print("   Nhiệt độ tối thiểu: ");
      Serial.println(minTemp1);
      Serial.print("   Nhiệt độ tối đa: ");
      Serial.println(maxTemp1);
      Serial.print("   Độ ẩm tối đa: ");
      Serial.println(maxHumidity1);
      Serial.print("   Áp suất tối đa: ");
      Serial.println(maxPressure1);
      Serial.print("   Tốc độ gió tối đa: ");
      Serial.println(maxWindSpeed1);

      Serial.println("Ngày 2:");
      Serial.print("   Nhiệt độ tối thiểu: ");
      Serial.println(minTemp2);
      Serial.print("   Nhiệt độ tối đa: ");
      Serial.println(maxTemp2);
      Serial.print("   Độ ẩm tối đa: ");
      Serial.println(maxHumidity2);
      Serial.print("   Áp suất tối đa: ");
      Serial.println(maxPressure2);
      Serial.print("   Tốc độ gió tối đa: ");
      Serial.println(maxWindSpeed2);

      Serial.println("Ngày 3:");
      Serial.print("   Nhiệt độ tối thiểu: ");
      Serial.println(minTemp3);
      Serial.print("   Nhiệt độ tối đa: ");
      Serial.println(maxTemp3);
      Serial.print("   Độ ẩm tối đa: ");
      Serial.println(maxHumidity3);
      Serial.print("   Áp suất tối đa: ");
      Serial.println(maxPressure3);
      Serial.print("   Tốc độ gió tối đa: ");
      Serial.println(maxWindSpeed3);

      // Lưu trữ thông tin thời tiết vào struct sendweather
      strcpy(sendweather.type, "SWR");

      *(float *)(sendweather.mintempday1) = minTemp1;
      *(float *)(sendweather.maxtempday1) = maxTemp1;
      *(uint16_t *)(sendweather.maxhumday1) = maxHumidity1;
      *(uint16_t *)(sendweather.maxpresday1) = maxPressure1;
      *(float *)(sendweather.maxwindday1) = maxWindSpeed1;

      *(float *)(sendweather.mintempday2) = minTemp2;
      *(float *)(sendweather.maxtempday2) = maxTemp2;
      *(uint16_t *)(sendweather.maxhumday2) = maxHumidity2;
      *(uint16_t *)(sendweather.maxpresday2) = maxPressure2;
      *(float *)(sendweather.maxwindday2) = maxWindSpeed2;

      *(float *)(sendweather.mintempday3) = minTemp3;
      *(float *)(sendweather.maxtempday3) = maxTemp3;
      *(uint16_t *)(sendweather.maxhumday3) = maxHumidity3;
      *(uint16_t *)(sendweather.maxpresday3) = maxPressure3;
      *(float *)(sendweather.maxwindday3) = maxWindSpeed3;
      vTaskDelay(500);
      count++;
    }
    if (count == 3) {  // Nếu đã chạy 3 lần
      vTaskDelay(500);
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 1, 0xA, &sendweather, sizeof(SendWeather));
      Serial.println(rs.getResponseDescription());
      count++;  // Tăng biến đếm để không lặp lại điều kiện này nữa
    }
    if (count > 3 && runEvery(28800000, &previousMillis1)) {
      count = 0;  // Đặt lại biến đếm sau mỗi khoảng thời gian nhất định
    }
    vTaskDelay(500);
  }
}

void TaskDataFirebase(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    if (runEvery(30000, &previousMillis2)) {
      int co21 = 95;
      Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Temperature", tLVR);
      Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Humidity", hLVR);
      Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/CO2", co21);
      Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/TVOC", TVOCLVR);
      Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Flame", FLMLVR);
      Firebase.setFloat(fbdo, "/DataSensor/LivingRoom/Smoke", SMKLVR);
      if ((FLMLVR > 0) && (FLMLVR < 50)) {
        Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FlameSensor", "Fire!!!");

      } else if (FLMLVR > 50) {
        Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FlameSensor", "Safe");
      }
      if (SMKLVR > 700) {
        Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_SmokeSensor", "Smoke detected!");
      } else if (SMKLVR < 700) {
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
      if (ct1 == 1) {
        Serial.println("1");
        Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_Light", "ON");
      }
      if (ct1 == 0) {
        Serial.println("0");
        Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_Light", "OFF");
      }
      if (ct2 == 1) {
        Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FAN", "ON");
      }
      if (ct2 == 0) {
        Firebase.setString(fbdo, "/DataSensor/LivingRoom/Status_FAN", "OFF");
      }
      if (ct3 == 1) {
        Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump1", "ON");
      }
      if (ct3 == 0) {
        Firebase.setString(fbdo, "/DataSensor/Farm/Status_Pump1", "OFF");
      }
    }
    vTaskDelay(350);
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
      *(uint16_t *)(sendweather.checkcn) = 1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 1, 0xA, &sendweather, sizeof(SendWeather));
      Serial.println(rs.getResponseDescription());
      vTaskDelay(350);
    }
    // xSemaphoreGive(taskmutex);
    vTaskDelay(500);
  }
}