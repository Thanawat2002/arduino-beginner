#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"

#define DHTPIN 25 // !!!!!! Pin of DHT !!!!!!
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321

//----------------------------------------SSID dan Password wifi mu gan.
const char* ssid = "SetzeroDev-2.4G";       //--> Nama Wifi / SSID.
const char* password = "51552105315";  //--> Password wifi .
//----------------------------------------
//----------------------------------------Host &fhttpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;  //--> Create a WiFiClientSecure object.
//AKfycbySBGfo6hRB5AP0RYibrL8uHyJSIX06BKs6b7JvMCVXq0MXca7WDDRJaN3cTnQmuTiTRA
String GAS_ID = "AKfycbwSemmgtifdYPOQbi3ExuZePmyXlsyV-9VdMMbu6k9ACTtfT5F7zuZvE4tKhAsux9Ok";  //--> spreadsheet script ID

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht.begin();

  WiFi.begin(ssid, password);  //--> Connect to your WiFi router
  Serial.println("");
  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  client.setInsecure();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(30000);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  client.setInsecure();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.println(F("°F"));

  sendData(t, h);
}
void sendData(float value, float value2) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {

    Serial.println("connection failed");
    return;
  }
  //----------------------------------------
  //----------------------------------------Proses dan kirim data
  float string_temp = value;
  float string_humi = value2;
  String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temp + "&humi=" + string_humi;
  // 2 variables
  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP32\r\n" + "Connection: close\r\n\r\n");
  Serial.println("request sent");
  //----------------------------------------
  //---------------------------------------
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp32/Arduino CI successfull!");
  } else {
    Serial.println("esp32/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
}