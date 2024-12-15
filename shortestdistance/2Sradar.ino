#include <Servo.h>
#include <SoftwareSerial.h>
//HC-06 시리얼창에서 "line ending 없음" 설정할것

Servo LRSubmotor; // 좌우 서보모터
Servo UDSubmotor; // 상하 서보모터

const int Tx = 12;//전송 보내는핀
const int Rx = 13;//수신 받는핀
SoftwareSerial BtSerial(Tx,Rx);

const int trigPin_R = 6; // 오른쪽 초음파 센서 trigPin
const int echoPin_R = 7; // 오른쪽 초음파 센서 echoPin

const int trigPin_L = 4; // 왼쪽 초음파 센서 trigPin
const int echoPin_L = 5; // 왼쪽 초음파 센서 echoPin

const int trigPin_T = 2;
const int echoPin_T = 3;

const int pirpin = 8; // pir센서가 연결된 핀번호
const int led = 9; // led 핀
const int buzer = A1; // 부저핀

int posLR = 90; // 좌우 서보모터 각도 초기값
int posUD = 90; // 상하 서보모터 각도 초기값

const int threshold = 100; // 한계 거리
bool objectDetected = false; // 물체 감지 여부
const int smoothingSamples = 5; // 평균을 위한 샘플 수 3

float distances[5][smoothingSamples] = {0}; // 센서별 거리값 저장 [센서][시간]
int sampleIndex = 0; // 현재 샘플 인덱스

// 함수 선언
float measureDistance(int trigPin, int echoPin);
float averageDistance(float *values);
void followObject(float avgL, float avgR, float avgT);

int maxDistance = 400; // Define max distance

// Variables for the duration and the distance
long duration;
long durationB;
long durationC;
int distance;
int distanceB;
int distanceC;

int distance_A = 400;
int distance_B = 400;
int distance_C = 400;

void setup() {
  Serial.begin(9600);
  LRSubmotor.attach(11); // 좌우 서보모터 핀
  UDSubmotor.attach(10); // 상하 서보모터 핀

  LRSubmotor.write(posLR);
  UDSubmotor.write(posUD);

  pinMode(pirpin,INPUT);
  pinMode(led,OUTPUT);
  pinMode(buzer,OUTPUT);
 
  pinMode(trigPin_L, OUTPUT);
  pinMode(echoPin_L, INPUT);

  pinMode(trigPin_R, OUTPUT);
  pinMode(echoPin_R, INPUT);

  pinMode(trigPin_T, OUTPUT);
  pinMode(echoPin_T, INPUT);

  //servo.write(0);
  Serial.println("초음파 센서 스캔 시작");
  Serial.begin(9600);
  BtSerial.begin(9600);
}

void loop() {
  short_distance();
  while (1){
    if (maxDistance < distance_A && maxDistance < distance_B && maxDistance < distance_C){
      if (maxDistance > distance_A || maxDistance > distance_B || maxDistance > distance_C){
        break;
      }
      radar();
    }
    else if (maxDistance > distance_A || maxDistance > distance_B || maxDistance > distance_C){
      find_object();
    }
  }
}

int L_calculateDistance() {

  digitalWrite(trigPin_L, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin_L, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin_L, LOW);
  durationB = pulseIn(echoPin_L, HIGH);
  distanceB = durationB * 0.034 / 2;

  return distanceB;
}

int R_calculateDistance() {
  digitalWrite(trigPin_R, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin_R, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin_R, LOW);
  durationC = pulseIn(echoPin_R, HIGH);
  distanceC = durationC * 0.034 / 2;
  return distanceC;
}


int calculateDistance() {
  digitalWrite(trigPin_T, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin_T, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin_T, LOW);
  duration = pulseIn(echoPin_T, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

void radar() {
  for (int posLR = 15; posLR <= 165; posLR++) {
    LRSubmotor.write(posLR);
    delay(30);
    distance = calculateDistance();
    distance_A = calculateDistance();
    distance_B = L_calculateDistance();
    distance_C = R_calculateDistance();
    Serial.print(posLR);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
    //return distance;
  }

  for (int posLR = 165; posLR > 15; posLR--) {
    LRSubmotor.write(posLR);
    delay(30);
    distance = calculateDistance();
    distance_A = calculateDistance();
    distance_B = L_calculateDistance();
    distance_C = R_calculateDistance();
    Serial.print(posLR);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
    //return distance;
  }
}

void short_distance() {
  int minDistance = 9999; 
  int minAngle = 0;
  for (int posLR = 15; posLR <= 165; posLR++) {
     LRSubmotor.write(posLR);
    delay(30);
    float distance = calculateDistance();
    Serial.print("각도: ");
    Serial.print(posLR);
    Serial.print("°, 거리: ");
    if (distance <= 15 && distance != -1) {
      Serial.println("(15cm 이내)");
    } else if (distance > 15) {
      Serial.print(distance);
      Serial.println(" cm");
    } else {
      Serial.println("4m");
    }

    if (distance > 15 && distance < minDistance) {
      minDistance = distance;
      minAngle = posLR;
    }
  }

  Serial.println("---------------------");
  if (minDistance == 9999) {
    Serial.println("유효한 물체를 찾지 못했습니다.");
  } else {
    Serial.print("가장 가까운 거리: ");
    Serial.print(minDistance);
    Serial.print(" cm (각도: ");
    Serial.print(minAngle);
    Serial.println("°)");
    maxDistance = minDistance;
    Serial.print("최대 거리 설정 변경: ");
    Serial.print(maxDistance);
    Serial.println(" cm");
  }
  Serial.println("---------------------");

  for (int posLR = 165; posLR >= 15; posLR--) {
     LRSubmotor.write(posLR);
    delay(30);
  }
  delay(1000);
}

float measureDistance() {
  digitalWrite(trigPin_T, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_T, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_T, LOW);

  long duration = pulseIn(echoPin_T, HIGH);
  float distance = duration * 0.034 / 2;

  if (distance > 15 && distance <= maxDistance) {
    return distance;
  } else if (distance <= 15) {
    return distance;
  } else {
    return -1;
  }
}

void followObject(float avgL, float avgR, float avgT) {
  // 거리 비교를 위한 변수
  float minDistance = min(avgL, min(avgR, avgT));
  
  // 물체 탐지 여부 갱신
  objectDetected = (minDistance < threshold);

  if (minDistance == avgT && avgT < threshold) {
    // 위쪽으로 이동
    if (posUD < 180) {
      posUD += 5; // 한 번에 작은 각도로 이동
      UDSubmotor.write(posUD);
      Serial.println("Moving Up");
    }
  } else if (avgT > threshold) {
    // 위쪽에서 아래로 복귀
    if (posUD > 90) {
      posUD -= 5;
      UDSubmotor.write(posUD);
      Serial.println("Returning Down");
    } else {
      // 아래로 복귀 후 좌우 이동
      if (minDistance == avgL && avgL < threshold) {
        if (posLR > 30) posLR -= 5;
        LRSubmotor.write(posLR);
        Serial.println("Moving Left");
      } else if (minDistance == avgR && avgR< threshold) {
        if (posLR < 150) posLR += 5;
        LRSubmotor.write(posLR);
        Serial.println("Moving Right");
      }
    }
  } 
}

void motion_tracking(){
  int motion = digitalRead(pirpin); //PIR 센서 감지
  if (motion == HIGH) {  // 움직임이 감지되면
    Serial.print("동작이 감지되었습니다.");
    //BtSerial.write("방범 장치에 사람이 감지되었습니다!");
    buzer_led();
    //delay(50);
  }
}

void buzer_led(){
  digitalWrite(led, HIGH); // LED 켜기
  tone(buzer, 494, 500);
  digitalWrite(buzer, HIGH); // 부저 켜기
  delay(50); // 1초 동안 켜짐
  digitalWrite(led, LOW); // LED 끄기
  //digitalWrite(buzer, LOW); // 부저 끄기
}

void find_object(){
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

  motion_tracking(); // pir센서로 동작 추적

  delay(100); // 물체 추적 속도 조절
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
