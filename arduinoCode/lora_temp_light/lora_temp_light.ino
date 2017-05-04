
#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include <math.h>

SoftwareSerial mySerial(10, 11); // RX, TX

int ThermistorPin = 2;
int Vo;
float R1 = 1000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
bool light = 1;
int sensor_value;


//create an instance of the rn2xx3 library,
//giving the software serial as port to use
rn2xx3 myLora(mySerial);

//connections//
//////////////

//ledselect//
//pin 13 -- lora sending led -- ledSelect(1)
//pin 9 --- light on/off led -- ledSelect(2)

 
// the setup routine runs once when you press reset:
void setup()
{
  //output LED pin
  pinMode(13, OUTPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");

  initialize_radio();

  //transmit a startup message
  myLora.tx("TTN Mapper on TTN Enschede node");

  delay(2000);
}

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;

  //ABP: initABP(String addr, String AppSKey, String NwkSKey);
  join_result = myLora.initABP("26011CB1", "E5D914EA2C594BE88C28C50BEF7671E4", "8ADF8E35D4C22466C1BC7C5C557F6945");

  //OTAA: initOTAA(String AppEUI, String AppKey);
  //join_result = myLora.initOTAA("70B3D57ED00001A6", "A23C96EE13804963F8C2BD6285448198");

  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}

// the loop routine runs over and over again forever:
void loop()
{
    sensor_value = analogRead(A4); //not being called in measure_Light() because of better performance.
    measure_Temp();
    measure_Light();
    
    Serial.println("TXing");
    String data_Temp = "|TMP" + String( Tc );
    String data_Light = "|LT." + String(light);
    myLora.tx( data_Temp + data_Light); //one byte, blocking function
    Serial.print(Tc);
    Serial.println(" C"); 
    led_on(1);
    delay(200);
    led_off(1);
}

void measure_Temp()
{
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15; 
}

void measure_Light()
{
   if (sensor_value < 150)// the point at which the state of LEDs change 
    { 
      led_on(2);
      light = false;
      Serial.println("light_off");
    }
  else
    {
      led_off(2);
      light = true;
      Serial.println("light_on");
    } 
}

int led_on( int ledSelect)
{
  if (ledSelect ==  1){
    digitalWrite(13, 1);
  }
  
  if (ledSelect == 2){
    digitalWrite(9, 1);
  }
}

int led_off(int ledSelect)
{
  if (ledSelect ==  1){
    digitalWrite(13, 0);
  }
  
  if (ledSelect == 2){
    digitalWrite(9, 0);
  }
  
}


