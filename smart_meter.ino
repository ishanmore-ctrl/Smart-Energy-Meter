#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

#define WIFI_SSID "iQOO Neo9 Pro"
#define WIFI_PASSWORD "12345678 "

#define API_KEY "AIzaSyAoj2AMDCEYnaH0uRlpLta5-QFYhfLvnOo"
#define DATABASE_URL "https://smartmeter-8af17-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

SoftwareSerial pzemSerial(D7, D8);
PZEM004Tv30 pzem(pzemSerial);

unsigned long sendDataPrevMillis = 0;

void setup()
{
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.signUp(&config, &auth, "", "");
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  if (Firebase.ready() && millis() - sendDataPrevMillis > 5000)
  {
    sendDataPrevMillis = millis();

    float voltage = pzem.voltage();
    float current = pzem.current();
    float power   = pzem.power();
    float energy  = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    if (!isnan(voltage))
    {
      float bill = energy * 8.0;

      Firebase.RTDB.setFloat(&fbdo, "/smartmeter/voltage", voltage);
      Firebase.RTDB.setFloat(&fbdo, "/smartmeter/current", current);
      Firebase.RTDB.setFloat(&fbdo, "/smartmeter/power", power);
      Firebase.RTDB.setFloat(&fbdo, "/smartmeter/energy", energy);
      Firebase.RTDB.setFloat(&fbdo, "/smartmeter/bill", bill);
      Firebase.RTDB.setFloat(&fbdo, "/smartmeter/frequency", frequency);
      Firebase.RTDB.setFloat(&fbdo, "/smartmeter/powerFactor", pf);

      Serial.println("Data Uploaded");
    }
    else
    {
      Serial.println("PZEM Not Connected");
    }
  }
}