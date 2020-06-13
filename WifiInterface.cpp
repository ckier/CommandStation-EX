#include "WifiInterface.h"
#include "Config.h"
#include "DIAG.h"
#include "StringFormatter.h"

 
const char READY_SEARCH[]="\r\nready\r\n";
const char OK_SEARCH[]="\r\nOK\r\n";

bool WifiInterface::connected=false;
DCCEXParser  WifiInterface::parser;
byte WifiInterface::loopstate=0;
int WifiInterface::datalength=0;
int WifiInterface::connectionId;
byte WifiInterface::buffer[MAX_WIFI_BUFFER];
MemStream  WifiInterface::streamer(buffer,sizeof(buffer));

void WifiInterface::setup() {
  DIAG(F("\n++++++ Wifi Setup In Progress ++++++++\n"));
  connected=setup2();
  // TODO calloc the buffer and streamer and parser etc 
  DIAG(F("\n++++++ Wifi Setup %s ++++++++\n"), connected?"OK":"FAILED");
}

bool WifiInterface::setup2()
{
  Serial1.begin(WIFI_BAUD_RATE);    // initialize serial for ESP module
  
  delay(1000);

  StringFormatter::send(Serial1,F("AT+RST\r\n")); // reset module
  if (!checkForOK(10000,READY_SEARCH)) return false; 
  
  StringFormatter::send(Serial1,F("AT+CWMODE=1\r\n")); // configure as access point
  if (!checkForOK(10000,OK_SEARCH)) return false;
 
  StringFormatter::send(Serial1,F("AT+CWJAP=\"%s\",\"%s\"\r\n"),WIFI_SSID,WIFI_PASS);
  if (!checkForOK(20000,OK_SEARCH)) return false;
  
  StringFormatter::send(Serial1,F("AT+CIFSR\r\n")); // get ip address //192.168.4.1
  if (!checkForOK(10000,OK_SEARCH)) return false;
  
  StringFormatter::send(Serial1,F("AT+CIPMUX=1\r\n")); // configure for multiple connections
  if (!checkForOK(10000,OK_SEARCH)) return false;
  
  StringFormatter::send(Serial1,F("AT+CIPSERVER=1,%d\r\n"),WIFI_PORT); // turn on server on port 80
  if (!checkForOK(10000,OK_SEARCH)) return false;

  return true;
}

bool WifiInterface::checkForOK( const int timeout,char * search) {
  long int time = millis()+timeout;
  byte locator=0;
  DIAG(F("\nWifi setup Check:"),search);
  while( time > millis()) {
    while(Serial1.available()) {
      int ch=Serial1.read();
      Serial.write(ch);
      if (ch!=search[locator]) locator=0;
      if (ch==search[locator]){
        locator++;
        if (!search[locator]) {
          DIAG(F("\nOK after %dms\n"),millis()-time+timeout);
          return true;
        }
      }
    }
  }
  DIAG(F("\nTIMEOUT after %dms\n"),timeout);
  return false;
}

 
void WifiInterface::loop() {
    if (!connected) return; 

    // read anything into a buffer, collecting info on the way  
    while (loopstate!=99 && Serial1.available()) { 
      int ch=Serial1.read();
      switch (loopstate) {
           case 0:  // looking for +
                connectionId=0;
                streamer.flush();
                if (ch=='+') loopstate=1;
                break;
           case 1:  // Looking for I     
                loopstate= (ch=='I')?2:0;
                break; 
           case 2:  // Looking for P     
                loopstate= (ch=='P')?3:0;
                break; 
           case 3:  // Looking for D     
                loopstate= (ch=='D')?4:0;
                break; 
           case 4:  // Looking for ,     
                loopstate= (ch==',')?5:0;
                break; 
           case 5:  // reading connection id
                if (ch==',') loopstate=6;
                else connectionId=10*connectionId+(ch-'0');
                break;
           case 6: // reading for length
                if (ch==':') loopstate=(datalength==0)?99:7;  // 99 is getout without reading next char
                else datalength=datalength*10 + (ch-'0');
                break;
           case 7: // reading data 
                datalength--;
                if (datalength==0) loopstate=99;
                break;
        } // switch 
    } // while
    if (loopstate!=99) return; 
    // TODO remove > in data 
    streamer.write('\0');
    streamer.flush();  // reset write position to start of buffer
    // SIDE EFFECT WARNING::: 
    //  We know that parser will read the entire buffer before starting to write to it.
    //  Otherwise we would have to copy the buffer elsewhere and RAM is in short supply.

    // TODO ... tell parser that callbacks are diallowed because we dont want to handle the async 
    parser.parse(streamer,buffer+1);
    StringFormatter::send(Serial1,F("AT+CIPSEND=%d,%d\r\n>%s"),connectionId,streamer.available(),buffer);
    loopstate=0; 
    }
