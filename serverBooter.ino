/*
  serverBooter

  built for Adafruit Feather M0 + Ethernet

  send a wake-on-lan packet to a machine periodically (default every 60 seconds)

  used to wake up server after power outage - when feather gets powered up, it will attempt to power on the server as well

  based on https://www.megunolink.com/download/libraries/wake-on-lan/ but updated for Ethernet library 2.0.1 (as well as my own changes for production mode)

  set testMode = true to run in console-based mode (waits for user input)
  set testMode = false to run as intended, send packet every XX seconds (defined as interval below)
*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

EthernetUDP Udp;

// Make up a mac Address and an IP address. Both should be globally unique or
// at least unique on the local network
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// create ip/dns for this device
// since its going to be on before the server, we need to set it up without dhcp
IPAddress ip(192, 168, 1, 149);
IPAddress myDns(8, 8, 4, 4);

unsigned int localPort = 8888;      // local port to listen on

// The machine to wake up. WOL should already be configured for the target machine. 
// hal9000 eth1 00:15:17:d6:35:8f
// hal9000 eth0 00:15:17:d6:35:8e
static byte g_TargetMacAddress[] = {0x00,0x15,0x17,0xd6,0x35,0x8f};
static byte g_TargetMacAddress2[] = {0x00,0x15,0x17,0xd6,0x35,0x8e};

// set interval for packet to be sent - figure 2 minutes is enough time not to boot on intermittent power return
int interval = 120;

// run in test mode at first - wait for console, will send command when console recieves "w"
// disable test mode to run continuously - when powered, send packet every x seconds
bool testMode = false;

int SendWOLMagicPacket(byte * pMacAddress)
{
  // The magic packet data sent to wake the remote machine. Target machine's
  // MAC address will be composited in here.
  const int nMagicPacketLength = 102;
  byte abyTargetIPAddress[] = { 255, 255, 255, 255 }; // don't seem to need a real ip address.
  const int nWOLPort = 9;
 
  // Compose magic packet to wake remote machine.
  byte magicPacket[102];
  int i,c1,j=0;
 
  for(i = 0; i < 6; i++,j++){
      magicPacket[j] = 0xFF;
  }
  for(i = 0; i < 16; i++){
      for( c1 = 0; c1 < 6; c1++,j++)
        magicPacket[j] = pMacAddress[c1];
  }
  
  if (Udp.begin(localPort) !=1) {
    Serial.println("failed creating udp port");
    return 1;
  }

  Serial.println("sending packet... ");

  if (Udp.beginPacket(abyTargetIPAddress, nWOLPort)!=1) {
    Serial.println("error beginning udp packet");
    return 2;
  }
  Udp.write(magicPacket, 102);

  if (Udp.endPacket() !=1) {
    Serial.println("error ending udp packet");
    return 3;
  }

  Serial.println("...sent.");
  return 0;
}
 
void setup() {
  byte t = 0;
  while(!Serial && t++ < 10) delay(1000); // curlies not needed for a one liner
  Serial.begin (115200); // well, that's the speed I use here

  Serial.println("beginning serverBooter setup...");
  
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields

  // try to configure using IP address instead of DHCP:
  Ethernet.begin(mac, ip, myDns);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  
  Serial.println("setup complete. ");

}

void loop() {
  if (testMode) {
    // When 'w' is received, send a magic packet to wake the remote machine. 
    if(Serial.available() > 0) {
      Serial.println("running in test mode, type w to send wol packet");
      if(Serial.read() == 'w') 
      {
        if (SendWOLMagicPacket(g_TargetMacAddress) == 0) {
          Serial.println("Magic packet sent");
        }
      }
    }
  } else {
    Serial.println("serverBooter running in stand-alone mode, sending packet after delay");
    while (true) {
      delay(interval*1000);
      Serial.println("sending WOL packet(s)");
      if (SendWOLMagicPacket(g_TargetMacAddress) == 0) {
        Serial.println("Magic packet sent");
      }
      if (SendWOLMagicPacket(g_TargetMacAddress2) == 0) {
        Serial.println("Magic packet sent");
      }
    }
  }

}
