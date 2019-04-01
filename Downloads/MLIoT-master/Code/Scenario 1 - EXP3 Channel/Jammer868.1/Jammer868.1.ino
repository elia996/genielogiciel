#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;
float frequency = 868.1;

void setup()
{
  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  rf95.setFrequency(frequency);
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);
}

float nextTime(float rateParameter){
  float randNumber = float (random(1,101)/float(100.0));
  return -logf(randNumber) / rateParameter;
}

void loop()
{
  uint8_t data[] = "I'm jamming 868.1";
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  delay(nextTime(0.001));
}
