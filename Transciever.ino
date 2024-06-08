
#include <LoRa.h>
#define SS D8 //15
#define RST D0 //16
#define DIO0 D4 //2
String data = "";
void setup()
{
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Sender Host");
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    delay(100);
    while (1);
  }
}
void loop()
{
  if(Serial.available()){
    data = Serial.readString();
      // Serial.print("Sending Data: ");
      // Serial.println(data);
      LoRa.beginPacket();
      LoRa.print(data);
      LoRa.endPacket();

  }
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Serial.print("Receiving Data: ");
    while (LoRa.available()) {
      String data = LoRa.readString();
      Serial.println(data);
    }
  }
  // delay(100);

  // delay(3000);
}
