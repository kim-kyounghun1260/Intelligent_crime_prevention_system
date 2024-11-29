블루투스 통신 코드
#include <SoftwareSerial.h>
//HC-06 시리얼창에서 "line ending 없음" 설정할것
intTx = 6;//전송 보내는핀
intRx = 7;//수신 받는핀
SoftwareSerial BtSerial(Tx,Rx);
void setup(){
 Serial.begin(9600);
 Serial.println("hello");
 BtSerial.begin(9600);
}
void loop(){
 if (BtSerial.available()){
  Serial.write(BtSerial.read());
 }
 if (Serial.available()){
  BtSerial.write(Serial.read());
 }
}
