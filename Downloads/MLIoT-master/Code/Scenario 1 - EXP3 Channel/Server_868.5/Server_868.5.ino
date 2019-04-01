#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;
int led = 8;
float frequency = 868.5;

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

int myCtr=0;

bool stringIsNumber(const char* str){
  for(int i=0; i<strlen(str);++i){
    if(!(str[i] >= '0' && str[i] <= '9')) return false;
  }
  return true;
}

char *substring(char *string, int position, int length)
{
   char *pointer;
   int c;
 
   pointer = (char*) malloc(length+1);
   
   if (pointer == NULL)
   {
      printf("Unable to allocate memory.\n");
      exit(1);
   }
 
   for (c = 0 ; c < length ; c++)
   {
      *(pointer+c) = *(string+position-1);      
      string++;  
   }
 
   *(pointer+c) = '\0';
 
   return pointer;
}

bool charIsNumber(char str){
  if(!(str >= '0' && str <= '9')) return false;
  return true;
}

void loop()
{
  if (rf95.available())
  {
    // Do not overload this part, should mainly be waiting for new messages
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      if(charIsNumber((char) ((char*) buf)[0])){
        myCtr++;
//        Serial.print("Got message: ");
//        Serial.print((char*)buf); 
//        Serial.println(", Total packets arrived: "+String(myCtr));
        Serial.println(String()+((char*) buf) + ","+ String(myCtr));
        uint8_t data[] = "c";
//        Serial.println("sending ack");
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
      }
    }
    else
    {
      Serial.println("recv failed");
    }
    
  }
}
