const int led=13; // variable which stores pin number

void setup() 
{
  pinMode(led, OUTPUT);  //configures pin 3 as OUTPUT
}

void loop() 
{
   int sensor_value = analogRead(A4);
  if (sensor_value < 150)// the point at which the state of LEDs change 
    { 
      digitalWrite(led, HIGH);  //sets LEDs ON
    }
  else
    {
      digitalWrite(led,LOW);  //Sets LEDs OFF
    }

}
