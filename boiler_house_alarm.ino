#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <max6675.h>
#include <Wire.h>

Adafruit_SSD1306 display(4);

int swichBtn1=3;
int swichBtn2=4;
int fireLed = A1;
String Btn1Status, Btn2Status;
int TEMP_HIGH=0;  
int TEMP_LOW=0;
int temperature,sensorStatus;


int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;


void setup() {

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
    display.display();
    delay(2000);
    display.clearDisplay();
    pinMode(swichBtn1, INPUT);
    pinMode(swichBtn2, INPUT);
    TEMP_HIGH= EEPROM.read(10);
    TEMP_LOW= EEPROM.read(11);

}

void loop() {
  display.clearDisplay(); 
  delay(2000);
   sensorStatus = analogRead(fireLed);
   Btn1Status = digitalRead(swichBtn1);
   Btn2Status = digitalRead(swichBtn2);
   ToOledPrint(String(sensorStatus), "print", 0,0);
   ToOledPrint("UP " + String(TEMP_HIGH), "print", 0,10);
   ToOledPrint("LOW " + String(TEMP_LOW), "print", 0,20);
   temperature = thermocouple.readCelsius();
   if ( sensorStatus > 10){
    ToOledPrint("Alarm", "print",50 ,0);    
    }
   else {
    ToOledPrint("OK", "print",50 ,0);  
    }
   if(Btn1Status == "1" &&  Btn2Status == "1"){
     byte status;
     status = true;
     
      while (status)
      {
        delay(1000);
        Btn1Status = digitalRead(swichBtn1);
        Btn2Status = digitalRead(swichBtn2);
        display.clearDisplay();
        ToOledPrint("TMP UP " + String(TEMP_HIGH), "print",0 ,0);
        ToOledPrint("TMP LOW "+ String(TEMP_LOW), "print",0 ,10);
        
        if(Btn1Status == "1"){        

            TEMP_HIGH+=1;
            delay(100);
          }
        if(Btn2Status == "1"){        

            TEMP_LOW+=1;
            delay(100);
          }
        if(Btn1Status == "1" &&  Btn2Status == "1"){
           status = false;
           display.clearDisplay();
           ToOledPrint("Exit set   mode", "print",0 ,0);
           //Write to flash
           EEPROM.write(10, TEMP_HIGH);
           EEPROM.write(11, TEMP_LOW);
           return;
          }
         
        }
      
       
   }
   
   

}



void ToOledPrint(String text, String mode, int x, int y) {
  display.setTextSize(1);
  display.setCursor(x,y);
  if(mode == "print"){
    display.setTextColor(WHITE);
    display.println(text); 
    display.display(); 
  }
  else {
    
    display.setTextColor(BLACK);
    display.println(text);
    delay(300);
    display.display();
    display.setTextColor(WHITE);
    display.println(text);
    display.display();
    delay(100);
    

  }
  
}
