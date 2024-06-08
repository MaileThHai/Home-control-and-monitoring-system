#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#define FPSerial Serial1

DFRobotDFPlayerMini myDFPlayer;

float tLVR = 39;
uint16_t CO2LVR = 1600;
uint16_t TVOCLVR = 0;
uint16_t FLMLVR = 2;
uint16_t SMKLVR = 800;

int checktemp = 0;
int checkco2 = 0;
int checkfire = 0;
int checksmoke = 0;

int sttfire = 0;
int sttsmoke = 0;
int sttco2 = 0;
int stttemp = 0;
unsigned long previousMillis3 = 0;
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
  FPSerial.begin(9600, SERIAL_8N1, 27, 26);

  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);
    }
  }

  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(29);

  xTaskCreatePinnedToCore(TaskAlert, "TaskAlert", 4096, NULL, 1, NULL, 1);
}

void loop() {}

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
    if (tLVR > 35) {
      checktemp = 1;
    } else if (tLVR < 35) {
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
      } else if ((checktemp == 1) && (stttemp == 0)){
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
    vTaskDelay(100);
  }
}

boolean runEvery(unsigned long interval, unsigned long *previousMillis) {
  unsigned long currentMillis = millis();
  if (currentMillis - *previousMillis >= interval) {
    *previousMillis = currentMillis;
    return true;
  }
  return false;
}