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
const int smoothingSamples = 5; // 평균 샘플 개수

float distances[3][smoothingSamples] = {0}; // 센서별 거리값 저장 [센서][시간]
int sampleIndex = 0; // 현재 샘플 인덱스

// 함수 선언
float measureDistance(int trigPin, int echoPin);
float filterDistance(float *values);
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

  // 필터링된 평균값 계산
  float avgL = filterDistance(distances[0]);
  float avgR = filterDistance(distances[1]);
  float avgT = filterDistance(distances[2]);

  // 디버깅용 출력
  Serial.print("Filtered Left: "); Serial.print(avgL); Serial.print(" cm, ");
  Serial.print("Filtered Right: "); Serial.print(avgR); Serial.print(" cm, ");
  Serial.print("Filtered Top: "); Serial.println(avgT);

  // 물체의 위치를 기준으로 모터 제어
  followObject(avgL, avgR, avgT);
  delay(100); // 최적화된 측정 주기 (100ms)
}

float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 최대 30ms 대기
  float distance = (duration * 0.034) / 2; // cm 단위 계산
  return distance > 400 ? 400 : (distance <= 0 ? 400 : distance); // 유효 범위 제한
}

float filterDistance(float *values) {
  float sum = 0;
  float minVal = 400, maxVal = 0;

  // 노이즈 필터링 (최대값, 최소값 제외)
  for (int i = 0; i < smoothingSamples; i++) {
    if (values[i] < minVal) minVal = values[i];
    if (values[i] > maxVal) maxVal = values[i];
    sum += values[i];
  }
  sum -= (minVal + maxVal); // 최대값과 최소값 제외
  return sum / (smoothingSamples - 2); // 유효값 평균
}

void followObject(float avgL, float avgR, float avgT) {
  // 거리 비교를 위한 변수
  float minDistance = min(avgL, min(avgR, avgT));

  // 물체 탐지 여부 확인
  if (minDistance < threshold) {
    if (minDistance == avgT) {
      // 위쪽 물체 추적
      if (posUD < 180) {
        posUD += 2;
        UDSubmotor.write(posUD);
        Serial.println("Moving Up");
      }
    } else if (minDistance == avgL) {
      // 왼쪽 물체 추적
      if (posLR > 30) {
        posLR -= 2;
        LRSubmotor.write(posLR);
        Serial.println("Moving Left");
      }
    } else if (minDistance == avgR) {
      // 오른쪽 물체 추적
      if (posLR < 150) {
        posLR += 2;
        LRSubmotor.write(posLR);
        Serial.println("Moving Right");
      }
    }
  } else {
    // 물체가 멀리 있을 때 초기 위치로 복귀
    if (posUD > 90) {
      posUD -= 2;
      UDSubmotor.write(posUD);
      Serial.println("Returning Down");
    }
    if (posLR != 90) {
      posLR += (posLR > 90) ? -2 : 2;
      LRSubmotor.write(posLR);
      Serial.println("Centering LR");
    }
  }
}
