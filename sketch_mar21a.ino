#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Replace with your own WiFi credentials
const char* ssid     = "aditya";
const char* password = "pkhg8370";

// Replace with your backend REST API endpoint
const char* serverName = "https://sensor-backend-a2mn.onrender.com/data";

// BMP280 I2C address can be 0x76 or 0x77 depending on your board
#define BMP280_I2C_ADDRESS 0x76

// Create a BMP280 object
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // Initialize the BMP280 sensor
  if (!bmp.begin(BMP280_I2C_ADDRESS)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1); // stops execution
  }
  //Optional: Set up sensor parameters if needed
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode.
                  Adafruit_BMP280::SAMPLING_X2,     // Temperature oversampling.
                  Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling.
                  Adafruit_BMP280::FILTER_X16,      // Filtering.
                  Adafruit_BMP280::STANDBY_MS_500); // Standby time.
}

void loop() {
  // Read pressure (in Pascals) and temperature (in Celsius) from the sensor
  float pressure = bmp.readPressure();        // Pressure in Pa
  float temperature = bmp.readTemperature();  // Temperature in °C
  float pressurehpa = pressure / 100.0;

  Serial.print("Pressure: ");
  Serial.print(pressurehpa);
  Serial.print(" Pa, Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

   // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi Disconnected. Attempting to reconnect...");
      WiFi.reconnect();
      delay(5000); // Wait for reconnection attempt
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to reconnect to WiFi.");
        return; // Skip sensor reading and sending if WiFi is not connected
      } else {
        Serial.println("WiFi reconnected!");
      }
    }

  // Create JSON payload
  // Estimate capacity: adjust if adding more fields. For two floats, 200 bytes is safe.
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["maxtemp"] = temperature;
  jsonDoc["pressure"] = pressurehpa;

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer);

  // Check WiFi connection
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);  // Specify destination URL
    http.addHeader("Content-Type", "application/json");  // Set content type header

    // Send HTTP POST request
    int httpResponseCode = http.POST(jsonBuffer);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response from server: ");
      Serial.println(response);

      if (response.indexOf("success") >= 0) {
        Serial.println("Backend acknowledged data successfully.");
      } else if (httpResponseCode >= 400) {
        Serial.println("Backend reported an error.");
      }
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();  // Free resources
  } else {
    Serial.println("WiFi Disconnected");
  }
  
  // Wait for 30 seconds before next reading
  delay(30000);
}
