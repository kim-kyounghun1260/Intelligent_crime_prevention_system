#include <Servo.h> // 서보 모터 제어를 위한 라이브러리

// 핀 정의
const int trigPin = 2;    // 초음파 센서의 Trig 핀
const int echoPin = 3;   // 초음파 센서의 Echo 핀
const int servoPin = 10;   // 서보 모터 제어 핀

Servo servo; // 서보 모터 객체 생성

// 함수: 초음파 센서를 이용해 거리 측정
long measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH); // 초음파 신호 왕복 시간 측정
  long distance = duration * 0.034 / 2;  // 거리를 cm로 변환
  return distance;
}

// 함수: 서보 모터를 천천히 특정 각도로 이동
void smoothServoMove(Servo &servo, int currentAngle, int targetAngle, int stepDelay) {
  if (currentAngle < targetAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle++) {
      servo.write(angle);
      delay(stepDelay); // 단계별 지연
    }
  } else {
    for (int angle = currentAngle; angle >= targetAngle; angle--) {
      servo.write(angle);
      delay(stepDelay); // 단계별 지연
    }
  }
}

void setup() {
  // 핀 설정
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // 서보 초기화
  servo.attach(servoPin); 
  
  // 시리얼 모니터 시작
  Serial.begin(9600);
}

void loop() {
  int ranges[][2] = {
    {0, 36}, {36, 72}, {72, 108}, {108, 144}, {144, 180}
  }; // 5개 범위 정의 (시작 각도, 끝 각도)
  
  int currentAngle = 0; // 현재 서보 모터 각도 추적
  
  for (int i = 0; i < 5; i++) {
    int startAngle = ranges[i][0];
    int endAngle = ranges[i][1];
    long minDistance = 9999;  // 가장 짧은 거리를 저장
    int minAngle = startAngle; // 가장 짧은 거리가 발생한 각도 저장

    Serial.print("Measuring range: ");
    Serial.print(startAngle);
    Serial.print(" to ");
    Serial.print(endAngle);
    Serial.println(" degrees");
    
    // 현재 범위 내에서 각도를 세밀하게 이동
    for (int angle = startAngle; angle <= endAngle; angle += 1) {
      smoothServoMove(servo, currentAngle, angle, 10); // 천천히 이동 (10ms 지연)
      currentAngle = angle; // 현재 각도 갱신
      
      long distance = measureDistance(); // 거리 측정
      Serial.print("Angle: ");
      Serial.print(angle);
      Serial.print(" degrees, Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
      
      // 최소 거리 및 각도 갱신
      if (distance < minDistance) {
        minDistance = distance;
        minAngle = angle;
      }
    }
    
    // 현재 범위에서 가장 짧은 거리와 각도 출력
    Serial.println("-----");
    Serial.print("Shortest Distance in range ");
    Serial.print(startAngle);
    Serial.print(" to ");
    Serial.print(endAngle);
    Serial.print(" degrees: ");
    Serial.print(minDistance);
    Serial.println(" cm");
    Serial.print("At Angle: ");
    Serial.print(minAngle);
    Serial.println(" degrees");
    Serial.println("-----");
    
    delay(1000); // 다음 범위 측정 전에 1초 대기
  }
  
  delay(5000); // 모든 범위를 측정 후 5초 대기
}
