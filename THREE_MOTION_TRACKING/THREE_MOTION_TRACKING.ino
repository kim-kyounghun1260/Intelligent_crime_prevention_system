#include <Servo.h>

Servo LRSubmotor; // 좌우 서보모터
Servo UDSubmotor; // 상하 서보모터

const int trigPin_R = 6; // 오른쪽 초음파 센서 trigPin
const int echoPin_R = 7; // 오른쪽 초음파 센서 echoPin

const int trigPin_L = 4; // 왼쪽 초음파 센서 trigPin
const int echoPin_L = 5; // 왼쪽 초음파 센서 echoPin

const int trigPin_T = 2; // 위쪽 초음파 센서 trigPin
const int echoPin_T = 3; // 위쪽 초음파 센서 echoPin

int posLR = 90; // 좌우 서보모터 각도 초기값
int posUD = 90; // 상하 서보모터 각도 초기값

float Lcm, Rcm, Tcm;
const int threshold = 70; // 한계 거리
bool objectDetected = false; // 물체 감지 여부

void setup() {
  Serial.begin(9600);

  LRSubmotor.attach(11); // 좌우 서보모터 핀
  UDSubmotor.attach(10); // 상하 서보모터 핀

  LRSubmotor.write(posLR);
  UDSubmotor.write(posUD);

  pinMode(trigPin_L, OUTPUT);
  pinMode(echoPin_L, INPUT);

  pinMode(trigPin_R, OUTPUT);
  pinMode(echoPin_R, INPUT);

  pinMode(trigPin_T, OUTPUT);
  pinMode(echoPin_T, INPUT);
}

void loop() {
  // 초음파 거리 측정
  Lcm = measureDistance(trigPin_L, echoPin_L);
  Rcm = measureDistance(trigPin_R, echoPin_R);
  Tcm = measureDistance(trigPin_T, echoPin_T);

  Serial.print("Left: "); Serial.print(Lcm); Serial.print(" cm, ");
  Serial.print("Right: "); Serial.print(Rcm); Serial.print(" cm, ");
  Serial.print("Top: "); Serial.print(Tcm); Serial.println(" cm");

  // 물체의 위치를 기준으로 모터 제어
  followObject();
  delay(50); // 물체 추적 속도 조절
}

float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * 0.034) / 2; // cm 단위 계산
  return distance > 400 ? 400 : distance; // 최대 거리 제한
}

void followObject() {
  // 거리 비교를 위한 변수
  float minDistance = min(Lcm, min(Rcm, Tcm));
  
  // 물체 탐지 여부 갱신
  objectDetected = (minDistance < threshold);

  if (minDistance == Tcm && Tcm < threshold) {
    // 위쪽으로 이동
    if (posUD < 180) {
      posUD += 1; // 한 번에 작은 각도로 이동
      UDSubmotor.write(posUD);
      Serial.println("Moving Up");
    }
  } else if (Tcm > threshold) {
    // 위쪽에서 아래로 복귀
    if (posUD > 90) {
      posUD -= 1;
      UDSubmotor.write(posUD);
      Serial.println("Returning Down");
    } else {
      // 아래로 복귀 후 좌우 이동
      if (minDistance == Lcm && Lcm < threshold) {
        if (posLR > 0) posLR -= 1;
        LRSubmotor.write(posLR);
        Serial.println("Moving Left");
      } else if (minDistance == Rcm && Rcm < threshold) {
        if (posLR < 180) posLR += 1;
        LRSubmotor.write(posLR);
        Serial.println("Moving Right");
      }
    }
  } else {
    // 물체가 탐지되지 않으면 모든 축 초기화
    if (posUD != 90) {
      posUD += (90 - posUD) / abs(90 - posUD); // 수직 초기 위치 복귀
      UDSubmotor.write(posUD);
    }
    if (posLR != 90) {
      posLR += (90 - posLR) / abs(90 - posLR); // 수평 초기 위치 복귀
      LRSubmotor.write(posLR);
    }
    Serial.println("No object detected, returning to center.");
  }
}
