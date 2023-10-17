#include <TridentTD_LineNotify.h>
#include <DHT.h>
#define DHTPIN 25        //pin connect DHT
#define DHTTYPE DHT22     //DHT11, DHT22 type of Sensor
#define SSID        "SetzeroDev-2.4G"
#define PASSWORD    "51552105315"
#define LINE_TOKEN  "tCZHlqtrh2TIec4nJONOIX5rRVbIXPvIhMAyyFaktdZ"

DHT dht(DHTPIN, DHTTYPE);
const int Led_1 = 17;

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  
  LINE.setToken(LINE_TOKEN);  // กำหนด Line Token
  dht.begin();
  pinMode(Led_1, OUTPUT);
}
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int adc=analogRead(A0);
  if (isnan(t) || isnan(h))
  {
    Serial.println("Error Reading DHT 22 !!!");
    delay(1000);
    return;    
  } else if (t > 28) {
    digitalWrite(Led_1, 1);
    // Serial.println(F("อุณหภูมิเกิน 28 °C"));
    LINE.notify("อุณหภูมิขณะนี้ "+String(t)+" องศา"); 
    LINE.notify("ความชื้นขณะนี้ "+String(h)+" %");   
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("\t");
    Serial.print("Temp: ");
    Serial.println(t);
    Serial.println("...............................");    
    LINE.notify("อุณหภูมิขณะนี้ "+String(t)+" องศา"); 
    LINE.notify("ความชื้นขณะนี้ "+String(h)+" %");     
  }
  delay(5000);
}
