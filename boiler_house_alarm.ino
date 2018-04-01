#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <max6675.h>
#include <Wire.h>


#define OK 1
#define NOTOK 2
#define TIMEOUT 3
#define RST 2
#define A6board Serial
#define A6baud 115200
#define SERIALTIMEOUT 3000

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

unsigned long startTime = 0, endTime = 0;

String phone_no="89219614704";

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

    //Power on module via power pin/
    pinMode(3, OUTPUT);
    digitalWrite(3,HIGH);
    delay(2500);
    digitalWrite(3,LOW);

    //Start uart, start gsm module, wait for network register
    A6board.begin(A6baud);
    delay(300); 
    A6begin();

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
       ToOledPrint("SMS SEND", "print",80 ,20);
        //Send sms
       A6command("AT+CMGF=1", "OK", "yy", 10000, 2);
       A6command("AT+CMGS=\""+phone_no+"\"", ">", "yy", 10000, 1);
       delay(300);
       A6board.print(String( temperature));
       delay(300);
       A6board.println (char(26));//the ASCII code of the ctrl+z is 26
       //Wait for 1.3 hour
       //delay(5000000);
       //wait for second send sms. while waite dispay temperature
       startTime = millis();

       while ((millis() - startTime)  < 15000) {
         temperature = thermocouple.readCelsius();
         ToOledPrint(String(temperature), "print", 0,0);

       }
      
      SendSmsCount++;
     }
     else {
       ToOledPrint("SMS OFF", "print",80 ,0);
       //Wait for an hour. for test 15 seconds
       startTime = millis();
       while ((millis() - startTime) < 15000) {
         temperature = thermocouple.readCelsius();
         ToOledPrint(String(temperature), "print", 0,0);
         if ( temperature < TEMP_HIGH) {
           return;
         }

       }
      SendSmsCount = 0;
      ToOledPrint("SMS ON", "print",80 ,0);
      delay(2000);
     }
  
    }
   else {
    ToOledPrint("OK", "print",50 ,0);
    if (endTime - millis() < 10000) {
      ToOledPrint("SMS ON", "print",80 ,0);
      delay(2000);
      SendSmsCount=0;
}  
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

bool A6begin() {
  A6board.println("AT+CREG?");
  byte hi = A6waitFor("1,", "5,", 1500);  
  while ( hi != OK) {
    A6board.println("AT+CREG?");
    hi = A6waitFor("1,", "5,", 1500);
  }

  if (A6command("AT&F0", "OK", "yy", 5000, 2) == OK) {   
    if (A6command("ATE0", "OK", "yy", 5000, 2) == OK) {  
      if (A6command("AT+CMEE=2", "OK", "yy", 5000, 2) == OK) 
         return OK;  
      else return NOTOK;
    }
  }
}


byte A6waitFor(String response1, String response2, int timeOut) {
  unsigned long entry = millis();
  int count = 0;
  String reply = A6read();
  byte retVal = 99;
  do {
    reply = A6read();
    //if (reply != "") {
    //  Serial.print((millis() - entry));
    //  Serial.print(" ms ");
    //  Serial.println(reply);
    //}
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && millis() - entry < timeOut );
  if ((millis() - entry) >= timeOut) {
    retVal = TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = OK;
    else retVal = NOTOK;
  }
  return retVal;
}


byte A6command(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = NOTOK;
  byte count = 0;
  while (count < repetitions && returnValue != OK) {
    A6board.println(command);
    //Serial.print("Command: ");
    //Serial.println(command);
    if (A6waitFor(response1, response2, timeOut) == OK) {
      returnValue = OK;
    } else returnValue = NOTOK;
    count++;
  }
  return returnValue;
}

String A6read() {
  String reply = "";
  if (A6board.available())  {
    reply = A6board.readString();
  }
  return reply;
} 
