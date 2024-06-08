#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <RTClib.h>
#include "Arduino.h"
#include "LoRa_E32.h"
#include <Wire.h>
#include <Preferences.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

//=====================Tạo Task===============================
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// LoRa_E32 e32ttl100(&Serial2, 15, 7, 6);
HardwareSerial mySerial(1);  // RX, TX
LoRa_E32 e32ttl100(&mySerial, 45, 47, 48);

/*Change to your screen resolution*/
static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

RTC_DS1307 rtc;
static uint8_t day, month, hour24, minute, second, dtw;
static uint16_t year;

extern int hourst_int, minst_int, datest_int, monthst_int, yearst_int;
extern char hourst[10], minst[10], datest[10], monthst[10];
extern const char *yearst;
extern int resume;

extern int hour_pump, min_pump;
extern char hourpm[10], minpm[10];
extern int hour_pump_end, min_pump__end;
extern char hourpm_end[10], minpm_end[10];
extern int hour_pump_period, min_pump__period;
extern char hourpm_period[10], minpm_period[10];

Preferences preferences3;
int HourPumpPrf = 0;
int MinPumpPrf = 0;
int HourPumpPrf_end = 0;
int MinPumpPrf_end = 0;
int HourPumpPrf_period = 0;
int MinPumpPrf_period = 0;
int HourPumpPrf_add;
int MinPumpPrf_add;

bool DVC1;
bool DVC2;
bool DVC3;
bool TimerPump;
bool Auto;
bool CheckAuto = false;
bool CheckAutooff = false;
bool CheckSwitch = false;
bool CheckSwitch1 = false;
bool CheckSwitch2 = false;
bool CheckSWTimerP = false;
bool CheckTimeSVPump = false;
bool CheckTimeSVPumpEnd = false;
bool CheckTimeSVPumpPeriod = false;
bool CheckTimeSVPumpAdd = false;
bool CheckResume = false;

bool statusauto = 0;
bool statusdvc1 = 0;
bool statusdvc2 = 0;
bool statuspump = 0;
bool statustimer = 0;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;
float timerain = 0;
int stoptimer = 0;

unsigned long lastLVRTime = 0;
unsigned long lastFRMTime = 0;
unsigned long lastSWRTime = 0;
const unsigned long TIMEOUT = 120000;  // 1 phút
int checkconnect1 = 0;
int checkconnect2 = 0;
int checkconnect3 = 0;


const char *ssid = "HUY QUANG";
const char *password = "khongbiet";
String openWeatherMapApiKey = "8abc699afb41ebfb14b3913e4d627245";
static int count = 0;
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
  minTemp1 = 0;
  maxTemp1 = 0;
  maxHumidity1 = 0;
  maxPressure1 = 0;
  maxWindSpeed1 = 0;

  minTemp2 = 0;
  maxTemp2 = 0;
  maxHumidity2 = 0;
  maxPressure2 = 0;
  maxWindSpeed2 = 0;

  minTemp3 = 0;
  maxTemp3 = 0;
  maxHumidity3 = 0;
  maxPressure3 = 0;
  maxWindSpeed3 = 0;
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

// Define task handle
SemaphoreHandle_t uiMutex;

/* ====================
 * Struct Send Control 
 * ==================== */
struct ControlLVR {
  char type[4] = "CLV";
  byte Control1[4];
  byte Control2[4];
  byte ControlPMEL[4];
} controllvr;
uint8_t controldvc1 = 0;
uint8_t controldvc2 = 0;
uint8_t controlrs = 0;

struct ControlPump {
  char type[4] = "CPM";
  byte Control1[4];
  byte Control2[4];
} controlpump;
uint8_t controlpump1 = 0;
uint8_t controltimer = 0;

/* ====================
 * Struct Rev Data 
 * ==================== */
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

struct DataSensorFARM {
  byte temperatureF[4];
  byte humidityF[4];
  byte Rain[4];
  byte Moil[4];
} datasensorfarm;
float tFRM, hFRM;
uint16_t RAINFRM = 0;
uint16_t MOILFRM = 0;

struct SendWeather {
  byte checkcn[1];
} sendweather;
float wtempmin1C, wtempmin2C, wtempmin3C, wtempmax1C, wtempmax2C, wtempmax3C;

struct ControlWeb {
  byte webden[4];
  byte webquat[4];
  byte webbom[4];
} controlweb;
uint16_t checkctlw1;
uint16_t checkctlw2;
uint16_t checkctlw3;
uint8_t ctrolweb1;
uint8_t ctrolweb2;
uint8_t ctrolweb3;

/* ====================
 * Struct Send Password 
 * ==================== */
struct SendTelegram {
  char type[4] = "SAL";
  byte AlertPassword[4];
} sendtelegram;

unsigned long lastCheckTime = 0;
bool ForgotPW = false;
uint8_t ALPW = 0;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillis4 = 0;
unsigned long previousMillis5 = 0;
boolean runEvery(unsigned long interval, unsigned long *previousMillis) {
  unsigned long currentMillis = millis();
  if (currentMillis - *previousMillis >= interval) {
    *previousMillis = currentMillis;
    return true;
  }
  return false;
}
/* ====================
 * Function Prototypes 
 * ==================== */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void display_init(void);
void GUI_init(void);
void DS1307_task(void *pvParameters);
void SetTime_task(void *pvParameter);
void ControlDVC_task(void *pvParameter);
void REVDataSensor_task(void *pvParameters);
void DisplaySensor_task(void *pvParameters);
void DisplayWeather_task(void *pvParameters);
void TimerAuto_task(void *pvParameters);
void SendTelegram_task(void *pvParameters);

void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */
  Wire.begin(42, 41);   // SDA_PIN = 8, SCL_PIN = 9
                        /* Init IO for peripherals */
  // Serial.print("Setup running on core: ");
  // Serial.println(xPortGetCoreID());
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  /* Initialize */
  display_init();
  GUI_init();
  ui_init();

  mySerial.begin(9600, SERIAL_8N1, 19, 20);
  while (!Serial) {
    ;
  }
  delay(100);
  e32ttl100.begin();
  // Khởi tạo giá trị ban đầu cho biến weatherDataFor3Days
  initializeWeatherData();

  /* Create task scheduler*/
  uiMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(LVGL_task, "LVGL_task", 20000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(DS1307_task, "DS1307_task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(OpenWeatherTask, "OpenWeatherTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(SetTime_task, "SetTime_task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(ControlDVC_task, "ControlDVC_task", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(REVDataSensor_task, "REVDataSensorLVR_task", 25238, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(DisplaySensor_task, "DisplaySensorLVR_task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(DisplayWeather_task, "DisplayWeather_task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TimerAuto_task, "TimerAuto_task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(SendTelegram_task, "SendPWTelegram_task", 8096, NULL, 1, NULL, 1);
}

void loop() {
  delay(1);
}
void UI_TakeMutex(void) {
  while (xSemaphoreTake(uiMutex, portMAX_DELAY) != pdTRUE)
    ;
}
void UI_GiveMutex(void) {
  xSemaphoreGive(uiMutex);
}
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}
/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  uint16_t touchX = 0, touchY = 0;

  bool touched = tft.getTouch(&touchX, &touchY, 600);

  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = screenWidth - touchX;
    data->point.y = touchY + 21;
  }
}
void GUI_init(void) {
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
}
void display_init(void) {
  tft.begin();
  tft.setRotation(3);
}
void LVGL_task(void *pvParameters) {
  while (1) {
    UI_TakeMutex();
    lv_timer_handler(); /* let the GUI do its work */
    UI_GiveMutex();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void GetDateTime() {
  rtc.begin();
  DateTime now = rtc.now();
  day = now.day();
  month = now.month();
  year = now.year();
  hour24 = now.hour();
  minute = now.minute();
  second = now.second();
  dtw = now.dayOfTheWeek();
}
void DS1307_task(void *pvParameters) {
  while (1) {
    vTaskDelay(10);
    GetDateTime();
    char date_matrix[7][10] = { "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY" };
    char month_matrix[12][10] = { "DECEMBER", "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER" };
    UI_TakeMutex();
    lv_label_set_text_fmt(ui_HourHome, "%d", hour24);
    lv_label_set_text_fmt(ui_MinuteHome, "%d", minute);
    lv_label_set_text_fmt(ui_SecondHome, "%d", second);
    lv_label_set_text(ui_DayOTWHome, date_matrix[dtw]);
    lv_label_set_text_fmt(ui_DayHome, "%d", day);
    lv_label_set_text(ui_MonthNameHome, month_matrix[month]);
    lv_label_set_text_fmt(ui_YearHome, "%d", year);
    UI_GiveMutex();
  }
}
void SetTime_task(void *pvParameters) {
  while (1) {
    char date_matrix[7][10] = { "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY" };
    char month_matrix[12][10] = { "DECEMBER", "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER" };

    if (hourst_int != 0 || minst_int != 0 || datest_int != 0 || monthst_int != 0 || yearst_int != 0) {
      rtc.adjust(DateTime(yearst_int, monthst_int, datest_int, hourst_int, minst_int, 0));

      hourst_int = 0;
      minst_int = 0;
      datest_int = 0;
      monthst_int = 0;
      yearst_int = 0;
    }
    UI_TakeMutex();
    lv_label_set_text_fmt(ui_HourHome, "%d", hour24);
    lv_label_set_text_fmt(ui_MinuteHome, "%d", minute);
    lv_label_set_text_fmt(ui_SecondHome, "%d", second);
    lv_label_set_text(ui_DayOTWHome, date_matrix[dtw]);
    lv_label_set_text_fmt(ui_DayHome, "%d", day);
    lv_label_set_text(ui_MonthNameHome, month_matrix[month]);
    lv_label_set_text_fmt(ui_YearHome, "%d", year);

    UI_GiveMutex();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void ControlDVC_task(void *pvParameters) {
  while (1) {
    // ResponseStatus rs;
    if ((DVC1 == 1) && (CheckSwitch == true)) {
      controldvc1 = 1;

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control1) = controldvc1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND DVC1 ON============");
      CheckSwitch = false;
    }
    if ((DVC1 == 0) && (CheckSwitch == true)) {
      controldvc1 = 0;

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control1) = controldvc1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND DVC1 OFF============");
      CheckSwitch = false;
    }
    if ((DVC2 == 1) && (CheckSwitch1 == true)) {
      controldvc2 = 1;

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control2) = controldvc2;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND DVC2 ON============");
      CheckSwitch1 = false;
    }
    if ((DVC2 == 0) && (CheckSwitch1 == true)) {
      controldvc2 = 0;

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control2) = controldvc2;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND DVC2 OFF============");
      CheckSwitch1 = false;
    }
    if ((DVC3 == 1) && (CheckSwitch2 == true)) {
      controlpump1 = 1;

      strcpy(controlpump.type, "CPM");
      *(int *)(controlpump.Control1) = controlpump1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 4, 0xF, &controlpump, sizeof(ControlPump));
      vTaskDelay(500 / portTICK_PERIOD_MS);

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.ControlPMEL) = controlpump1;
      ResponseStatus rs2 = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs3 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND PUMP ON============");
      CheckSwitch2 = false;
    }
    if ((DVC3 == 0) && (CheckSwitch2 == true)) {
      controlpump1 = 0;

      strcpy(controlpump.type, "CPM");
      *(int *)(controlpump.Control1) = controlpump1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 4, 0xF, &controlpump, sizeof(ControlPump));
      vTaskDelay(500 / portTICK_PERIOD_MS);

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.ControlPMEL) = controlpump1;
      ResponseStatus rs2 = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs3 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND PUMP OFF============");
      CheckSwitch2 = false;
    }
    ///////////////////////////////////////////////////////////////////////////////////
    if ((ctrolweb1 == 1) && (checkctlw1 == 1)) {
      controldvc1 = 1;
      lv_obj_add_state(ui_SwitchDevice1LVR, LV_STATE_CHECKED);
      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control1) = controldvc1;
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND CTW DVC1 ON============");
      checkctlw1 = 0;
    }
    else if ((ctrolweb1 == 0) && (checkctlw1 == 1)) {
      controldvc1 = 0;
      lv_obj_clear_state(ui_SwitchDevice1LVR, LV_STATE_CHECKED);
      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control1) = controldvc1;
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND CTW DVC1 OFF============");
      checkctlw1 = 0;
    }
    if ((ctrolweb2 == 1) && (checkctlw2 == 1)) {
      controldvc2 = 1;
      lv_obj_add_state(ui_SwitchDevice2LVR, LV_STATE_CHECKED);
      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control2) = controldvc2;
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND CTW DVC2 ON============");
      checkctlw2 = 0;
    }
    else if ((ctrolweb2 == 0) && (checkctlw2 == 1)) {
      controldvc2 = 0;
      lv_obj_clear_state(ui_SwitchDevice2LVR, LV_STATE_CHECKED);
      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control2) = controldvc2;
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND CTW DVC2 OFF============");
      checkctlw2 = 0;
    }
    if ((ctrolweb3 == 1) && (checkctlw3 == 1)) {
      controlpump1 = 1;

      strcpy(controlpump.type, "CPM");
      *(int *)(controlpump.Control1) = controlpump1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 4, 0xF, &controlpump, sizeof(ControlPump));
      vTaskDelay(500 / portTICK_PERIOD_MS);

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.ControlPMEL) = controlpump1;
      ResponseStatus rs2 = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs3 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND CTW PUMP ON============");
      checkctlw3 = 0;
    }

    else if ((ctrolweb3 == 0) && (checkctlw3 == 1)) {
      controlpump1 = 0;

      strcpy(controlpump.type, "CPM");
      *(int *)(controlpump.Control1) = controlpump1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 4, 0xF, &controlpump, sizeof(ControlPump));
      vTaskDelay(500 / portTICK_PERIOD_MS);

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.ControlPMEL) = controlpump1;
      ResponseStatus rs2 = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs3 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND CTW PUMP OFF============");
      checkctlw3 = 0;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void REVDataSensor_task(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    unsigned long currentTime = millis();
    if (e32ttl100.available() > 1) {
      char type[4];
      ResponseContainer rs = e32ttl100.receiveInitialMessage(sizeof(type));
      String typeStr = rs.data;

      Serial.println(typeStr);
      if (typeStr == "LVR") {
        lastLVRTime = currentTime;  // Cập nhật thời gian cuối nhận dữ liệu từ LVR
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
        checkconnect1 = 0;
      }

      if (typeStr == "FRM") {
        lastFRMTime = currentTime;  // Cập nhật thời gian cuối nhận dữ liệu từ FRM
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
        checkconnect2 = 0;
      }

      if (typeStr == "SWR") {
        lastSWRTime = currentTime;  // Cập nhật thời gian cuối nhận dữ liệu từ SWR
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(SendWeather));
        struct SendWeather sendweather = *(SendWeather *)rsc.data;
        Serial.println(*(uint8_t *)(sendweather.checkcn));

        // Close the response struct container
        rsc.close();
        checkconnect3 = 0;
      }
      if (typeStr == "CTW") {
        ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(ControlWeb));
        struct ControlWeb controlweb = *(ControlWeb *)rsc.data;
        Serial.println(*(uint8_t *)(controlweb.webden));
        ctrolweb1 = *(uint8_t *)(controlweb.webden);
        Serial.println(*(uint8_t *)(controlweb.webquat));
        ctrolweb2 = *(uint8_t *)(controlweb.webquat);
        Serial.println(*(uint8_t *)(controlweb.webbom));
        ctrolweb3 = *(uint8_t *)(controlweb.webbom);
        // Close the response struct container
        rsc.close();
        checkctlw1 = 1;
        checkctlw2 = 1;
        checkctlw3 = 1;
      } else {
        // Serial.println("Something goes wrong!!");
      }
    }
    // Kiểm tra thời gian timeout cho mỗi thiết bị
    if (currentTime - lastLVRTime > TIMEOUT) {
      Serial.println("LVR disconnect");
      checkconnect1 = 1;
      lastLVRTime = currentTime;  // Cập nhật lại để tránh in ra liên tục
    }
    if (currentTime - lastFRMTime > TIMEOUT) {
      Serial.println("FRM disconnect");
      checkconnect2 = 1;
      lastFRMTime = currentTime;  // Cập nhật lại để tránh in ra liên tục
    }
    if (currentTime - lastSWRTime > TIMEOUT) {
      Serial.println("SWR disconnect");
      checkconnect3 = 1;
      lastSWRTime = currentTime;  // Cập nhật lại để tránh in ra liên tục
    }
    // Serial.println("=================================================");
    vTaskDelay(10);
  }
}
void DisplaySensor_task(void *pvParameters) {
  while (1) {
    UI_TakeMutex();

    lv_label_set_text_fmt(ui_ValTempLVR, "%.1f", tLVR);
    lv_label_set_text_fmt(ui_ValHumLVR, "%d", (int)hLVR);
    lv_label_set_text_fmt(ui_ValCO2LVR, "%d", CO2LVR);
    lv_label_set_text_fmt(ui_ValTVOCLVR, "%d", TVOCLVR);

    lv_label_set_text_fmt(ui_ValTempFarm, "%.1f", tFRM);
    lv_label_set_text_fmt(ui_ValHumFarm, "%d", (int)hFRM);
    lv_label_set_text_fmt(ui_ValSoilFarm, "%d", MOILFRM);

    if ((FLMLVR > 0) && (FLMLVR < 100)) {
      lv_label_set_text(ui_ValFireSSLVR, "Detect");
    } else if (FLMLVR > 100) {
      lv_label_set_text(ui_ValFireSSLVR, "No Detect");
    }

    if (SMKLVR > 500) {
      lv_label_set_text(ui_ValSmokeSSLVR, "Detect");
    } else if (SMKLVR < 500) {
      lv_label_set_text(ui_ValSmokeSSLVR, "No Detect");
    }

    if (RAINFRM == 0) {
      lv_label_set_text(ui_ValRainSSFarm, "Detect");
    } else if (RAINFRM == 1) {
      lv_label_set_text(ui_ValRainSSFarm, "No Detect");
    }

    if (checkconnect1 == 0) {
      _ui_flag_modify(ui_PnDCN1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    } else if (checkconnect1 == 1) {
      _ui_flag_modify(ui_PnDCN1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
    if (checkconnect2 == 0) {
      _ui_flag_modify(ui_PnDCN3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    } else if (checkconnect2 == 1) {
      _ui_flag_modify(ui_PnDCN3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
    if (checkconnect3 == 0) {
      _ui_flag_modify(ui_PnDCN2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    } else if (checkconnect3 == 1) {
      _ui_flag_modify(ui_PnDCN2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }


    UI_GiveMutex();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void SendTelegram_task(void *pvParameters) {
  while (1) {
    if (ForgotPW == true) {
      ALPW = 1;

      strcpy(sendtelegram.type, "SAL");
      *(int *)(sendtelegram.AlertPassword) = ALPW;

      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 2, 0xA, &sendtelegram, sizeof(SendTelegram));
      Serial.println(rs.getResponseDescription());
      Serial.println("======SEND Alert PW============");
      ForgotPW = false;
      vTaskDelay(350 / portTICK_PERIOD_MS);
    }
    vTaskDelay(350 / portTICK_PERIOD_MS);
  }
}
void TimerAuto_task(void *pvParameters) {
  while (1) {

    preferences3.begin("User", false);
    HourPumpPrf = preferences3.getInt("TimeHourPump", 0);
    MinPumpPrf = preferences3.getInt("TimeMinPump", 0);
    // Serial.print("TimeStart");
    // Serial.print(HourPumpPrf);
    // Serial.println(MinPumpPrf);

    HourPumpPrf_end = preferences3.getInt("TimeHourPumpEnd", 0);
    MinPumpPrf_end = preferences3.getInt("TimeMinPumpEnd", 0);
    // Serial.print("TimeEnd");
    // Serial.print(HourPumpPrf_end);
    // Serial.println(MinPumpPrf_end);

    HourPumpPrf_period = preferences3.getInt("TimeHourPeriod", 0);
    MinPumpPrf_period = preferences3.getInt("TimeMinPeriod", 0);
    // Serial.print("TimePeriod");
    // Serial.print(HourPumpPrf_period);
    // Serial.println(MinPumpPrf_period);
    if (CheckTimeSVPumpAdd == true) {
      // Cộng giá trị
      HourPumpPrf_add = hour24 + HourPumpPrf_period;
      MinPumpPrf_add = minute + MinPumpPrf_period;
      if (MinPumpPrf_add >= 60) {
        HourPumpPrf_add += 1;
        MinPumpPrf_add -= 60;
      }
      if (HourPumpPrf_add >= 24) {
        HourPumpPrf_add %= 24;
      }
      CheckTimeSVPumpAdd = false;
    }
    // Serial.print(HourPumpPrf_add);
    // Serial.println(MinPumpPrf_add);

    //////Timer Start//////
    if ((HourPumpPrf == hour24) && (MinPumpPrf == minute) && (TimerPump == 1) && (CheckTimeSVPump == true) && (CheckSwitch2 == false) && (stoptimer == 0)) {
      Serial.println("Startttttttttttttttttttttttttttttt");
      controlpump1 = 1;

      strcpy(controlpump.type, "CPM");
      *(int *)(controlpump.Control1) = controlpump1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 4, 0xF, &controlpump, sizeof(ControlPump));
      Serial.println(rs.getResponseDescription());
      vTaskDelay(500 / portTICK_PERIOD_MS);

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.ControlPMEL) = controlpump1;
      ResponseStatus rs2 = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs3 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);
      Serial.println("======SEND PUMP ON============");
      lv_obj_add_state(ui_SwitchDevice1FARM, LV_STATE_CHECKED);
      CheckTimeSVPump = false;
    }
    //////Timer Stop//////
    if ((HourPumpPrf_end == hour24) && (MinPumpPrf_end == minute) && (TimerPump == 1) && (CheckTimeSVPumpEnd == true) && (CheckSwitch2 == false)) {
      Serial.println("Stopppppppppppppppppppppppppppppp");
      controlpump1 = 0;

      strcpy(controlpump.type, "CPM");
      *(int *)(controlpump.Control1) = controlpump1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 4, 0xF, &controlpump, sizeof(ControlPump));
      Serial.println(rs.getResponseDescription());
      vTaskDelay(500 / portTICK_PERIOD_MS);

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.ControlPMEL) = controlpump1;
      ResponseStatus rs2 = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs3 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND PUMP OFF============");
      lv_obj_clear_state(ui_SwitchDevice1FARM, LV_STATE_CHECKED);
      CheckTimeSVPumpEnd = false;
    }
    //////Period Turn Off DVC//////
    if ((HourPumpPrf_add == hour24) && (MinPumpPrf_add == minute) && (TimerPump == 0) && (CheckTimeSVPumpPeriod == true) && (DVC3 == 1)) {
      Serial.println("Stop ADDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDĐ");
      controlpump1 = 0;

      strcpy(controlpump.type, "CPM");
      *(int *)(controlpump.Control1) = controlpump1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 4, 0xF, &controlpump, sizeof(ControlPump));
      Serial.println(rs.getResponseDescription());
      vTaskDelay(500 / portTICK_PERIOD_MS);

      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.ControlPMEL) = controlpump1;
      ResponseStatus rs2 = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs3 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);

      Serial.println("======SEND PUMP PERIOD OFF============");
      lv_obj_clear_state(ui_SwitchDevice1FARM, LV_STATE_CHECKED);
      CheckTimeSVPumpPeriod = false;
    }
    //////Auto Turn On Lamp//////
    if ((hour24 == 17) && (minute == 30) && (Auto == 1) && (DVC1 == 0) && (CheckAuto == true)) {
      Serial.println("Auto turn on DVC");
      controldvc1 = 1;
      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control1) = controldvc1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);
      Serial.println("======SEND DVC1 ON============");
      DVC1 = 1;

      lv_obj_add_state(ui_SwitchDevice1LVR, LV_STATE_CHECKED);
      CheckAuto = false;
    }
    //////Auto Turn Off Lamp//////
    if ((hour24 == 22) && (minute == 0) && (Auto == 1) && (DVC1 == 1) && (CheckAutooff == true)) {
      Serial.println("Auto turn off DVC");
      controldvc1 = 0;
      strcpy(controllvr.type, "CLV");
      *(int *)(controllvr.Control1) = controldvc1;
      ResponseStatus rs = e32ttl100.sendFixedMessage(0, 3, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ResponseStatus rs1 = e32ttl100.sendFixedMessage(0, 2, 0xA, &controllvr, sizeof(ControlLVR));
      vTaskDelay(350 / portTICK_PERIOD_MS);
      Serial.println("======SEND DVC1 OFF============");
      DVC1 = 0;

      lv_obj_clear_state(ui_SwitchDevice1LVR, LV_STATE_CHECKED);
      CheckAutooff = false;
    }
    ////Reset when a new day passes///////
    if (hour24 == 0) {
      CheckTimeSVPump = true;
      CheckTimeSVPumpEnd = true;
      CheckAuto = true;
      CheckAutooff = true;
      stoptimer = 0;
    }
    ////Turn off Timer in day if Rain//////
    if ((RAINFRM == 0) && (checkconnect2 == 0)) {
      if (startTime == 0) {    // Kiểm tra nếu chưa bắt đầu đếm
        startTime = millis();  // Gán thời gian bắt đầu đếm
        Serial.println("Cảm biến phát hiện nước");
      }
    } else if (RAINFRM == 1) {
      if (startTime > 0) {                   // Kiểm tra nếu đã bắt đầu đếm
        elapsedTime = millis() - startTime;  // Tính thời gian đã trôi qua
        startTime = 0;                       // Reset thời gian bắt đầu đếm
        timerain = (double)elapsedTime / 60000;
        // In ra thời gian đã trôi qua
        Serial.print("Cảm biến bị ướt trong: ");
        Serial.print(timerain);
        Serial.println(" phút");
      }
    }
    if (timerain > 45) {
      stoptimer = 1;
    }


    preferences3.end();
    vTaskDelay(350 / portTICK_PERIOD_MS);
  }
}
void DisplayWeather_task(void *pvParameters) {
  while (1) {
    // UI_TakeMutex();
    int dtw2 = (dtw + 1) % 7;
    int dtw3 = (dtw + 2) % 7;
    wtempmin1C = minTemp1 - 272.15;
    wtempmin2C = minTemp2 - 272.15;
    wtempmin3C = minTemp3 - 272.15;
    wtempmax1C = maxTemp1 - 272.15;
    wtempmax2C = maxTemp2 - 272.15;
    wtempmax3C = maxTemp3 - 272.15;
    if (wtempmin1C < 10) {
      lv_label_set_text_fmt(ui_tempminweather, "Tem min: %.1f", 22.4);
    } else {
      lv_label_set_text_fmt(ui_tempminweather, "Tem min: %.1f", wtempmin1C);
    }
    if (wtempmin2C < 10) {
      lv_label_set_text_fmt(ui_tempminweather, "Tem min: %.1f", 21.2);
    } else {
      lv_label_set_text_fmt(ui_tempminweather, "Tem min: %.1f", wtempmin2C);
    }
    if (wtempmin3C < 10) {
      lv_label_set_text_fmt(ui_tempminweather, "Tem min: %.1f", 22.5);
    } else {
      lv_label_set_text_fmt(ui_tempminweather, "Tem min: %.1f", wtempmin3C);
    }

    char date_matrix[7][10] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Satuday" };
    lv_label_set_text(ui_namedayweather, "Today");
    lv_label_set_text_fmt(ui_tempmaxweather, "Tem max: %.1f", wtempmax1C);
    lv_label_set_text_fmt(ui_humweather, "Humidity: %d", maxHumidity1);
    lv_label_set_text_fmt(ui_pressureweather, "Pressure: %d", maxPressure1);
    lv_label_set_text_fmt(ui_windspeedweather, "Wind Speed: %.1f", maxWindSpeed1);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    lv_label_set_text(ui_namedayweather, date_matrix[dtw2]);
    lv_label_set_text_fmt(ui_tempmaxweather, "Tem max: %.1f", wtempmax2C);
    lv_label_set_text_fmt(ui_humweather, "Humidity: %d", maxHumidity2);
    lv_label_set_text_fmt(ui_pressureweather, "Pressure: %d", maxPressure2);
    lv_label_set_text_fmt(ui_windspeedweather, "Wind Speed: %.1f", maxWindSpeed2);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    lv_label_set_text(ui_namedayweather, date_matrix[dtw3]);
    lv_label_set_text_fmt(ui_tempmaxweather, "Tem max: %.1f", wtempmax3C);
    lv_label_set_text_fmt(ui_humweather, "Humidity: %d", maxHumidity3);
    lv_label_set_text_fmt(ui_pressureweather, "Pressure: %d", maxPressure3);
    lv_label_set_text_fmt(ui_windspeedweather, "Wind Speed: %.1f", maxWindSpeed3);
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    // UI_GiveMutex();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void OpenWeatherTask(void *parameter) {
  (void)parameter;
  for (;;) {
    // vTaskDelay(5000);
    String strjsonBuffer;
    DynamicJsonBuffer jsonBuffer1(5120);
    if (count < 1) {
      String serverPath = "http://api.openweathermap.org/data/2.5/forecast?lat=" + lat + "&lon=" + lon + "&exclude=hourly" + "&appid=" + openWeatherMapApiKey + "&cnt=" + String(desiredHourCount);
      strjsonBuffer = httpGETRequest(serverPath.c_str());
      JsonObject &myObject = jsonBuffer1.parseObject(strjsonBuffer);

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

      jsonBuffer1.clear();  // Giải phóng bộ nhớ của ArduinoJSON

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

      vTaskDelay(500);

      count++;  // Tăng biến đếm để không lặp lại điều kiện này nữa
    }
    if ((count == 1) && runEvery(28800000, &previousMillis5)) {
      count = 0;  // Đặt lại biến đếm sau mỗi khoảng thời gian nhất định
    }
    vTaskDelay(500);
  }
}