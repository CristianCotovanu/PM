/****************************************
* Include Libraries
****************************************/
#include <Wire.h>
#include <dht11.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

/****************************************
* Define Instances and Constants
****************************************/  
#define BAUD_RATE_NODE 115200

#define DELAY_TIME 3000
#define WARNING_DELAY_TIME 2000
#define ALERT_DELAY_TIME 5000

#define TEMP_HUMID_PIN 4
#define BUZZER_OUT_PIN 12
#define FLAME_PIN A2
#define SMOKE_PIN A1

#define HUMIDITY_THRESHOLD 5
#define TEMPERATURE_THRESHOLD 26
#define FLAME_THRESHOLD 50
#define GAS_THRESHOLD 550

#define ARDUINO_FIRE_ALERT "FIRE"
#define ARDUINO_SMOKE_ALERT "SMOKE"

dht11 DHT11;
LiquidCrystal_I2C LCD(0x27, 16, 2);
SoftwareSerial espSerial(5, 6);

int flameSensor = 0;
int gasSensor = 0;

/****************************************
* Auxiliar Functions
****************************************/

void displayTemperatureOnLCD();
void displayMessageOnLCD(const char *errorMessage, const char *errorDescription);
void sendDataToNodeMCU(String data);
void temperatureSensorFlow();
void flameSensorFlow();
void gasSensorFlow();

void displayTemperatureOnLCD()
{
  LCD.print("Humidity(%):" );
  LCD.print((float)DHT11.humidity);
  LCD.setCursor(0, 1);
  LCD.print("Temp(C):");
  LCD.print((float)DHT11.temperature);
}

void displayMessageOnLCD(const char *errorMessage, const char *errorDescription)
{
    LCD.clear();
    LCD.print(errorMessage);
    LCD.setCursor(0, 1);
    LCD.print(errorDescription);
}

void sendDataToNodeMCU(String data)
{
    espSerial.println(data);
}

void temperatureSensorFlow()
{
  DHT11.read(TEMP_HUMID_PIN);
  
  displayTemperatureOnLCD();
  
  delay(DELAY_TIME);

  if ((float)DHT11.humidity < HUMIDITY_THRESHOLD)
  {
    displayMessageOnLCD("   WARNING   ", "Humidity is too low!");
    delay(DELAY_TIME);
  }

  if ((float)DHT11.temperature > TEMPERATURE_THRESHOLD)
  {
    displayMessageOnLCD("   WARNING   ", "Temperature is too high!");
    delay(DELAY_TIME);
  }

  sendDataToNodeMCU(String("T=") + String(DHT11.temperature));
}

void flameSensorFlow()
{
  flameSensor = analogRead(FLAME_PIN);

    Serial.print("Fire Detected! flame value: ");
    Serial.println(flameSensor);
  
  if (flameSensor < FLAME_THRESHOLD)
  {
    Serial.print("Fire Detected! flame value: ");
    Serial.println(flameSensor);

    digitalWrite(BUZZER_OUT_PIN, HIGH);
    
    sendDataToNodeMCU(String(ARDUINO_FIRE_ALERT));
    displayMessageOnLCD("    ALERT    ", "Fire detected!!");
    
    delay(ALERT_DELAY_TIME);
  }

  digitalWrite(BUZZER_OUT_PIN, LOW);
  delay(flameSensor);
}

void gasSensorFlow()
{
  gasSensor = analogRead(SMOKE_PIN);
  
  if (gasSensor > GAS_THRESHOLD)
  {
    Serial.print("Gas detected! gas value: ");
    Serial.println(gasSensor);

    digitalWrite(BUZZER_OUT_PIN, HIGH);
    
    sendDataToNodeMCU(String(ARDUINO_SMOKE_ALERT));
    displayMessageOnLCD("    ALERT    ", "Gas detected!!");
    
    delay(ALERT_DELAY_TIME);
  }

  digitalWrite(BUZZER_OUT_PIN, LOW);
  delay(gasSensor);
}

/****************************************
* Main Functions
****************************************/

void setup()
{
  Serial.begin(BAUD_RATE_NODE);
  espSerial.begin(BAUD_RATE_NODE);
  
  LCD.begin();
  LCD.backlight();

  pinMode(BUZZER_OUT_PIN, OUTPUT);
  pinMode(SMOKE_PIN, INPUT);
}

void loop()
{
  LCD.clear();
  
  temperatureSensorFlow();
  flameSensorFlow();
  gasSensorFlow();
}
