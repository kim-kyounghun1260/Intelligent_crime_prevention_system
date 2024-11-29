서보모터2개 작동 코드

#include <Servo.h> // Servo 라이브러리 포함

Servo servoBottom; // 아래쪽 서보모터 객체 생성
Servo servoTop;    // 위쪽 서보모터 객체 생성

int posBottom; // 아래 서보모터 각도 변수
int posTop;    // 위 서보모터 각도 변수

void setup() {
  servoBottom.attach(6); // 아래 서보모터 신호선 → 디지털핀 6번
  servoTop.attach(9);    // 위 서보모터 신호선 → 디지털핀 9번
  Serial.begin(9600);
}

void loop() {
  // 아래 서보모터 제어
  posBottom = 30;
  servoBottom.write(posBottom); // 서보모터를 30도로 회전
  delay(1500);

  posBottom = 60;
  servoBottom.write(posBottom); // 서보모터를 60도로 회전
  delay(1500);

  posBottom = 90;
  servoBottom.write(posBottom); // 서보모터를 90도로 회전
  delay(1500);

  for (posBottom = 90; posBottom < 180; posBottom++) {
    servoBottom.write(posBottom); // 서보모터를 90도 → 180도로 점진적으로 회전
    delay(50);
  }

  // 위 서보모터 제어

  posTop = 90;
  servoTop.write(posTop); // 서보모터를 90도로 회전
  delay(1500);

  posTop = 180;
  servoTop.write(posTop); // 서보모터를 180도로 회전
  delay(1500);

  for (posTop = 90; posTop < 180; posTop++) {
    servoTop.write(posTop); // 서보모터를 90도 → 180도로 점진적으로 회전
    delay(50);
  }
}