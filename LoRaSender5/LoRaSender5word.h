
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
int msgCount = 10000001;
int interval = 2000;          // interval between sends
long lastSendTime = 0;        // time of last packet send
int CR;
int SF;
int prm = 53;   //max prm is 53
int BW = 125E3;
int TxP = 17;	//configurable transmission power

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

  LoRa.setTxPower(TxP);
  LoRa.setSignalBandwidth(BW);
  CR = prm/6;
  LoRa.setCodingRate4(CR); 
  SF = (prm%6)+7;
  LoRa.setSpreadingFactor(SF);       // ranges from 6-12,default 7 see API docs
  Serial.println(String(SF) + " " + String(CR));
  LoRa.noCrc();
  LoRa.explicitHeaderMode();
  Serial.println("LoRa init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    String message = "";   // send a message
    message += msgCount;
    Serial.println(String(BW)+ " " + String(SF) + " " + String(CR) + " " + message); //change to println and remove the space at the end of the string if not measuring air time
    sendMessage(message);
    lastSendTime = millis();            // timestamp the message
    //interval = random(2000) + 1000;    // 2-3 seconds
    msgCount++;
    
  }
  if (msgCount>10000500){
    LoRa.setTxPower(17);
      sendMessage("changeRC");
      delay(2000);//in case the relay wasn't functioning the message should reach the receiver
    prm--;
    CR=prm/6;
    LoRa.setCodingRate4(CR);
    SF=(prm%6)+7;    
    LoRa.setSpreadingFactor(SF);
    int i=0;
    while (i<20){
      sendMessage("changePM");
      Serial.println("change PM " + String(SF) + " " + String(CR) + " " + String(prm));
      delay(1500);    //send changePM 20 times to change the relay
      i++;
    }
    if (SF<10)
    interval = 4000;
    else
    interval = 2000;
    delay (45000);
    LoRa.setTxPower(TxP);
    
    msgCount = 10000001;
  }// to keep size 8 bytes
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  //Serial.print(String(micros()) + " ");//Serial.println("started");
  LoRa.endPacket();                     // finish packet and send it
  //Serial.println(String(micros()));//Serial.println("ended");
  //msgCount++;                           // increment message ID
  display.clear();
  display.drawString(0,0, outgoing);
  display.display();
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}
