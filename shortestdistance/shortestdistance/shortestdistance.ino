#include <Servo.h>

// 초음파 센서 핀 설정
const int trigPin = 2;
const int echoPin = 3;

// 서보 모터 설정
Servo servo;
const int servoPin = 11;

// 스캔 범위 및 변수 설정
const int scanStart = 0;      // 스캔 시작 각도
const int scanEnd = 180;      // 스캔 종료 각도
const int stepDelay = 30;     // 각도 변화 간 대기 시간 (ms)
const int returnSpeedDelay = 50; // 0도로 복귀 시 속도 조정 딜레이 (ms)

// 거리 데이터 저장 변수
float minDistance = 9999;     // 가장 가까운 거리
int minAngle = 0;             // 가장 가까운 물체의 각도
float maxDistance = 400;      // 최대 탐지 거리 (초기값: 400cm)

void setup() {
  // 시리얼 통신 초기화
  Serial.begin(9600);

  // 초음파 핀 모드 설정
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // 서보 모터 초기화
  servo.attach(servoPin);
  servo.write(0);

  Serial.println("초음파 센서 스캔 시작");
}

void loop() {
  minDistance = 9999;  // 최소 거리 초기화
  minAngle = 0;        // 최소 거리 각도 초기화

  // 서보 모터를 회전하며 초음파 센서 스캔
  for (int angle = scanStart; angle <= scanEnd; angle++) {
    servo.write(angle);                // 서보 모터를 특정 각도로 회전
    delay(stepDelay);                  // 모터 안정화를 위해 대기
    float distance = measureDistance(); // 현재 각도에서 거리 측정

    // 거리 정보 출력
    Serial.print("각도: ");
    Serial.print(angle);
    Serial.print("°, 거리: ");
    if (distance <= 50 && distance != -1) {
      Serial.println("(50cm 이내)"); // 50cm 이하일 경우 출력 변경
    } else if (distance > 50) {
      Serial.print(distance);
      Serial.println(" cm");
    } else {
      Serial.println("4m");
    }

    // 유효 거리(50cm 초과, maxDistance 이하)만 확인
    if (distance > 50 && distance < minDistance) {
      minDistance = distance;
      minAngle = angle;
    }
  }

  // 가장 가까운 물체의 정보 출력
  Serial.println("---------------------");
  if (minDistance == 9999) {
    Serial.println("유효한 물체를 찾지 못했습니다.");
  } else {
    Serial.print("가장 가까운 거리: ");
    Serial.print(minDistance);
    Serial.print(" cm (각도: ");
    Serial.print(minAngle);
    Serial.println("°)");

    // 다음 스캔의 최대 거리로 업데이트
    maxDistance = minDistance;
    Serial.print("최대 거리 설정 변경: ");
    Serial.print(maxDistance);
    Serial.println(" cm");
  }
  Serial.println("---------------------");

  // 모터를 0도로 천천히 복귀
  for (int angle = scanEnd; angle >= scanStart; angle--) {
    servo.write(angle);
    delay(returnSpeedDelay); // 속도 조정 딜레이
  }

  delay(1000); // 다음 스캔 전 대기
}

// 초음파 센서를 사용하여 거리 측정
float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 초음파 신호가 돌아오는 시간을 측정
  long duration = pulseIn(echoPin, HIGH);

  // 시간을 거리로 변환 (음속: 343m/s)
  float distance = duration * 0.034 / 2;

  // 유효 거리 범위 조건 (50cm 초과, maxDistance 이하만 허용)
  if (distance > 50 && distance <= maxDistance) {
    return distance;
  } else if (distance <= 50) {
    return distance; // 50cm 이하 거리 반환
  } else {
    return -1; // 유효하지 않은 거리
  }
}
