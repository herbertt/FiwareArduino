#define BUFFER 1024 

#define SSID   "TestArduino" 
#define PASS   "12345678"
#define DST_IP "123.456.789.10" 

char buffer[BUFFER]; 
String cmd;  


#define DEBUG Serial 

char reply[500];
const bool printReply = true;
const char line[] = "-----\n\r";
#include <AltSoftSerial.h>
AltSoftSerial ESP8266(8,9);

char OK[] = "OK\r\n";

//=====================================================================
void setup() {
  delay(1000);
  ESP8266.begin(9600); // Start ESP8266 comms 
  Serial.begin(9600); // Start seriam monitor comms for debug messages
  DEBUG.println("Initialising...");
  initESP8266(); // Initialise the ESP8266 module
}

//=====================================================================
void waitForResponse(int wait)
{
    int tempPos = 0;
    long int time = millis();
    while( (time + wait) > millis())
    {
        while(ESP8266.available())
        {
            char c = ESP8266.read(); 
            if (tempPos < 500) { reply[tempPos] = c; tempPos++;   }
        }
        reply[tempPos] = 0;
    } 
 
    if (printReply) { Serial.println( reply );  Serial.println(line);     
    }
}

//=====================================================================
void initESP8266() { 
  ESP8266.println("AT+RST");
  waitForResponse(10000);
   
  ESP8266.println("AT+CWMODE=1");
  waitForResponse(5000);  
  
  cmd = "AT+CWJAP=\""; 
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  ESP8266.println(cmd);  
  waitForResponse(10000);
  
  ESP8266.println("AT+CIPMUX=0"); 
  waitForResponse(5000); 
  
  ESP8266.println("AT+CIPMODE=0"); 
  waitForResponse(5000);  
  
  ESP8266.println("AT+CIFSR");
  waitForResponse(5000);
  
  DEBUG.println("============================");
  delay(1000);
}


//=====================================================================
void loop() { 
  cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",1026";
  ESP8266.println(cmd);  
  waitForResponse(3000);

  cmd =  "POST /v1/contextEntities/Room1 HTTP/1.1\r\n";
  cmd += "Host: [define DST_IP ]:1026\r\n"; 
  cmd += "Content-Type: application/json\r\n"; 
  cmd += "Content-length: 121\r\n\r\n";
  cmd += "{\"type\":\"Room\",\"isPattern\":\"false\",\"id\":\"Room1\",\"attributes\": [{\"name\":\"temperature\",\"type\":\"centigrade\",\"value\":\"";
  cmd += String(analogRead(0));
  cmd += "\"}]}";
     
  ESP8266.print("AT+CIPSEND=");
  ESP8266.println(cmd.length());   
  waitForResponse(3000);
  ESP8266.println(cmd);
  waitForResponse(3000); 
  ESP8266.println("AT+CIPCLOSE"); 
  delay(5000); 
}
