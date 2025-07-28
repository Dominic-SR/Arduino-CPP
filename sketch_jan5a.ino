#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "";
const char* password = "";

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






//Node JS mqtt_server.js
const mqtt = require('mqtt');
const client = mqtt.connect('mqtt://192.168.1.100'); // Use your Mosquitto broker IP

client.on('connect', () => {
  console.log('✅ Connected to MQTT broker');
});

// Function to control relay
function controlRelay(relay, state) {
  const command = `${relay}_${state}`; // e.g., relay1_on
  client.publish('relay/control', command);
  console.log(`Published: ${command}`);
}

// Example: Turn all ON
setTimeout(() => {
  controlRelay('relay1', 'on');
  controlRelay('relay2', 'on');
  controlRelay('relay3', 'on');
  controlRelay('relay4', 'on');
}, 2000);

// Example: Turn all OFF after 10 seconds
setTimeout(() => {
  controlRelay('relay1', 'off');
  controlRelay('relay2', 'off');
  controlRelay('relay3', 'off');
  controlRelay('relay4', 'off');
}, 112000);

















// CPP


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "JioFi2_A98992";
const char* password = "mm69cbbwmc";
const char* mqtt_server = "192.168.1.100";  // Your PC IP where Mosquitto is running

WiFiClient espClient;
PubSubClient client(espClient);

// Relay pins
#define RELAY1 D1
#define RELAY2 D2
#define RELAY3 D5
#define RELAY4 D6

void setup_wifi() {
  delay(10);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String command = String((char*)payload);

  Serial.print("Command received: ");
  Serial.println(command);

  if (command == "relay1_on") digitalWrite(RELAY1, LOW);
  else if (command == "relay1_off") digitalWrite(RELAY1, HIGH);
  else if (command == "relay2_on") digitalWrite(RELAY2, LOW);
  else if (command == "relay2_off") digitalWrite(RELAY2, HIGH);
  else if (command == "relay3_on") digitalWrite(RELAY3, LOW);
  else if (command == "relay3_off") digitalWrite(RELAY3, HIGH);
  else if (command == "relay4_on") digitalWrite(RELAY4, LOW);
  else if (command == "relay4_off") digitalWrite(RELAY4, HIGH);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected");
      client.subscribe("relay/control");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(RELAY1, OUTPUT); digitalWrite(RELAY1, HIGH);
  pinMode(RELAY2, OUTPUT); digitalWrite(RELAY2, HIGH);
  pinMode(RELAY3, OUTPUT); digitalWrite(RELAY3, HIGH);
  pinMode(RELAY4, OUTPUT); digitalWrite(RELAY4, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}




// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>
// #include <ArduinoJson.h>

// const char* ssid = "JioFi2_A98992";
// const char* password = "mm69cbbwmc";

// // Replace with your actual API endpoint (local IP or public URL)
// const char* apiUrl = "http://192.168.1.100:5000/atyourservice/utilities/roles"; // ✅ example, replace with your actual URL

// void setup() {
//   // Wifi Connection sextion start
//   Serial.begin(9600);
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("\nWiFi connected");
//   // Wifi Connection sextion end

//   WiFiClient client;
//   HTTPClient http;
//   http.begin(client, apiUrl); // Use HTTP, not HTTPS for ESP8266 (unless using WiFiClientSecure)

//   int httpCode = http.GET();

//   if (httpCode == HTTP_CODE_OK) {
//     String payload = http.getString();
//     Serial.println("Response:");
//     Serial.println(payload);

//     // Use sufficient size based on your JSON
//     DynamicJsonDocument doc(2048);
//     DeserializationError error = deserializeJson(doc, payload);

//     if (error) {
//       Serial.print("JSON parse failed: ");
//       Serial.println(error.c_str());
//       return;
//     }

//     // Check and access 'data' array
//     if (doc.containsKey("data") && doc["data"].is<JsonArray>()) {
//       JsonArray data = doc["data"];
//       for (JsonObject role : data) {
//         const char* desc = role["role_desc"];
//         Serial.print("Role Description: ");
//         Serial.println(desc);
//       }
//     } else {
//       Serial.println("Invalid JSON format: 'data' field not found or not an array.");
//     }

//   } else {
//     Serial.printf("HTTP GET failed. Code: %d, Error: %s\n", httpCode, http.errorToString(httpCode).c_str());
//   }

//   http.end();
// }

// void loop() {
//   // Nothing to do
// }





