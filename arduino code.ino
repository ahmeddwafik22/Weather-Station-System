#include<LiquidCrystal.h>
LiquidCrystal MyLCD(12,11,5,4,3,2);
void setup()
{
  MyLCD.begin(16,2);
  MyLCD.home();
  Serial.begin(9600); 
}

void loop() {
  if (Serial.available()) {
    String data_rcvd = Serial.readString();
    // read one byte from serial buffer and save to data_rcvd
    MyLCD.clear();
    Serial.println(data_rcvd);
    
    
    if (data_rcvd[0]=='*')
    {
      MyLCD.print(" temperuture = ");
    }
    else if (data_rcvd[0]=='/')
    {
       MyLCD.print("   hummidity = ");
    }
    else if (data_rcvd[0]=='-')
    {
       MyLCD.print(" pressure = ");
    }
    else if (data_rcvd[0]=='+')
    {
      MyLCD.print(" altitude = ");
    }

    MyLCD.setCursor(0, 1);
    data_rcvd=data_rcvd.substring(1);
    Serial.println(data_rcvd);
    MyLCD.print("    ");
    MyLCD.print(data_rcvd);
    delay(2000);
    
    
    
      
  }
     // send the char '0' to serial if button is not pressed.

}

