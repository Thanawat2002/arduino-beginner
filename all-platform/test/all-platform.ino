/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL6mBVuwHeE"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "ExSMZhlZZkW3LOQRRfMZkGdBtpw72JJS"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include "DHT.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TridentTD_LineNotify.h> // line
#include "ThingSpeak.h" // ThingSpeak
#include <HTTPClient.h> // test

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Thanawat iPhone";
char pass[] = "09876543";

const char* host = "script.google.com";
const int httpsPort = 443;

BlynkTimer timer;
#define DHTPIN 26
#define DHTTYPE DHT22
#define LINE_TOKEN  "nPOdp8o3UoWcB0hU8h2WmbKF7r9J0XCVlotrlOGeXR7"
unsigned long myChannelNumber = 2307944;          //เลข ID
const char* myWriteAPIKey = "HTV4C9BZ08SKNRUQ";  //API KEY
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
WiFiClientSecure client_secure;
String GAS_ID = "AKfycbyC6albbgWQYzd3P73B2cJsmn474mHzZnuu5785SCDcXqoiBsYcXo-O4gOBMFigmnDeQw";  //--> spreadsheet script ID

const int LED_1 = 25;
const int LED_2 = 27;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V4)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V2, value);
  digitalWrite(LED_1, value);
}

BLYNK_WRITE(V5)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V3, value);
  digitalWrite(LED_2, value);
}


// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent() {
  // You can send any value at any time.
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  if (t > 23) {
    LINE.notify("อุณหภูมิขณะนี้ "+String(t)+" องศา"); 
    LINE.notify("ความชื้นขณะนี้ "+String(h)+" %");  
  }
  //อัพค่าขึ้น Blynk
  Blynk.virtualWrite(V0,t);
  Blynk.virtualWrite(V1,h);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C \n"));
  // sendData(t, h);
  googlesheet(t, h);
}

void setup() {
  // Debug console
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  Serial.printf("WiFi connecting to %s\n",  ssid);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  Serial.println(LINE.getVersion());
  LINE.setToken(LINE_TOKEN);

  ThingSpeak.begin(client);

  WiFi.begin(ssid, pass);
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client_secure.setInsecure();
  // Setup a function to be called every second
  // ช่วงเวลาทำงาน
  timer.setInterval(10000L, myTimerEvent);
}



void loop() {
  Blynk.run();
  timer.run();
  // อย่าใส่ delay
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
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




void googlesheet(float t, float h) {

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
