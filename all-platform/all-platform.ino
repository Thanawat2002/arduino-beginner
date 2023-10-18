/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include "DHT.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TridentTD_LineNotify.h>
#include "ThingSpeak.h" 
#include <HTTPClient.h> 

// ############################# Config #################################
char ssid[] = "SetzeroDev-2.4G"; // User Wifi
char pass[] = "51552105315"; // Password Wifi
#define LINE_TOKEN  "LVGavsQCcAAZwhh77ylAQ03HZPNhEgSKISnkgs45YRs" // Token Line Notify
unsigned long myChannelNumber = 2307944;          //เลข Channel ID
const char* myWriteAPIKey = "HTV4C9BZ08SKNRUQ";  //API KEY
String GAS_ID = "AKfycbz8YWs9xx1w7zJEsa-qe_kTNerT1fvpOB2UwkdpyDgcbSgKcjuGpfLaaTpdsfVpmzNOJg"; // ของ google sheets
/* Copy Device Info */
#define BLYNK_TEMPLATE_ID "TMPL6mBVuwHeE"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "vpgI4XhUnFehRfq1k1CSHdBhhRIfZ6wq"
// ######################################################################


// ############################# Config pin #############################
#define DHTPIN 26
const int LED_1 = 16;
const int LED_2 = 17;
// ######################################################################



const char* host = "script.google.com";
const int httpsPort = 443;

BlynkTimer timer;
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
WiFiClientSecure client_secure;

BLYNK_WRITE(V4)
{
  int value = param.asInt();

  Blynk.virtualWrite(V2, value); // V ของ LED 1
  digitalWrite(LED_1, value);
}

BLYNK_WRITE(V5)
{
  int value = param.asInt();

  Blynk.virtualWrite(V3, value);
  digitalWrite(LED_2, value);
}


// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  if (t > 28) {
    digitalWrite(LED_1, 1); 
  } else {
    digitalWrite(LED_1, 0);
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C \n"));

  // sendData(t, h);
  sendLinenotify(t, h);
  sendBlynk(t, h);
  sendThinkspeak(t, h);
  sendGooglesheet(t, h);
}

void setup() {
  // Debug console
  Serial.begin(115200);

  timer.setInterval(10000L, myTimerEvent);
  WiFi.begin(ssid, pass);
  Serial.printf("WiFi connecting to %s\n",  ssid);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  Serial.println(LINE.getVersion());
  LINE.setToken(LINE_TOKEN);

  ThingSpeak.begin(client);
  client_secure.setInsecure();

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
}



void loop() {
  Blynk.run();
  timer.run();
}



void sendData(float value, float value2) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  //----------------------------------------Connect to Google host
  if (!client_secure.connect(host, httpsPort)) {

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
  client_secure.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP32\r\n" + "Connection: close\r\n\r\n");
  Serial.println("request sent");
  //----------------------------------------
  //---------------------------------------
  while (client_secure.connected()) {
    String line = client_secure.readStringUntil('\n');
    if (line == "\r") {      Serial.println("headers received");
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

void sendGooglesheet(float t, float h) {

  String urlFinal = "https://script.google.com/macros/s/" + GAS_ID + "/exec?" + "&temp=" + String(t) + "&humi=" + String(h);
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload: " + payload);
    }
    //---------------------------------------------------------------------
    http.end();
}

void sendThinkspeak(float t, float h) {

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

}

void sendBlynk(float t, float h) {

  Blynk.virtualWrite(V0,t);
  Blynk.virtualWrite(V1,h);

}

void sendLinenotify(float t, float h) {

  if (t > 20) {
    digitalWrite(LED_1, 1);
    LINE.notify("อุณหภูมิขณะนี้ "+String(t)+" องศา"); 
    LINE.notify("ความชื้นขณะนี้ "+String(h)+" %");  
  } else {
    digitalWrite(LED_1, 0);
  }

}