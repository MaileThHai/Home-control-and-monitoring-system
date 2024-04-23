#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <RTClib.h>
#include "main.h"

/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

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

bool RLYSTATE[4] = {0};

// Define task handle
SemaphoreHandle_t uiMutex;

/* ====================
 * Function Prototypes 
 * ==================== */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void display_init(void);
void GUI_init(void);
void DS1307_task(void *pvParameters);
void SetTime_task(void *pvParameter);


void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */
  Wire.begin(8, 9);     // SDA_PIN = 8, SCL_PIN = 9
  Serial.print("Setup running on core: ");
  Serial.println(xPortGetCoreID());

  /* Initialize */
  display_init();
  GUI_init();
  ui_init();


  /* Create task scheduler*/
  uiMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(LVGL_task, "LVGL_task", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(DS1307_task, "DS1307_task", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(SetTime_task, "SetTime_task", 4096, NULL, 1, NULL, 1);
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
    data->point.y = touchY + 20;

    // Serial.print("Data x ");
    // Serial.println(touchX);

    // Serial.print("Data y ");
    // Serial.println(touchY);
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
    vTaskDelay(10 / portTICK_PERIOD_MS);
    UI_TakeMutex();
    lv_timer_handler(); /* let the GUI do its work */
    UI_GiveMutex();
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
    vTaskDelay(1000);
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
    vTaskDelay(1000);
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
  }
}