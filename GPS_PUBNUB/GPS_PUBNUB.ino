//////////GPS TRACKER//////////////////////////////////////

/////////LATITUDE LONGITUDE//////////////////////////////

///////TESTED 12/06/2018/////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
//#include <ThingSpeak.h>


//////VARIA0BLES/////////////////////////
static const int RXPin = 12 , TXPin = 13;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;
//WiFiClient client;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

//////////WIFI SETTINGS/////////////////
const char* ssid     = "vivo1727";
const char* pass = "04MAY2000";

//unsigned long myChannelNumber = 534636;
//const char * myWriteAPIKey  ="P68W6J8LZJIKRFCB";
 

void setup()
{
 
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, pass);

  while (WiFi.status()!= WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  //ThingSpeak.begin(client);
  Serial.println();
 
}

void loop()
{
  
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();   //////////////NOW CALL FUNCION//////////////       

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
}

void displayInfo()
{
  double latitude;
  double longitude;
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);

    /*ThingSpeak.setField(1,(String)longitude);
    ThingSpeak.setField(2, (String)latitude);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  */
    delay(500);
  
    ///////////////JSON PARSING/////////////////////
    char message[100];
    sprintf(message, "{\"id\":47,\"Lat\":%3.1f,\"Lng\":%3.1f}", latitude, longitude);
    Serial.println(message);
    ////////////////HTTP POST//////////////////////

        HTTPClient http;  //Declare an object of class HTTPClient     //1.

    ////////////////PUB NUB////////////////////////////
    char target[] = "http://ps.pndsn.com/publish/pub-c-4a16c1ae-c7fa-4315-966d-a947f6080bb2/sub-c-2443cc4a-7bad-11e8-bf64-d6949d01620d/0/cityggn/0?store=0&uuid=cycle";
    http.begin(target);  //Specify request destination         ////////commented for testing///////////
//    http.begin("http://ps.pndsn.com/publish/pub-c-15cf8036-7552-427f-acd6-f89b855d1e79/sub-c-51a5b564-6860-11e8-9499-26ede0a09d22/0/officeDesk/0?store=0&uuid=12121212");  //2.

    int httpCode = http.POST(message);   //Send the request     //3.
    String payload = http.getString();                  //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code        //4.                
    Serial.println(payload);    //Print request response payload    //5.

    http.end();  //Close connection                               //6.    

    //////////////////HTTP POST////////////////////////////
    delay(5000);  //Send a request every 5 seconds                 //7.   
   }
  else
  {
    Serial.print(F("INVALID"));
  }
//////////////TIME PART//////////COMMENTED///////////////////
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
