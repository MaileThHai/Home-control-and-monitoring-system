#include <WiFi.h>
#include <HTTPClient.h>
// #include <Arduino_JSON.h>
#include <ArduinoJson.h>
const char* ssid = "HUY QUANG";
const char* password = "khongbiet";

String openWeatherMapApiKey = "8abc699afb41ebfb14b3913e4d627245";

String strjsonBuffer;
DynamicJsonBuffer jsonBuffer;

String lat = "11.8317";     // vĩ độ
String lon = "106.9883";    // kinh độ
int desiredHourCount = 24;  // Số lượng giờ bạn muốn lấy

// Khai báo cấu trúc dữ liệu để lưu trữ thông tin về nhiệt độ, độ ẩm, áp suất và tốc độ gió
struct WeatherData {
  double minTemp;
  double maxTemp;
  int maxHumidity;
  int maxPressure;
  double maxWindSpeed;
};

// Khai báo biến toàn cục của kiểu cấu trúc WeatherData để lưu trữ thông tin của 3 ngày
WeatherData weatherDataFor3Days[3];

// Hàm khởi tạo giá trị ban đầu cho biến weatherDataFor3Days
void initializeWeatherData() {
  for (int i = 0; i < 3; ++i) {
    weatherDataFor3Days[i].minTemp = INFINITY;
    weatherDataFor3Days[i].maxTemp = -INFINITY;
    weatherDataFor3Days[i].maxHumidity = 0;
    weatherDataFor3Days[i].maxPressure = 0;
    weatherDataFor3Days[i].maxWindSpeed = 0;
  }
}

unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Khởi tạo giá trị ban đầu cho biến weatherDataFor3Days
  initializeWeatherData();

  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop() {
  // Send an HTTP GET request
  // if ((millis() - lastTime) > timerDelay) {
  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    String serverPath = "http://api.openweathermap.org/data/2.5/forecast?lat=" + lat + "&lon=" + lon + "&exclude=hourly" + "&appid=" + openWeatherMapApiKey + "&cnt=" + String(desiredHourCount);
    strjsonBuffer = httpGETRequest(serverPath.c_str());
    JsonObject& myObject = jsonBuffer.parseObject(strjsonBuffer);

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
      // Lấy dự báo thời tiết cho giờ hiện tại
      JsonObject& forecast = myObject["list"][i];

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

    // Lặp qua mỗi mục trong dữ liệu JSON và cập nhật weatherDataFor3Days tương ứng
    for (int i = 0; i < desiredHourCount; ++i) {
      // Lấy dự báo thời tiết cho giờ hiện tại
      JsonObject& forecast = myObject["list"][i];

      // Lấy ngày từ dt_txt
      String dt_txt = forecast["dt_txt"];
      String currentDate = dt_txt.substring(0, 10);

      // Nếu là một ngày mới, tăng chỉ số ngày
      if (i > 0 && currentDate != uniqueDates[dayIndex]) {
        dayIndex++;
      }

      // Lấy nhiệt độ tối thiểu và tối đa từ dự báo thời tiết
      double tempMin = forecast["main"]["temp_min"];
      double tempMax = forecast["main"]["temp_max"];
      int humidityT = forecast["main"]["humidity"];
      int pressureT = forecast["main"]["pressure"];
      double windSpeedT = forecast["wind"]["speed"];

      // Cập nhật nhiệt độ tối thiểu nhỏ nhất và nhiệt độ tối đa lớn nhất cho mỗi ngày
      if (tempMin < weatherDataFor3Days[dayIndex].minTemp) {
        weatherDataFor3Days[dayIndex].minTemp = tempMin;
      }
      if (tempMax > weatherDataFor3Days[dayIndex].maxTemp) {
        weatherDataFor3Days[dayIndex].maxTemp = tempMax;
      }

      // Cập nhật giá trị tối đa cho độ ẩm, áp suất và tốc độ gió
      if (humidityT > weatherDataFor3Days[dayIndex].maxHumidity) {
        weatherDataFor3Days[dayIndex].maxHumidity = humidityT;
      }
      if (pressureT > weatherDataFor3Days[dayIndex].maxPressure) {
        weatherDataFor3Days[dayIndex].maxPressure = pressureT;
      }
      if (windSpeedT > weatherDataFor3Days[dayIndex].maxWindSpeed) {
        weatherDataFor3Days[dayIndex].maxWindSpeed = windSpeedT;
      }
    }

    // In ra thông tin về thời tiết của 3 ngày
    for (int i = 0; i < 3; i++) {
      Serial.print("Ngày ");
      Serial.print(i + 1);
      Serial.println(":");
      Serial.print("   Nhiệt độ tối thiểu: ");
      Serial.println(weatherDataFor3Days[i].minTemp);
      Serial.print("   Nhiệt độ tối đa: ");
      Serial.println(weatherDataFor3Days[i].maxTemp);
      Serial.print("   Độ ẩm tối đa: ");
      Serial.println(weatherDataFor3Days[i].maxHumidity);
      Serial.print("   Áp suất tối đa: ");
      Serial.println(weatherDataFor3Days[i].maxPressure);
      Serial.print("   Tốc độ gió tối đa: ");
      Serial.println(weatherDataFor3Days[i].maxWindSpeed);
    }

    jsonBuffer.clear();
  } else {
    Serial.println("WiFi Disconnected");
  }
  // lastTime = millis();
  delay(10000);
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
