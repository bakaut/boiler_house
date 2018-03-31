#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <max6675.h>
#include <Wire.h>


int swichBtn1=9;
int swichBtn2=8;
int swichBtn3=7;
int swichBtn4=2;
String Btn1Status, Btn2Status,Btn3Status,Btn4Status;
int TEMP_HIGH=0;  
int TEMP_LOW=0;
int temperature;


int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;


int vccPin = 3;
int gndPin = 2;

int SendSmsCount = 0;

Adafruit_SSD1306 display(4);
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
    display.display();
    delay(1000);
    display.clearDisplay();
    pinMode(swichBtn1, INPUT);
    pinMode(swichBtn2, INPUT);
    pinMode(swichBtn3, INPUT);
    pinMode(swichBtn4, INPUT);
    TEMP_HIGH= EEPROM.read(10);
    TEMP_LOW= EEPROM.read(11);


}

void loop() {
  
   display.clearDisplay(); 
   delay(500);
   Btn1Status = digitalRead(swichBtn1);
   Btn2Status = digitalRead(swichBtn2);
   Btn3Status = digitalRead(swichBtn3);
   Btn4Status = digitalRead(swichBtn4);

   temperature = thermocouple.readCelsius();

   ToOledPrint(String(temperature), "print", 0,0);
   ToOledPrint("UP " + String(TEMP_HIGH), "print", 0,10);
   ToOledPrint("LOW " + String(TEMP_LOW), "print", 0,20);
   ToOledPrint(Btn1Status + Btn2Status  + Btn3Status +Btn4Status , "print", 50,20);
   
   if ( temperature > TEMP_HIGH){
     if (SendSmsCount < 3) {
       ToOledPrint("Alarm", "print",50 ,0);
       //Send sms and wait for second check
      //Send sms code
      //Wait for 1.3 hour
      //delay(5000000);
      delay(6000); //for test set 6 second
      SendSmsCount++;
     }
     else {
       ToOledPrint("SMS OFF", "print",50 ,0);
       SendSmsCount=10;
     }
  
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
        Btn3Status = digitalRead(swichBtn3);
        Btn4Status = digitalRead(swichBtn4);
        
        display.clearDisplay();
        ToOledPrint("TMP UP " + String(TEMP_HIGH), "print",0 ,0);
        ToOledPrint("TMP LOW "+ String(TEMP_LOW), "print",0 ,10);
        
        if(Btn3Status == "1"){        

            TEMP_HIGH+=1;
            delay(100);
          }
        if(Btn4Status == "1"){        

            TEMP_HIGH-=1;
            delay(100);
          }
        if(Btn1Status == "1"){        

            TEMP_LOW+=1;
            delay(100);
          }
        if(Btn2Status == "1"){        

            TEMP_LOW-=1;
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





