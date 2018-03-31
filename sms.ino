
#define OK 1
#define NOTOK 2
#define TIMEOUT 3
#define RST 2
#define A6board Serial
#define A6baud 115200
#define SERIALTIMEOUT 3000

String phone_no="89219614704";

void setup() {

    //Power on module via power pin/
    pinMode(5, OUTPUT);
    digitalWrite(5,HIGH);
    delay(2500);
    digitalWrite(5,LOW);

    //Start uart, start gsm module, wait for network register
    A6board.begin(A6baud);
    delay(300); 
    A6begin();

    //Send sms
    A6command("AT+CMGF=1", "OK", "yy", 10000, 2);
    A6command("AT+CMGS=\""+phone_no+"\"", ">", "yy", 10000, 1);
    delay(300);
    A6board.print("GSM A6 test message!");
    delay(300);
    A6board.println (char(26));//the ASCII code of the ctrl+z is 26

}

void loop()
{

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
