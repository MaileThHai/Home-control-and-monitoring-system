#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "HUY QUANG";
const char* password = "khongbiet";

String openWeatherMapApiKey = "8abc699afb41ebfb14b3913e4d627245";

String lat = "16.076667";   // vĩ độ
String lon = "108.222778"; // kinh độ

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
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
    if(WiFi.status() == WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/forecast?lat=" + lat + "&lon=" + lon + "&appid=" + openWeatherMapApiKey;
      String jsonBuffer = httpGETRequest(serverPath.c_str());
      JSONVar myObject = JSON.parse(jsonBuffer);
      
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

      // Loop through the forecast data for the next 3 days
      for (int i = 0; i < 3; i++) {
        JSONVar forecast = myObject["list"][i * 8]; // Lấy thông tin thời tiết cho mỗi ngày
        Serial.println("Forecast for Day " + String(i + 1) + ":");
        Serial.print("Temperature: ");
        Serial.print(forecast["main"]["temp"]);
        Serial.println(" °C");
        Serial.print("Pressure: ");
        Serial.print(forecast["main"]["pressure"]);
        Serial.println(" hPa");
        Serial.print("Humidity: ");
        Serial.print(forecast["main"]["humidity"]);
        Serial.println(" %");
        Serial.print("Wind Speed: ");
        Serial.print(forecast["wind"]["speed"]);
        Serial.println(" m/s");
      }
    }
    else {
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
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
