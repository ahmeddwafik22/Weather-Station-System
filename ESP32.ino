#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include "time.h"
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11 


/* 1. Define the WiFi credentials */
#define WIFI_SSID "Orange-me4hena"
#define WIFI_PASSWORD "Mlawl1177502022"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyAH92ZwKoBLfRfL9g4aLDlhmzGjlBybS9g"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://iot-project-b09d1-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "farh@gmail.com"
#define USER_PASSWORD "Mlawl1177502022"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseJson json;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

float prevTemp =0;
float prevAltitude=0 ;
float prevHumidity=0 ;
float prevPressure =0;
const char* ntpServer = "pool.ntp.org";
bool tempBool = true , humidityBool = false  , pressureBool = false  , altitudeBool = false ;
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;
int start  ;
int lcdTimer ;
void setup() {
  start = millis();
  lcdTimer = millis();
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  configTime(7200, 0, ntpServer);
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  

  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  }
  Serial.println(F("DHTxx test!"));
  dht.begin();


   
}
  
void loop() {
  
  float pressure = bmp.readPressure() ;
  
    
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  float Altitude = bmp.readAltitude();
  
    
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

  

  
   // Firebase.ready() should be called repeatedly to handle authentication tasks.

if (Firebase.ready() &&(prevTemp != t || prevAltitude != Altitude || prevPressure != pressure || prevHumidity != h)){
    sendDataPrevMillis = millis();
    Firebase.setString(fbdo,"/esp32/altitude",String(Altitude));
    Firebase.setString(fbdo,"/esp32/humidity",String(h));
    Firebase.setString(fbdo,"/esp32/pressure",String(pressure));
    Firebase.setString(fbdo,"/esp32/temp",String(t));
    prevTemp = t;
    prevAltitude = Altitude ;
    prevPressure = pressure ;
    prevHumidity = h;

    
}
if (millis()-start  >=10000 )
{
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  char timeStringBuff[50]; //50 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  //print like "const char*"
  Firebase.pushString(fbdo,"/data/altitude",String(Altitude));
  Firebase.pushString(fbdo,"/data/humidity",String(h));
  Firebase.pushString(fbdo,"/data/pressure",String(pressure));
  Firebase.pushString(fbdo,"/data/temp",String(t));
  Firebase.pushString(fbdo,"/data/time",String(timeStringBuff));
  start = millis();
}

delay(500);
if (millis()-lcdTimer >=5000)
{
  if (tempBool == true)
  {
    tempBool = false ;
    humidityBool = true ;
    String s = "*"+String(t);
    Serial.print(s);
    delay(100);
  }
  else if (humidityBool == true )
  {
    humidityBool = false ;
    pressureBool = true;
    String s = "/"+String(h);
    Serial.print(s);
    delay(100);
  }
  else if (pressureBool==true)
  {
    pressureBool = false;
    altitudeBool = true ;
    String s = "-"+String(pressure);
    Serial.print(s);
    delay(100);
  }
  else if (altitudeBool == true)
  {
    altitudeBool = false ;
    tempBool = true ;
    String s = "+"+String(Altitude);
    Serial.print(s);
    delay(100);
  }
  lcdTimer = millis();
}

}

