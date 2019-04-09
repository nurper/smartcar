#define trigPin 2
#define echoPin 3
int lightSensor = A0;
int lightValue = 0;
int security = HIGH; //turns off when opened with 'key'
int ignition = LOW; //tunrns on when pressed ignition button
int frontled1 = 4;
int frontled2 = 5;
int securityled = 6;//blinks when burglar ignites the car
void setup() {
  // put your setup code here, to run once:
pinMode(trigPin, OUTPUT); // setting the output pin with fast output (will output signals in high speed)
pinMode(echoPin, INPUT); // setting the input pin with fast input (will get signals in high speed) 
}

void loop() {
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
}
