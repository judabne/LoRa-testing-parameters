//code by Judy Abi Nehme
//This was intended to be used half way between the sender and receiver.
//It should send the change signal again in case the receiver didn't receive it from the sender.
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <SSD1306.h>

const int csPin = 18;          // LoRa radio chip select
const int resetPin = 14;       // LoRa radio reset
const int irqPin = 26;         // change for your board; must be a hardware interrupt pin

// Screen configuration
#define OLED_I2C_ADDR 0x3C
#define OLED_RESET 16
#define OLED_SDA 21
#define OLED_SCL 22
SSD1306 display (OLED_I2C_ADDR, OLED_SDA, OLED_SCL);

byte msgCount = 0;            // count of outgoing messages
int interval = 2000;          // interval between sends
long lastSendTime = 0;        // time of last packet send
int CR;
int SF;
int prm = 52;
long BW = 250E3;

int counter=0;
int rssi = 0;

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);

  SPI.begin(5,19,27,18);
  //screen initialization
  pinMode(OLED_RESET, OUTPUT);
  digitalWrite(OLED_RESET, LOW);
  delay(50);
  digitalWrite(OLED_RESET, HIGH);
  display.init ();
  display.flipScreenVertically ();
  display.setFont (ArialMT_Plain_16);
  display.setTextAlignment (TEXT_ALIGN_LEFT);
  display.display ();
  
  Serial.println("LoRa Duplex - Set spreading factor");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

  if (!LoRa.begin(866E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  LoRa.setTxPower(17);
  LoRa.setSignalBandwidth(BW);
  CR=prm/6;
  LoRa.setCodingRate4(CR);
  SF=(prm%6)+7;
  LoRa.setSpreadingFactor(SF);            //prm from 30 to 53. prm = CR*6+(SF-7)
  LoRa.noCrc();
  LoRa.explicitHeaderMode();
  Serial.println("LoRa init succeeded.");
}

void loop() {
  /*if (millis() - lastSendTime > interval) {
    String message = "HeLoRa World! ";   // send a message
    message += msgCount;
    sendMessage(message);
    Serial.println("Sending " + message);
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 1000;    // 2-3 seconds
    msgCount++;
    
  }*/
  //disabled sender

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //Serial.println("I am here");
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  //msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
  //Serial.println("Packet Size: " + String(packetSize));
  // read packet header bytes:
  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  Serial.print(String(BW) + " " + String(SF) + +" " + String (CR) + " "); //print parameters
  Serial.print(incoming);
  rssi = LoRa.packetRssi();
  Serial.print(" " + String(rssi)); // print rssi
  Serial.print(" " + String(LoRa.packetSnr())); //print snr
  //Serial.println();

  display.clear();
  display.drawString(0,0, incoming);
  display.drawString(0,20, "SF: "); display.drawString(70,20,String(SF));
  //increase counter of received packages
  counter++;
  display.drawString(0,40, "CR: ");display.drawString(70,40,String(CR));
  display.display();  
  Serial.println(" Counter: " + String(counter));

  if (incoming=="changePM"){ //change parameters
    prm++;
    CR=prm/6;
    LoRa.setCodingRate4(CR);
    SF=(prm%6)+7;
    LoRa.setSpreadingFactor(SF);
    Serial.println("Spreading factor " + String(SF));
    int i = 0;
    while (i<20){
      sendMessage("changeRC"); //change receiver
      Serial.println("changeRC");
      delay (1500);
      i++;
    }
    prm = prm -2;
    CR=prm/6;
    LoRa.setCodingRate4(CR);
    SF=(prm%6)+7;
    LoRa.setSpreadingFactor(SF);
    Serial.println("SF " + String(SF) + " CR " + String(CR) + "Waiting...");
    //CR--;
    //LoRa.setCodingRate4(CR);
    //Serial.println("Coding rate " + String(CR)); 
  }
}
