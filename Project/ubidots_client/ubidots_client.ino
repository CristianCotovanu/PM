/****************************************
* Include Libraries
****************************************/
#include <SoftwareSerial.h>
#include "Ubidots.h"

/****************************************
* Define Instances and Constants
****************************************/   
#define BAUD_RATE_NODE 115200
#define DELAY_TIME 5000

#define UBIDOTS_FIRE "Fire"
#define UBIDOTS_SMOKE "Smoke"
#define UBIDOTS_TEMP "Temperature"

#define ARDUINO_FIRE_ALERT "FIRE"
#define ARDUINO_SMOKE_ALERT "SMOKE"

#define D1 4
#define D2 5
 
const char* UBIDOTS_TOKEN = "BBFF-lWjGmy4h5zi9LJntHTSJasWkoemhmq";
const char* WIFI_SSID = "Cristian-2.4GHz";
const char* WIFI_PASS = "homodeus234#";

SoftwareSerial espSerial(D1, D2);
Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);

/****************************************
* Auxiliar Functions
****************************************/

void setVariable(const char *variable);
void resetVariables();
void processIncomingDataFromArduino();

void setVariable(const char *variable)
{
  sendVariableDataToCloud(variable, 1);
}

void resetVariables()
{
  sendVariableDataToCloud(UBIDOTS_FIRE, 0);
  sendVariableDataToCloud(UBIDOTS_SMOKE, 0);
}

void sendVariableDataToCloud(const char *variableName, float value)
{
  ubidots.add(variableName, value);
  
  bool sentData = false;
  sentData = ubidots.send();
  
  if (sentData) {
    Serial.print("Value updated for ");
    Serial.println(variableName);
  }
}

void processIncomingDataFromArduino()
{
  byte arduinoSentData = espSerial.available();
  
  if (arduinoSentData != 0)
  {
    String receivedData = (String)espSerial.readString();
    Serial.println(receivedData);   //print to serial monitor what I have received
    
    if (receivedData == ARDUINO_FIRE_ALERT)
    {
      Serial.println("ALERT Fire!!");
      setVariable("fire");
    }
    else if (receivedData.equals(String(ARDUINO_SMOKE_ALERT)))
    {
      Serial.println("ALERT Smoke!!");
      setVariable("smoke");
    }
    else if (receivedData.charAt(0) == 'T')
    {
      float temp = receivedData.substring(2).toFloat();
      Serial.print("Temperature: ");
      Serial.println(temp);
      sendVariableDataToCloud(UBIDOTS_TEMP, temp);
    }
  }
}

/****************************************
* Main Functions
****************************************/  

void setup()
{      
  Serial.begin(BAUD_RATE_NODE);     
  espSerial.begin(BAUD_RATE_NODE);

  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  ubidots.setDebug(true);
}

void loop()
{
  processIncomingDataFromArduino();
  delay(DELAY_TIME);
}
