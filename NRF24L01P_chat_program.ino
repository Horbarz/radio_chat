#include<SPI.h>
#include"nRF24L01.h"
#include"RF24.h"
#include"printf.h"
RF24 radio(7, 8);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
bool go_to_tx_mode = false;
char input[32] = "";



//ping variables
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000;
char txNum = '0';
char dataToSend[10] = "Message 0";

void setup() {
  Serial.begin(57600);
  printf_begin();
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);
  radio.startListening();
  radio.printDetails();
  // put your setup code here, to run once:
}
void loop() {
  if (Serial.available())
  { 
    int len = Serial.readBytesUntil('\n', input, 31);
    input[len] = '\0';
    go_to_tx_mode = true;
  }
  if(strcmp(input,"ping")== 0){
    //Serial.println("Hello");
    //delay(100);
    pingRadio();
    }else{
      MessageRadio();
      }
    
}

void pingRadio(){
currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        sendPing();
        prevMillis = millis();
    }
  
  }

 void sendPing(){
  if(go_to_tx_mode){
    radio.stopListening();
    bool fine = radio.write( &dataToSend, sizeof(dataToSend) );
    Serial.print("ping request ");
    Serial.println(dataToSend);
    if(fine){
      Serial.println("  Acknowledge received");
      updateMessage();
      }
//      else{
//       Serial.println("Host Device Unreachable");
//        }
    }
  }

  void updateMessage() {
        // so you can see that new data is being sent
    txNum += 1;
    if (txNum > '9') {
        txNum = '0';
    }
    dataToSend[8] = txNum;
}

  void MessageRadio(){
    if (go_to_tx_mode)
  {
    radio.stopListening();
    bool ok = radio.write(input, 32);
    if (ok)
    {
      printf("ME : %s\n", input);
      go_to_tx_mode = false;
      radio.startListening();
    }
    else{
      //printf("could not transmit, check LOS\n");
    }
  }

  
  if (radio.available())
  {
    char payload[32] = "";
    bool done = false;
    while (!done)
    {
      done = radio.read( payload , 32 );
      printf("HIM : %s\n", payload);
    }
 
  }
    }
