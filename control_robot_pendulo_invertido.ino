#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// Pines del driver de motor (TB6612)
const int PWMA = 9;
const int AIN1 = 7;
const int AIN2 = 8;
const int PWMB = 10;
const int BIN1 = 5;
const int BIN2 = 6;
const int STBY = 4;
const int ledPin = 13; // LED indicador

// Variables PID
float setpoint = 0;  // Ángulo objetivo
float input, error, output;
float lastError = 0;
float integral = 0;
float derivative = 0;

// Constantes PID (ajuste fuerte para levantarse)
float Kp_fast = 24.0;
float Ki_fast = 0;
float Kd_fast = 0.5;

// Constantes PID (ajuste fino para estabilizar)
float Kp_slow = 12.3;
float Ki_slow = 0.2;
float Kd_slow = 1.3;

// Valores activos del PID
float Kp = Kp_fast;
float Ki = Ki_fast;
float Kd = Kd_fast;

unsigned long lastTime = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Inicializar MPU
  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("Error MPU6050");
    while (1);
  }

  Serial.println("Calibrando MPU...");
  delay(1000);
  mpu.calcOffsets(true, true);
  Serial.println("Calibración completa.");

  // Parpadeo del LED
  unsigned long startBlink = millis();
  while (millis() - startBlink < 5000) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }

  // Configurar pines del motor
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH); // Activar motores
}

void loop() {
  mpu.update();
  input = mpu.getAngleX();  // Leer ángulo

  // Cambio automático de modo PID
  if (abs(input) > 12) {
    Kp = Kp_fast;
    Ki = Ki_fast;
    Kd = Kd_fast;
  } else {
    Kp = Kp_slow;
    Ki = Ki_slow;
    Kd = Kd_slow;
  }

  // Cálculo PID
  error = input - setpoint;
  integral += error;
  derivative = error - lastError;
  output = Kp * error + Ki * integral + Kd * derivative;
  lastError = error;

  // Control de motores
  int velocidad = constrain(abs(output), 0, 255);
  if (output > 1) {
    moverAtras(velocidad);
  } else if (output < -1) {
    moverAdelante(velocidad);
  } else {
    detenerMotores();
  }

  // Debug
  Serial.print("Ángulo: ");
  Serial.print(input);
  Serial.print(" | Velocidad: ");
  Serial.println(velocidad);

  delay(10);  // 100 Hz de actualización
}

// Control de motores

void moverAdelante(int velocidad) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, velocidad);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, velocidad);
}

void moverAtras(int velocidad) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, velocidad);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, velocidad);
}

void detenerMotores() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}
