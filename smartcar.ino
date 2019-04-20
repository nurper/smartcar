#define trigPin 2
#define echoPin 3
#define buzzer 7
bool Opening = LOW;
int lightSensor = A0;
int lightValue = 0;
int security = HIGH; //turns off when opened with 'key'
int ignition = LOW; //tunrns on when pressed ignition button
int frontled1 = 4; //4
int frontled2 = 5;
int securityled = 6;//blinks when burglar ignites the car, turn on when car is closed
//int airconditioner = 7; 
int buttonpin = 8; //button for ignition, if it is pressed with security HIGH then security alarm
int c = 0;

int ledState = LOW;
unsigned long previousMillis = 0;
unsigned long interval = 200;

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // Подключение библиотеки
//#include <LiquidCrystal_PCF8574.h> // Подключение альтернативной библиотеки

//LiquidCrystal_I2C lcd(0x27,16,2); // Указываем I2C адрес (наиболее распространенное значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов в каждой 
//LiquidCrystal_PCF8574 lcd(0x27); // Вариант для библиотеки PCF8574 

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
7       RST        5      49         Reset
8      +3.3V (VCC) 3V3     3V3       3.3V
* Not needed
1 on ICPS header
*/
#define SAD 53
#define RST 49
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
  SPI.begin();
  Serial.begin(9600);
  Serial.println("Looking for MFRC522.");
  nfc.begin();
  byte version = nfc.getFirmwareVersion();
  if (! version) {
    Serial.print("Didn't find MFRC522 board.");
    while(1); //halt
  }
  
  Serial.print("Found chip MFRC522 ");
  Serial.print("Firmware ver. 0x");
  Serial.print(version, HEX);
  Serial.println(".");
  
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT); // setting the output pin with fast output (will output signals in high speed)
  pinMode(echoPin, INPUT); // setting the input pin with fast input (will get signals in high speed) 
  pinMode(buttonpin, INPUT);
  pinMode(frontled1, OUTPUT);
  pinMode(frontled2, OUTPUT);
  pinMode(securityled, OUTPUT);
  //pinMode(openingled, OUTPUT);
  
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer,HIGH);//initialize the buzzerPin as LOW level
  
#define AUTHORIZED_COUNT 2 /*If you want more Authorized of cards set the count here, and then add the serials below*/
byte Authorized[AUTHORIZED_COUNT][6] = {

                            {0x45, 0x14, 0x39, 0x2E, 0xFF, 0xFF, }
                           ,{0xDD, 0xC7, 0x84, 0xB9, 0xFF, 0xFF, }  /* if. example how to add more authorized cards*/
                            
                          };
                          
void printSerial(byte *serial);
boolean isSame(byte *key, byte *serial);
boolean isAuthorized(byte *serial);

void loop() {
    
  byte status;
  byte data[MAX_LEN];
  byte serial[5];
  digitalWrite(securityled, security);
  
  status = nfc.requestTag(MF1_REQIDL, data);

  if (status == MI_OK) {
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);
    
    if(Opening==LOW){
      if(isAuthorized(serial)){
      Serial.println("Authenticated");
      Opening = HIGH;
      security = !Opening;
      digitalWrite(securityled, security);
      digitalWrite(buzzer, LOW);
      delay(45);
      digitalWrite(buzzer, HIGH);
      delay(45);
      digitalWrite(buzzer, LOW);
      delay(45);
      digitalWrite(buzzer, HIGH);
      delay(1000);
      }
    } else if(Opening==HIGH) {
      if(isAuthorized(serial)){
        Serial.println("Closed");
        Opening = LOW;
        security = !Opening;
        digitalWrite(securityled, security);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(1000);
      }
    } else
    { 
      printSerial(serial);
      Serial.println("is NOT authenticated");
      Opening = LOW;
    }

    
    nfc.haltTag();

    //delay(2000);
    
   
  }


  //delay(500);
   c=c+1;
    if(security == LOW){
      if(ignition == LOW && digitalRead(buttonpin) == 1){
        delay(300);
        ignition = HIGH; //ignite car
        c=0;
        digitalWrite(frontled1, HIGH);
        digitalWrite(frontled2, HIGH);
        delay(300);
        digitalWrite(frontled1, LOW);
        digitalWrite(frontled2, LOW);
        delay(300);
        digitalWrite(frontled1, HIGH);
        digitalWrite(frontled2, HIGH);
        delay(300);
        digitalWrite(frontled1, LOW);
        digitalWrite(frontled2, LOW);
        delay(300);
         
        Serial.println("Turn on");
      } else if(digitalRead(buttonpin) == 1 && (ignition == HIGH)){
        delay(300);
        ignition = LOW;
        c=0;
        digitalWrite(frontled1, HIGH);
        digitalWrite(frontled2, HIGH);
        delay(300);
        digitalWrite(frontled1, LOW);
        digitalWrite(frontled2, LOW);
        delay(300);
        digitalWrite(frontled1, HIGH);
        digitalWrite(frontled2, HIGH);
        delay(300);
        digitalWrite(frontled1, LOW);
        digitalWrite(frontled2, LOW);
        delay(300);
        
        Serial.println("Turn off");
        digitalWrite(frontled1, LOW);
        digitalWrite(frontled2, LOW);
      }
      
    } else {
      if(digitalRead(buttonpin) == 1){
        //alarm
         Serial.println("Security alarm!");
         for(int i = 0; i < 10; i++)
         {
         
          unsigned long currentMillis = millis();
        
          if (currentMillis - previousMillis >= interval) {
            // save the last time you blinked the LED
            previousMillis = currentMillis;
        
            // if the LED is off turn it on and vice-versa:
            if (ledState == LOW) {
              ledState = HIGH;
            } else {
              ledState = LOW;
            }
        
            // set the LED with the ledState of the variable:
            digitalWrite(securityled, ledState);
          }
         /*digitalWrite(securityled, HIGH);
         delay(200);
         digitalWrite(securityled, LOW);
         delay(200);*/
         tone(buzzer, 10);
         delay(100);
         noTone(buzzer);
         delay(100);
         digitalWrite(buzzer, HIGH);
         }
      } else {
         digitalWrite(securityled, HIGH); 
      }
    }

  if(ignition == HIGH) {
 long duration, distance;
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;

/* if (distance>=1000 || distance <=0) {
  Serial.println("Out of range");
 }else{
  Serial.print(distance);
  Serial.println(" cm");
 }*/
 
  /*Serial.print(distance);
  Serial.println(" cm");
  delay(100);*/
  
  if (distance <= 5) {
    
       digitalWrite(securityled, HIGH);
       delay(200);
       digitalWrite(securityled, LOW);
       delay(200);
       
       Serial.println("Caution! Distance is very small");
       lcd.clear();
       lcd.setCursor(0,0);              // Установка курсора в начало первой строки
       lcd.print("Caution!");       // Набор текста на первой строке
       lcd.setCursor(0,1);              // Установка курсора в начало второй строки
       lcd.print("Distance < 5 cm");       // Набор текста на второй строке
        
       digitalWrite(buzzer, LOW);
}  else if (distance <= 10) {
       digitalWrite(securityled, HIGH);
       delay(300);
       digitalWrite(securityled, LOW);
       delay(300);
       
       digitalWrite(buzzer, HIGH);
} else if (distance <= 20) {
       digitalWrite(securityled, HIGH);
       delay(500);
       digitalWrite(securityled, LOW);
       delay(500);
       
       digitalWrite(buzzer, HIGH);
}  else if (distance <= 25) {
       digitalWrite(securityled, HIGH);
       delay(1000);
       digitalWrite(securityled, LOW);
       delay(1000);
}
  else {
    digitalWrite(securityled,LOW);
    digitalWrite(buzzer, HIGH);
  }

    if(ignition == HIGH && (c == 1)){
    Serial.println();
    Serial.println("Hello");
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    lcd.clear();
    lcd.setCursor(0,0);              // Установка курсора в начало первой строки
    lcd.print("Hello");       // Набор текста на первой строке
    lcd.setCursor(0,1);              // Установка курсора в начало второй строки
    lcd.print("Temperature = ");       // Набор текста на второй строке
    lcd.print(bme.readTemperature());
    lcd.print(" *C");
    
    delay(2000);
    }
  
  
  
  // put your main code here, to run repeatedly:
  lightValue = analogRead(lightSensor);
  
  if(ignition == HIGH){
    if(lightValue > 135){
      digitalWrite(frontled1, HIGH);
      digitalWrite(frontled2, HIGH);
    } else {
      digitalWrite(frontled1, LOW);
      digitalWrite(frontled2, LOW);
    }
  }
  //Serial.println(lightValue);
  }

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
