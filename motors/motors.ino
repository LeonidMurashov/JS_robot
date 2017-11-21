#include <AFMotor.h> // Подключаем библиотеку для работы с Motor Shield L293D

// Придумываем имена моторам и объявляем клеммники к которым они подсоединены
AF_DCMotor my_motor1(1);//колесо
AF_DCMotor my_motor3(3);//колесо
AF_DCMotor my_motor2(2);//захват
AF_DCMotor my_motor4(4);//кран

// trig 51 echo 53



void set_speed(int speed) // функция СКОРОСТЬ
  {
  my_motor1.setSpeed(speed);
  my_motor3.setSpeed(speed);  
  my_motor4.setSpeed(speed);
  }
void motor_forward() // функция ПЕРЕДНИЙ ХОД
  {
  my_motor1.run(FORWARD);
  my_motor3.run(FORWARD);
  my_motor4.run(FORWARD);
  delay(100);
  }
  
  
void motor_backward() // функция ЗАДНИЙ ХОД
  {
  my_motor1.run(BACKWARD); 
  my_motor3.run(BACKWARD);
  my_motor4.run(BACKWARD);
  
  delay(100);
  }

void motor_release() // функция ОСТАНОВКА ДВИГАТЕЛЕЙ
  {
  my_motor1.run(RELEASE);
  my_motor3.run(RELEASE);
  my_motor4.run(RELEASE);
  delay(100);
  }
  

void setup() // НАСТРОЙКИ
  {
  // Посмотрите насколько проще работать с кодом после создания функций
  // нам нужна всего лишь одна строка вызова нужной функции, с помощью которой мы дадим команду сразу четырем двигателям 
    set_speed(250); // вызываем функцию СКОРОСТЬ - устанавливаем начальную скорость двигателей
    Serial.begin(9600);

    my_motor4.run(BACKWARD);
    delay(1000);
    my_motor4.run(FORWARD);
    delay(1000);
    my_motor4.run(BACKWARD);
    delay(1000);
    my_motor4.run(FORWARD);
    delay(1000);
    my_motor4.run(RELEASE);
  }

void loop() // ОСНОВНОЙ ЦИКЛ
{
}
