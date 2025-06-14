# pendulo_invertido

# Robot Péndulo Invertido - Control por PID con MPU6050

Este proyecto implementa un **robot de equilibrio vertical (péndulo invertido)** que puede partir tanto desde una posición inclinada (~40°) como de una posición en equilibrio y logra estabilizarse automáticamente en posición vertical utilizando un **controlador PID** con realimentación de ángulo obtenida por un sensor **MPU6050**.

---

## Requisitos de hardware

- **Placa Arduino Nano** (u otra compatible)
- **Sensor MPU-6050** (giroscopio + acelerómetro)
- **Driver de motores TB6612FNG**
- **2 motores DC** con ruedas
- **Batería adecuada** (En este caso Li-Po)
- **PCB y chasis**

---

## Conexiones

### MPU6050 → Arduino
- VCC → 5V  
- GND → GND  
- SDA → A4  
- SCL → A5

### TB6612FNG → Arduino
- PWMA → Pin 9  
- AIN1 / AIN2 → Pin 7 / 8  
- PWMB → Pin 10  
- BIN1 / BIN2 → Pin 5 / 6  
- STBY → Pin 4  
- VM / VCC / GND → Alimentación del motor

---

## Lógica de cambio de configuraciones PID

```cpp
if (abs(input) > 12) {
  Kp = Kp_fast; Ki = Ki_fast; Kd = Kd_fast;
} else {
  Kp = Kp_slow; Ki = Ki_slow; Kd = Kd_slow;
}
```

---

## Funcionamiento General

### 1. Inicialización y calibración

Al iniciar, el programa calibra el sensor MPU6050 y establece la posición inicial como referencia de equilibrio.

### 2. Lectura del ángulo

En cada ciclo del `loop()`, se obtiene el ángulo de inclinación del robot a partir del eje X del sensor. Este ángulo indica que tan lejos está el robot de la posición de equilibrio.

### 3. Cambio automático de modo PID

Se implementan dos configuraciones PID:

* **Modo rápido (levantamiento)**: Se activa si el ángulo supera los 12°. Usa constantes más agresivas para levantar el robot rápidamente desde una posición inclinada.
* **Modo lento (estabilización)**: Se activa cuando el robot está cerca del equilibrio. Usa constantes más suaves para evitar oscilaciones y mejorar la estabilidad.

### 4. Cálculo PID

El programa calcula la salida del PID (`output`) como una combinación proporcional, integral y derivativa del error entre el ángulo actual y el deseado (`setpoint`, que es 0°).

### 5. Control de motores

Según la señal de salida:

* Si el robot está inclinado hacia adelante, los motores se mueven hacia atrás.
* Si está inclinado hacia atrás, los motores avanzan.
* Si el error es muy pequeño, los motores se detienen.

### 6. Depuración

Se imprime por serial el ángulo actual y la velocidad aplicada, lo que resulta útil para ajustar los parámetros PID y monitorear el comportamiento general del sistema.
