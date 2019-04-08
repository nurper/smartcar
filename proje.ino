#define trigPin 2
#define echoPin 3
int lightSensor = A0;
int lightValue = 0;
int led1 = 1;
void setup() {
  // put your setup code here, to run once:
pinMode(trigPin, OUTPUT); // setting the output pin with fast output (will output signals in high speed)
pinMode(echoPin, INPUT); // setting the input pin with fast input (will get signals in high speed) 
}

void loop() {
  // put your main code here, to run repeatedly:
  lightValue = analogRead(lightSensor);
  if(lightValue<135){
    led1 = HIGH;
  }
}
