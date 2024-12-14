#include <Servo.h>

// Defines Tirg and Echo pins of the Ultrasonic Sensor
const int trigPin = 2;
const int echoPin = 3;
const int servoPin = 11; // Define the servo pin
int maxDistance = 400; // Define max distance

// Variables for the duration and the distance
long duration;
int distance;
Servo servo;

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin); // Defines on which pin the servo motor is attached
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.write(0);
  Serial.println("초음파 센서 스캔 시작");
}

void loop() {
  short_distance();

  while (true) {
    if (maxDistance > distance)
      
    else
      radar();
  }
}

int calculateDistance() {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

void radar() {
  for (int i = 15; i <= 165; i++) {
    servo.write(i);
    delay(30);
    distance = calculateDistance();
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
  }

  for (int i = 165; i > 15; i--) {
    servo.write(i);
    delay(30);
    distance = calculateDistance();
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
  }
}

void short_distance() {
  int minDistance = 9999; 
  int minAngle = 0;
  for (int angle = 15; angle <= 165; angle++) {
    servo.write(angle);
    delay(30);
    float distance = calculateDistance();
    Serial.print("각도: ");
    Serial.print(angle);
    Serial.print("°, 거리: ");
    if (distance <= 50 && distance != -1) {
      Serial.println("(50cm 이내)");
    } else if (distance > 50) {
      Serial.print(distance);
      Serial.println(" cm");
    } else {
      Serial.println("4m");
    }

    if (distance > 50 && distance < minDistance) {
      minDistance = distance;
      minAngle = angle;
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

  for (int angle = 165; angle >= 15; angle--) {
    servo.write(angle);
    delay(30);
  }
  delay(1000);
}

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  if (distance > 50 && distance <= maxDistance) {
    return distance;
  } else if (distance <= 50) {
    return distance;
  } else {
    return -1;
  }
}
