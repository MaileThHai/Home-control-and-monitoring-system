#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "HUY QUANG";
const char* password = "khongbiet";

String openWeatherMapApiKey = "8abc699afb41ebfb14b3913e4d627245";

String lat = "12.0192";   // vĩ độ
String lon = "107.1799";  // kinh độ

String jsonBuffer;
String jsonBuffer1;
String jsonBuffer2;
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {

      String serverPathToday = "http://api.openweathermap.org/data/2.5/weather?lat=" + lat + "&lon=" + lon + "&appid=" + openWeatherMapApiKey;
      String serverPathTomorrow = "http://api.openweathermap.org/data/2.5/forecast?lat=" + lat + "&lon=" + lon + "&appid=" + openWeatherMapApiKey;
      String serverPathDayAfterTomorrow = "http://api.openweathermap.org/data/2.5/forecast?lat=" + lat + "&lon=" + lon + "&appid=" + openWeatherMapApiKey + "&cnt=3";

      jsonBuffer = httpGETRequest(serverPathToday.c_str());
      jsonBuffer1 = httpGETRequest(serverPathTomorrow.c_str());
      jsonBuffer2 = httpGETRequest(serverPathDayAfterTomorrow.c_str());
      Serial.println(jsonBuffer);
      Serial.println(jsonBuffer1);
      Serial.println(jsonBuffer2);
      JSONVar myObjectToday = JSON.parse(jsonBuffer);
      JSONVar myObjectTomorrow = JSON.parse(jsonBuffer1);
      JSONVar myObjectDayAfterTomorrow = JSON.parse(jsonBuffer2);

      if (JSON.typeof(myObjectToday) == "undefined" || JSON.typeof(myObjectTomorrow) == "undefined" || JSON.typeof(myObjectDayAfterTomorrow) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

      Serial.print("JSON object = ");
      Serial.println(myObjectToday);
      Serial.print("nhiệt độ day1: ");
      Serial.println(myObjectToday["main"]["temp"]);
      Serial.print("áp suất day1: ");
      Serial.println(myObjectToday["main"]["pressure"]);
      Serial.print("độ ẩm day1: ");
      Serial.println(myObjectToday["main"]["humidity"]);
      Serial.print("tốc độ gió day1: ");
      Serial.println(myObjectToday["wind"]["speed"]);

      Serial.print("JSON object = ");
      Serial.println(myObjectTomorrow);
      Serial.print("nhiệt độ day2: ");
      Serial.println(myObjectTomorrow["main"]["temp"]);
      Serial.print("áp suất day2: ");
      Serial.println(myObjectTomorrow["main"]["pressure"]);
      Serial.print("độ ẩm day2: ");
      Serial.println(myObjectTomorrow["main"]["humidity"]);
      Serial.print("tốc độ gió day2: ");
      Serial.println(myObjectTomorrow["wind"]["speed"]);

      Serial.print("JSON object = ");
      Serial.println(myObjectDayAfterTomorrow);
      Serial.print("nhiệt độ day3: ");
      Serial.println(myObjectDayAfterTomorrow["main"]["temp"]);
      Serial.print("áp suất day3: ");
      Serial.println(myObjectDayAfterTomorrow["main"]["pressure"]);
      Serial.print("độ ẩm day3: ");
      Serial.println(myObjectDayAfterTomorrow["main"]["humidity"]);
      Serial.print("tốc độ gió day3: ");
      Serial.println(myObjectDayAfterTomorrow["wind"]["speed"]);

    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
