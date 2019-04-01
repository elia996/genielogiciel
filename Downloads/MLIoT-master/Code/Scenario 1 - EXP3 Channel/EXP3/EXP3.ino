#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;
float frequency1 = 868.1;
float frequency3 = 868.3;
float frequency5 = 868.5;

void setup(){
  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
    rf95.setCodingRate4(5);
}

int myPacket=1001;
double gamma=0.9;
double weights[3] = {1.0, 1.0, 1.0};
double probabilities[3];
uint8_t state;

double sumArray(double arr[]){
  double res=0.0;
  for(int i=0 ; i<3 ; ++i){
    res+= arr[i];
  }
  return res;
}

void updateProbabilities(double arr[], double gam, int k){
  for(int i=0 ; i<3 ; ++i){
    probabilities[i] = (1-gam)*arr[i]/sumArray(arr) + gam/k;
  }
}

void setFrequency(){
  int myRand = random(1,101);
  //Serial.println(myRand);
  if( myRand<=probabilities[0]*100){
    state=0;
    rf95.setFrequency(frequency1);
  }
  else if(probabilities[0]*100<myRand && myRand<=probabilities[0]*100+probabilities[1]*100){
    state=1;
    rf95.setFrequency(frequency3);
  }
  else{
    state=2;
    rf95.setFrequency(frequency5);
  }
}

bool ackReceived=false;
bool firstLoop=true;
bool lastArrived1=false;
bool lastArrived2=false;
bool lastArrived3=false;
int packetLimit=2000;

void loop()
{
  while(!lastArrived3){
  if(firstLoop){
    //Serial.println("Waiting 10 seconds before starting...");
    delay(20000);
    firstLoop=false;
  }
  Serial.print(String(myPacket)+",");
  updateProbabilities(weights,gamma,3);
  for(int i=0 ; i<2 ; ++i){
    Serial.print(String(probabilities[i]) +",");
  }
  Serial.print(String(probabilities[2])+",");
    for(int i=0 ; i<2 ; ++i){
    Serial.print(String(weights[i]) +"||");
  }
  Serial.println(String(weights[2]));
  setFrequency();
  String myData = String(myPacket) + ", I am currently learning!";
  uint8_t data[31];
  myData.toCharArray(data, 31);

  //Serial.println("Sending data " + String(myPacket));
  
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();

  long timeDelay=millis() + 100;
  while(millis()<=timeDelay){
      if (rf95.available()){
         uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
         uint8_t len = sizeof(buf);
         if (rf95.recv(buf, &len)){
             if((*(char*) buf) == 'c'){
                //Serial.println("Client received acknowledgement");
                ackReceived=true;
                if(myPacket>=packetLimit && state==0) lastArrived1=true;
                if(lastArrived1){
                  weights[0]=0;
                  weights[1]=1;
                  weights[2]=0;
                }
                if(myPacket>=packetLimit && state==1 && lastArrived1) lastArrived2=true;
                if(lastArrived2){
                  weights[0]=0;
                  weights[1]=0;
                  weights[2]=1;
                }
                if(myPacket>=packetLimit && state==2 && lastArrived2) lastArrived3=true;
             }
         }
      }
  }
  //Serial.println(lastArrived1);
  //Serial.println(lastArrived2);
  //Serial.println(lastArrived3);
  if(ackReceived){
//    float a = weights[state]*exp(gamma/(3*probabilities[state]));
//    a = round(a);
//    a = fmod(a,10000);
//    Serial.println(round(a));
    //long b = a%10000;
    //float c = float(b);
    //weights[state]=c;
    float newWeight = weights[state]*exp(gamma/(3*probabilities[state]));
    if(newWeight>=100000000){
      weights[0] = (weights[0] / 1000000)+1;
      weights[1] = (weights[1]/1000000)+1;
      weights[2]=(weights[2]/1000000)+1;
    }
    else{
      weights[state] = fmod(weights[state]*exp(gamma/(3*probabilities[state])),100000000);
    }
  }
  myPacket++;
  ackReceived=false;
  delay(250);
  }
  myPacket=1001;
  weights[0]=1.0;
  weights[1]=1.0;
  weights[2]=1.0;

//  weights = {1.0, 1.0, 1.0};
  firstLoop=true;
  lastArrived1=false;
  lastArrived2=false;
  lastArrived3=false;
}


