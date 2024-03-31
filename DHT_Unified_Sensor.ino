
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>
#include <WiFi.h>
// #include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

const char* ssid = "CTCO_2.4"; // Tên mạng WiFi của bạn
const char* password = "1611060291"; // Mật khẩu mạng WiFi của bạn
// const char* password = "123456789";
String jsonStr;
StaticJsonDocument<200> doc;

HTTPClient http;

// AsyncWebServer server(80);
#define Rainpin 35
#define COpin 34

#define DHTPIN 15     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT22     // DHT 22 (AM2302)


DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setup() {
  Serial.begin(115200);
  // Initialize device.
  dht.begin();
  pinMode(2,OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("...");
  }
  // server.begin();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Print temperature sensor details.
  sensor_t sensor;

  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;



}

void loop() {
  // Delay between measurements.
  // delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    // Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    // Serial.println(F("°C"));
// int temperature = event.temperature;
    doc["temperture"] = String(event.temperature);

  }
  
  int valueCO= analogRead(COpin);
  // Serial.print("CO value: ");
  // Serial.println(valueCO);
  doc["gasdata"] = String(valueCO);

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    // Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    // Serial.println(F("%"));
    doc["humidity"] = String(event.relative_humidity);
// int humidity = event.relative_humidity;
    // doc["humidity"] = String(humidity);

  }
  if (String(event.relative_humidity)="null"){
  dht.begin();} 

  int rain= analogRead(Rainpin);
  // Serial.print("Rain value: ");
  // Serial.println(4095-rain);

  if (rain<4090)
  doc["raindrop"] = true;
  else
  doc["raindrop"] = false;

  jsonStr="";
 
  serializeJson(doc, jsonStr);

  // In ra serial monitor
  // Serial.println(jsonOutput);
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // request->send(200, "text/plain", jsonOutput);
    // client.println("<h1>ESP32 Web Server</h1>");
  // });

sendAPIRequest(jsonStr);

}
// String serverUrl="http://http://27.78.218.89:88/updateReport";
const char* serverUrl = "http://27.78.218.89:88/updateReport";

void sendAPIRequest(String jsonStr) {
  delay(2000);
    int contentLength = jsonStr.length();

  String host = serverUrl;
  host.remove(0, 7); // Loại bỏ "http://"

  // Tách phần path từ URL
  int pathIndex = host.indexOf("/");
  String path = host.substring(pathIndex);
  host.remove(pathIndex);

  // Gửi yêu cầu HTTP PUT
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(contentLength));
  http.addHeader("Host", host);
    Serial.println(host);
    Serial.println(jsonStr);
  int httpResponseCode = http.PUT(jsonStr);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      if (httpResponseCode=200){
        digitalWrite(2, HIGH);  // turn the LED on (HIGH is the voltage level)
        delay(100);                      // wait for a second
        digitalWrite(2, LOW);   // turn the LED off by making the voltage LOW
        delay(100);  
      }
      String payload = http.getString();
      Serial.println("Response payload:");
      Serial.println(payload);
  
}
