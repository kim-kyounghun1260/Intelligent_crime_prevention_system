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

const int threshold = 100; // 한계 거리
bool objectDetected = false; // 물체 감지 여부
const int smoothingSamples = 3; // 평균을 위한 샘플 수

float distances[3][smoothingSamples] = {0}; // 센서별 거리값 저장 [센서][시간]
int sampleIndex = 0; // 현재 샘플 인덱스

// 함수 선언
float measureDistance(int trigPin, int echoPin);
float averageDistance(float *values);
void followObject(float avgL, float avgR, float avgT);

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
  // 초음파 거리 측정 및 저장
  distances[0][sampleIndex] = measureDistance(trigPin_L, echoPin_L); // Left
  distances[1][sampleIndex] = measureDistance(trigPin_R, echoPin_R); // Right
  distances[2][sampleIndex] = measureDistance(trigPin_T, echoPin_T); // Top

  // 샘플 인덱스 갱신
  sampleIndex = (sampleIndex + 1) % smoothingSamples;

  // 평균값 계산
  float avgL = averageDistance(distances[0]);
  float avgR = averageDistance(distances[1]);
  float avgT = averageDistance(distances[2]);

  Serial.print("Avg Left: "); Serial.print(avgL); Serial.print(" cm, ");
  Serial.print("Avg Right: "); Serial.print(avgR); Serial.print(" cm, ");
  Serial.print("Avg Top: "); Serial.println(avgT);

  // 물체의 위치를 기준으로 모터 제어
  followObject(avgL, avgR, avgT);
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

float averageDistance(float *values) {
  float sum = 0;
  for (int i = 0; i < smoothingSamples; i++) {
    sum += values[i];
  }
  return sum / smoothingSamples;
}

void followObject(float avgL, float avgR, float avgT) {
  // 거리 비교를 위한 변수
  float minDistance = min(avgL, min(avgR, avgT));
  
  // 물체 탐지 여부 갱신
  objectDetected = (minDistance < threshold);

  if (minDistance == avgT && avgT < threshold) {
    // 위쪽으로 이동
    if (posUD < 180) {
      posUD += 2; // 한 번에 작은 각도로 이동
      UDSubmotor.write(posUD);
      Serial.println("Moving Up");
    }
  } else if (avgT > threshold) {
    // 위쪽에서 아래로 복귀
    if (posUD > 90) {
      posUD -= 2;
      UDSubmotor.write(posUD);
      Serial.println("Returning Down");
    } else {
      // 아래로 복귀 후 좌우 이동
      if (minDistance == avgL && avgL < threshold) {
        if (posLR > 30) posLR -= 2;
        LRSubmotor.write(posLR);
        Serial.println("Moving Left");
      } else if (minDistance == avgR && avgR< threshold) {
        if (posLR < 150) posLR += 2;
        LRSubmotor.write(posLR);
        Serial.println("Moving Right");
      }
    }
  } 
}
