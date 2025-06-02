#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "JioFi2_A98992";
const char* password = "mm69cbbwmc";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Make API request once at startup
  fetchJsonData();
}

void loop() {
  // You could repeat every 30s if needed
  delay(30000);
  fetchJsonData();
}

void fetchJsonData() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    const char* url = "http://192.168.1.106:5000/atyourservice/switch";
    http.begin(client, url); // use WiFiClient + URL

    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("\nReceived payload:");
      Serial.println(payload);

      // Parse JSON
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return;
      }

       JsonArray nestedField = doc["data"].as<JsonArray>();
       if (!nestedField.isNull()) {

           for (JsonObject obj : nestedField) {
            int switch_name = obj["switch_name"];
            Serial.print("Swirtch name: "); Serial.println(switch_name);
          }
       }
       else {
          Serial.println("Nested Field is missing");
        }
    } else {
      Serial.print("HTTP GET failed: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end();
  }
}
