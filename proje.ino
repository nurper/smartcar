#define trigPin 2
#define echoPin 3
int lightSensor = A0;
int lightValue = 0;
int security = HIGH; //turns off when opened with 'key'
int ignition = LOW; //tunrns on when pressed ignition button
int frontled1 = 4;
int frontled2 = 5;
int securityled = 6;//blinks when burglar ignites the car, turn on when car is closed
int openingled = 7; //(green) blinks two times when car is open, turn on when car is ignited
int buttonpin = 53; //button for ignition, if it is pressed with security HIGH then security alarm
int buttonstate = 0;

#include <MFRC522.h> 
//Get library from https://github.com/ljos/MFRC522
#include <SPI.h>

/*
How to connect RFID
Pins        SPI    UNO  Mega2560  Leonardo
1 (NSS) SAD (SS)   10     53        10
2       SCK        13     52        SCK1
3       MOSI       11     51        MOSI1
4       MISO       12     50        MISO1
5       IRQ        *      *         *
6       GND       GND     GND       GND
7       RST        5      ?         Reset
8      +3.3V (VCC) 3V3     3V3       3.3V
* Not needed
1 on ICPS header
*/
#define SAD 10
#define RST 8 //5
MFRC522 nfc(SAD, RST);

//libraries for Barometer
#include <DHT.h>
#include <DHT_U.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);


void setup() {
  // put your setup code here, to run once:
pinMode(trigPin, OUTPUT); // setting the output pin with fast output (will output signals in high speed)
pinMode(echoPin, INPUT); // setting the input pin with fast input (will get signals in high speed) 
}

#define AUTHORIZED_COUNT 1 /*If you want more Authorized of cards set the count here, and then add the serials below*/
byte Authorized[AUTHORIZED_COUNT][6] = {

                            {0x45, 0x14, 0x39, 0x2E, 0xFF, 0xFF, }
                           /*,{0x10, 0x14, 0x39, 0x2E, 0xFF, 0xFF, }*/  /*f. example how to add more authorized cards*/
                            
                          };
                          
void printSerial(byte *serial);
boolean isSame(byte *key, byte *serial);
boolean isAuthorized(byte *serial);

void loop() {
  byte status;
  byte data[MAX_LEN];
  byte serial[5];
  boolean Opening = false;
  digitalWrite(securityled, !Opening);
 
  status = nfc.requestTag(MF1_REQIDL, data);

  if (status == MI_OK) {
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);
    
    if(isAuthorized(serial))
    { 
      Serial.println("Authenticated");
      Opening = true;
    }
    else
    { 
      printSerial(serial);
      Serial.println("is NOT authenticated");
      Opening = false;
    }
    
    nfc.haltTag();
    
    digitalWrite(openingled, Opening);
    digitalWrite(securityled, !Opening);
    delay(2000);
    
    security = !Opening;
  }

  delay(500);
  
  // put your main code here, to run repeatedly:
  lightValue = analogRead(lightSensor);

  if(ignition==HIGH){
    if(lightValue<135){
      frontled1 = HIGH;
      frontled2 = HIGH;
    }else{
      frontled1 = LOW;
      frontled2 = LOW;
    }
  }
  
  

    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure());
    Serial.println(" Pa");
    
    Serial.println();
    delay(2000);

}


boolean isSame(byte *key, byte *serial)
{
    for (int i = 0; i < 4; i++) {
      if (key[i] != serial[i])
      { 
        return false; 
      }
    }
    
    return true;

}

boolean isAuthorized(byte *serial)
{
    for(int i = 0; i<AUTHORIZED_COUNT; i++)
    {
      if(isSame(serial, Authorized[i]))
        return true;
    }
   return false;
}

void printSerial(byte *serial)
{
        Serial.print("Serial:");
    for (int i = 0; i < 4; i++) {
      Serial.print(serial[i], HEX);
      Serial.print(" ");
    }
}

