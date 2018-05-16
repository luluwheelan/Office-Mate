/*
        Name of the project: Office Mate
        Description: Set the time between 2-62 minutes for remind drink water,
                     the moisture sensor detects the moisture level of the soil and remind to water the plant.
                     Data is sent to Thingspeak.
        Date: April 17, 2018
        Author: Lu Chen
        Requirements:  Arduino with 321Maker Shield, 1602 Serial LCD, soil moisture detector
        Serial LCD library: https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/NewliquidCrystal_1.3.4.zip      
*/
//Load libraries
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

//Define address for the Serial LCD display.
#define I2C_ADDR  0x27 
#define BACKLIGHT_PIN  3
//Thingspeak Server
#define DST_IP "api.thingspeak.com"  

//ThingSpeak Write API key.
String writeAPIKey = "QATTHCX7MP19N6UT";  
//Moisture input pin A3, define dry soil value and wet soil value
int Moisture = A3;
//BuzzerPin is connected to pin 5.
int buzzer = 5;
//The rotation sensor is A0.
int rotationPin = A0; 

//Find the value of the dry soil, and set it as a waterPoint; 
//All the plants have different water level,
//the soil for this project wet value(after watering) is 260, dry value(very dry) is 800
//Set the waterPOint 500
int waterPoint = 500;
//sensorValue store the value from the moisture detector
int sensorValue;
//Time is for how long for remind drink water.
int time=0;     
//Update every Minute       
unsigned int interval = 60000;  
//Used to hold time variables.
long int now=0, previous=-50000; 
//Initialise the Serial LCD.
LiquidCrystal_I2C lcd(I2C_ADDR, 2,1,0,4,5,6,7); //These pin numbers are hard coded in on the serial backpack board.

void setup()
 {
    pinMode(buzzer, OUTPUT);  //Setup buzzer pin as an output pin.
    lcd.begin (16,2);     //Initalize the LCD.
    lcd.setBacklightPin(3,POSITIVE);//Setup the backlight.
    lcd.setBacklight(HIGH); //Switch on the backlight.

    //Read the value from the rotation pin and store it in the time variable.
    //this equation will turn the value from rotation pin (0-1024) to 2-62(as minute);
    time = analogRead(rotationPin)/17+2; 
     
    lcd.clear();
    lcd.setCursor(0,0);   //goto first column  and first line (0,0), print message
    lcd.print("Have some water");   
    lcd.setCursor(0,1);   //goto first column and second line 
    String string = (String)"every " + time + " minutes";
    lcd.print(string);   //Print at cursor Location 
    delay(5000);   
    
    pinMode(Moisture, INPUT);
    Serial.begin(9600);

    Serial.begin(9600);  // Open serial Connection to Computer
  Serial1.begin(115200);  // Open serial Connection to ESP8266 
  
    while (!Serial) {// wait for serial port to connect. Needed for Leonardo only
  }  
  
  Serial.println("Thingspeak Demo");
  Serial1.println("AT+RST");  //Issue Reset Command
  Serial.println("AT+RST");
  delay(1000);

  //DEBUG LOOP- display ESP output to serial Monitor.
  while (Serial1.available()) { 
  Serial.println(Serial1.read());
  }
  Serial.println("AT+CWMODE=1"); 
  Serial1.println("AT+CWMODE=1");  //Set single client mode.
  delay(8000);

  Serial.println("AT+CIFSR");
  Serial1.println("AT+CIFSR"); //Display IP Information
  delay(1000);
  //DEBUG LOOP- display ESP output to serial Monitor.
  while (Serial1.available()) {  
  Serial.println(Serial1.read());
  }
  
  Serial.println("AT+CIPMUX=0");  
  Serial1.println("AT+CIPMUX=0");  //Sets up Single connection mode.
  delay(1000);

  //DEBUG LOOP- display ESP output to serial Monitor.
  while (Serial1.available()) { 
    Serial.write(Serial1.read());
  }
  delay(1000);
 }
 
void loop(){
   //read the input on analog pin 3 every second:
  sensorValue = analogRead(Moisture);
  delay(1000);
  now=millis(); //Get the current time.
  if(now - previous >= interval){  //Check to see if it's time to run.
  previous = now; 
  Serial.println("Running Update");
  updateThingSpeak();
  }
  //If the soil is wet, the buzzer will make sound and LCD will display the drink water message every time.
  if(sensorValue < waterPoint){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Just Feel");
    lcd.setCursor(0, 1);
    lcd.print("       AWESOME ");
    delay(time*60000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("It's time to");
    lcd.setCursor(0, 1);
    lcd.print("have some water!");
    tone(buzzer, 2000,100);   //Play a tone of 2000Hz for a duration of 100 milliseconds on buzzer.
    delay(10000);//This drink water message will stay for 10 seconds.
    }else{
      //Message will display and buzzer will make sound when the soil is dry
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("I am thisty...");
      lcd.setCursor(0, 1);
      lcd.print("Let's DRINK!");
      tone(buzzer, 2000,100);   //Play a tone of 2000Hz for a duration of 100 milliseconds on pin D6.
      delay(10000);
      }
}
//Code bellow is simply copy from class
void updateThingSpeak(){ 
   int moisture = analogRead(Moisture);//Read the Moisture
  
   //Builds the connection string for the ESP8266
    String cmd = "AT+CIPSTART=\"TCP\",\"";
    cmd += DST_IP;
    cmd += "\",80";
    Serial1.println(cmd);  //Run the command
    Serial.println(cmd);    //Print this to the debug window
    delay(1000);

    //DEBUG LOOP- display ESP output to serial Monitor.
    while (Serial1.available()) {
    Serial.write(Serial1.read());
    } 

   //I NEED TO UPDATE THIS IF STATEMENT TO MAKE SURE CONNECTION WORKED
  //if (client.connect(thingSpeakAddress, 80)){         
    String httpcmd="GET /update?api_key=";
    httpcmd += writeAPIKey+"&field1=";
    httpcmd += moisture;
    httpcmd += " HTTP/1.1\r\n";
    httpcmd += "Host: api.thingspeak.com\n";
    httpcmd += "Connection: close\r\n\r\n";
    
    Serial.print("AT+CIPSEND=");
    Serial.println(httpcmd.length());
  
    Serial1.print("AT+CIPSEND=");
    Serial1.println(httpcmd.length());
    delay(1000);

    Serial.print(">");
    Serial1.println(httpcmd);
    Serial.println(httpcmd);
    delay(3000);

    //DEBUG LOOP- display ESP output to serial Monitor.
     while (Serial1.available()) {
      Serial.write(Serial1.read());
      }
   
    Serial.println("AT+CIPCLOSE");
    Serial1.println("AT+CIPCLOSE"); //Close the Web Connection
}


    
