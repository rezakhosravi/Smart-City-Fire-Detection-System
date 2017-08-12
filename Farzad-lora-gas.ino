/*
    This is a demo to test gas library
    This code is running on Xadow-mainboard, and the I2C slave is Xadow-gas
    There is a ATmega168PA on Xadow-gas, it get sensors output and feed back to master.
    the data is raw ADC value, algorithm should be realized on master.
    please feel free to write email to me if there is any question
    Jacky Zhang, Embedded Software Engineer
    qi.zhang@seeed.cc
    17,mar,2015
*/

#include <Wire.h>
#include "MutichannelGasSensor.h"
#include <SPI.h>              // include libraries
#include <LoRa.h>

const int csPin = 6;          // LoRa radio chip select
const int resetPin = 5;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0x47;     // address of this device
byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends



void setup()
{
    Serial.begin(115200);  // start serial for output
    Serial.println("power on!");
    gas.begin(0x04);//the default I2C address of the slave is 0x04
    gas.powerOn();
//    Serial.print("Firmware Version = ");
//    Serial.println(gas.getVersion());

// override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(915E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");


}

void loop()
{
    float c0;
    float c1;
    float c2;
    float c3;
    float c4;
    float c5;
    float c6;
    float c7;

/*
//    c0 = gas.measure_NH3();
//    Serial.print("The concentration of NH3 is ");
    if(c>=0) Serial.print(c);
    else Serial.print("invalid");
//    Serial.println(" ppm");
    
    c1 = gas.measure_CO();
//    Serial.print("The concentration of CO is ");
    Serial.print(",");
    if(c>=0) Serial.print(c);
    
    else Serial.print("invalid");
//    Serial.println(" ppm");

    c2 = gas.measure_NO2();
//    Serial.print("The concentration of NO2 is ");
    Serial.print(",");
    if(c>=0) Serial.print(c);
    else Serial.print("invalid");
//    Serial.println(" ppm");

    c3 = gas.measure_C3H8();
//    Serial.print("The concentration of C3H8 is ");
    Serial.print(",");
    if(c>=0) Serial.print(c);
    else Serial.print("invalid");
//    Serial.println(" ppm");

    c4 = gas.measure_C4H10();
//    Serial.print("The concentration of C4H10 is ");
    Serial.print(",");
    if(c>=0) Serial.print(c);
    else Serial.print("invalid");
//    Serial.println(" ppm");

    c5 = gas.measure_CH4();
//    Serial.print("The concentration of CH4 is ");
    Serial.print(",");
    if(c>=0) Serial.print(c);
    else Serial.print("invalid");
 //   Serial.println(" ppm");

    c6 = gas.measure_H2();
 //   Serial.print("The concentration of H2 is ");
    Serial.print(",");
    if(c>=0) Serial.print(c);
    else Serial.print("invalid");
 //   Serial.println(" ppm");

    c7 = gas.measure_C2H5OH();
//    Serial.print("The concentration of C2H5OH is ");
    Serial.print(",");
    if(c>=0) Serial.println(c);
    else Serial.print("invalid");
//    Serial.println(" ppm");

//    delay(1000);
*/
if (millis() - lastSendTime > interval) {
    c0 = gas.measure_NH3();
    c1 = gas.measure_CO();
    c2 = gas.measure_NO2();
    c3 = gas.measure_C3H8();
    c4 = gas.measure_C4H10();
    c5 = gas.measure_CH4();
    c6 = gas.measure_H2();
    c7 = gas.measure_C2H5OH();
    String message = String (c0) + "," + String (c1) + "," +String (c2) + "," + String (c3)+ "," + String (c4) + "," +String (c5) + "," + String (c6) + "," + String (c7) ;   // send a message
    sendMessage(message);
    Serial.println("Sending " + message);
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 1000;    // 2-3 seconds
  }

  // parse for a packet, and call onReceive with the result:
 // onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
//  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}

//}
