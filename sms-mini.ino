
String phone_no="89219614704";

void setup() {
    Serial.begin(9600);
    delay(300); 

    Serial.println("AT+CMGF=1");    
    delay(2000);
    Serial.print("AT+CMGS=\""+phone_no+"\"");
    //Serial.write(0x22);
    //Serial.write(0x0D);  // hex equivalent of Carraige return    
    //Serial.write(0x0A);  // hex equivalent of newline
    delay(2000);
    Serial.print("GSM A6 test message!");
    delay(500);
    Serial.println (char(26));//the ASCII code of the ctrl+z is 26

}

void loop()
{

}
