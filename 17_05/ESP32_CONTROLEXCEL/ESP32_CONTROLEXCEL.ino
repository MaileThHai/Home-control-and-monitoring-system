#include "WiFi.h"
#include <HTTPClient.h>
//----------------------------------------

//----------------------------------------SSID and PASSWORD of your WiFi network.
const char* ssid = "HUY QUANG";      //--> Your wifi name
const char* password = "khongbiet";  //--> Your wifi password
//----------------------------------------

// Google script Web_App_URL.
String Web_App_URL = "https://script.google.com/macros/s/AKfycbw03K3sj-Mc9EMVFMTxFLtdgOBjXnlEUAYZAf_7jK9s2POrI8B9wznknU6m9sdXwjKE/exec";


//________________________________________________________________________________VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println();
  delay(1000);
  //----------------------------------------Set Wifi to STA mode
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");
  //----------------------------------------

  //----------------------------------------Connect to Wi-Fi (STA).
  Serial.println();
  Serial.println("------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int connecting_process_timed_out = 20;  //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
    if (connecting_process_timed_out > 0) connecting_process_timed_out--;
    if (connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("------------");
  //::::::::::::::::::
  //----------------------------------------

  delay(100);

  Serial.println();
  Serial.println();
  delay(1000);


  delay(2000);
}
//________________________________________________________________________________

//________________________________________________________________________________VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:

  // Calling the "Getting_DHT11_Sensor_Data()" subroutine.


  // Calling the "Read_Switches_State()" subroutine.

  //----------------------------------------Conditions that are executed when WiFi is connected.
  // This condition is the condition for sending or writing data to Google Sheets.
  if (WiFi.status() == WL_CONNECTED) {
    // Create a URL for sending or writing data to Google Sheets.
    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL += "&temp=" + String(33.7);
    Send_Data_URL += "&hum=" + String(46);
    Send_Data_URL += "&co2=" + String(87);
    Send_Data_URL += "&tvoc=" + String(12);
    Send_Data_URL += "&fire=" + String("NoDetect");
    Send_Data_URL += "&smoke=" + String("NoDetect");
    Send_Data_URL += "&dvc1=" + String("Off");
    Send_Data_URL += "&dvc2=" + String("Off");
    Send_Data_URL += "&tempf=" + String(35.7);
    Send_Data_URL += "&humf=" + String(67);
    Send_Data_URL += "&soil=" + String(87);
    Send_Data_URL += "&rain=" + String("NoDetect");
    Send_Data_URL += "&pump=" + String("Off");




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
  //----------------------------------------

  delay(10000);
}