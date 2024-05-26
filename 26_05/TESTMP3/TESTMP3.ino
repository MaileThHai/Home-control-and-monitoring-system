#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#define FPSerial Serial1

DFRobotDFPlayerMini myDFPlayer;

void setup() {
  FPSerial.begin(9600, SERIAL_8N1, 27, 26);

  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(27);  //Set volume value. From 0 to 30
}

void loop() {
  myDFPlayer.playMp3Folder(1);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(15000);
  myDFPlayer.playMp3Folder(2);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(15000);
  myDFPlayer.playMp3Folder(3);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(15000);
  myDFPlayer.playMp3Folder(2);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(15000);
  myDFPlayer.playMp3Folder(5);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(15000);
  myDFPlayer.playMp3Folder(2);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(15000);
  myDFPlayer.playMp3Folder(7);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(15000);
  myDFPlayer.playMp3Folder(2);  //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(15000);
}
