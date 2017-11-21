#include <AFMotor.h> // Подключаем библиотеку для работы с Motor Shield L293D

// Придумываем имена моторам и объявляем клеммники к которым они подсоединены
AF_DCMotor my_motor1(1);//колесо
AF_DCMotor my_motor3(3);//колесо
AF_DCMotor my_motor2(2);//захват
AF_DCMotor my_motor4(4);//кран
int suckerPin = 46;

void forward(int speed)
{
  my_motor1.setSpeed(speed);
  my_motor3.setSpeed(speed);
  my_motor1.run(FORWARD);
  my_motor3.run(FORWARD);
}

void left(int speed)
{
  my_motor1.setSpeed(speed);
  my_motor3.setSpeed(speed);    
  my_motor1.run(BACKWARD);
  my_motor3.run(FORWARD);
}

void right(int speed)
{
  my_motor1.setSpeed(speed);
  my_motor3.setSpeed(speed);
  my_motor3.run(BACKWARD);
  my_motor1.run(FORWARD);
}

void backward(int speed)
{
  my_motor1.setSpeed(speed);
  my_motor3.setSpeed(speed);    
  my_motor1.run(BACKWARD);
  my_motor3.run(BACKWARD);
}

void stop()
{
  my_motor1.run(RELEASE);
  my_motor3.run(RELEASE);
}

void crane_up(){
  my_motor4.run(FORWARD);
}

void crane_down(){
  my_motor4.run(BACKWARD);  
}

void bring(){
  digitalWrite(suckerPin, LOW);
}

void drop(){
  digitalWrite(suckerPin, HIGH);
}

byte trigPins [] = {53, 51, 49};
byte echoPins [] = {52, 50, 48};
int get_distance(byte n)
{
  long duration, cm;
  digitalWrite(trigPins[n], LOW);
  delayMicroseconds(5);
  digitalWrite(trigPins[n], HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPins[n], LOW);
  pinMode(echoPins[n], INPUT);
  duration = pulseIn(echoPins[n], HIGH);
  cm = (duration/2) / 29.1;
  if(cm > 255)
    cm = 255;
  delay(5);
  return cm;
}

void setup() 
{
  Serial.begin (9600);
  for(int i = 0; i < 3; i++)
  {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
  pinMode(suckerPin, OUTPUT);  
  pinMode(47, INPUT_PULLUP);
  my_motor4.setSpeed(255);
  drop();
  while(digitalRead(47));
}

void loop()
{ 
  digitalWrite(suckerPin, HIGH);
  delay(5000);
  digitalWrite(suckerPin, LOW);
  delay(5000);
  /*
  bring();
  delay(1000);
  drop();
  delay(1000);
  
  
  stop();
  delay(5000000);
*/
}


