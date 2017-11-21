#include <Servo.h> // подключаем библиотеку для работы с сервоприводом
#include "motors.h"
Servo claw; // restriction 0 - 150

byte startPin = 47, lineSensor = 22;
bool craneUp = true;

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


int measure_distance(byte n)
{
  float dist = 0;
  for(int i = 0; i < 6; i++)
    dist += get_distance(n);
  return int(dist/(float)6);
}

void claw_close(){
   claw.write(0);
   delay(700);  
}

void claw_open(){
   claw.write(90);
   delay(1000);
}

void print_all_sensors()
{
    Serial.print(get_distance(0));
    Serial.print(" ");
    Serial.print(get_distance(1));
    Serial.print(" ");
    Serial.print(get_distance(2));
    Serial.print(" ");
    Serial.print(compass.readAzimut());
    Serial.println(" ");
}

void move_along(int to_dist, int keep_dist, byte sensor)
{
  while(measure_distance(1) > to_dist)
  {
    int dist = get_distance(sensor);
    byte k = (sensor == 0) ? 1 : -1;
    forward_ratio(255, k*(float)(dist-keep_dist)/(float)5);
  }
}

void setup() {
  Serial.begin(9600);
  // инициализация компаса
  compass.begin();
  // калибровка компаса
  compass.calibrateMatrix(compassCalibrationMatrix, compassCalibrationBias);
  for(int i = 0; i < 3; i++)
  {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
  claw.attach(10); // привязываем сервопривод к аналоговому выходу 11
  /*
  int a = compass.readAzimut();
  while(true)
  Serial.println(angle_dist(compass.readAzimut(), a));
  */
  pinMode(startPin, INPUT_PULLUP);
  /*while(digitalRead(startPin));
  currAngle = compass.readAzimut();

  
  if(measure_distance(1) < 10 || measure_distance(1) == 255)
  {
    crane_up();
    craneUp = true;
  }   
  //turn(-90);
  int t = millis();
  //forward(255);
  while(measure_distance(1) > 10)
  {
  Serial.print(int(compass.readAzimut() - currAngle));
  Serial.println(" ");
  }
  stop();
  while(true){
    //right(255);
    //print_all_sensors();
  }
  // Follow from start to pallets
  int dist = 255;
  
  move_along(30, 15, 0);
  turn(-90);
  move_along(32, 30, 0);
  turn(-90);
  move_along(25, 38, 0);  
  backward(255);
  delay(500);
  stop();
  
  catch_item();  
  turn(-90);
  turn(-90);
  
  move_along(32, 38, 2);    
  stop();
  claw_open();
  // Counting lines
  
  */
 
  //claw_close();
  //catch_item();
}

void catch_item(){
  byte min_dist = 14;
  byte speed = 160;
  if(!craneUp)
    crane_up();
  claw_open();
  while(measure_distance(1) > min_dist)
    forward(speed);
    
  while(measure_distance(1) < min_dist)
    backward(speed);
  left(255);
  delay(500);
  stop();

  while(measure_distance(1) > min_dist)
    right(speed+70);
  stop();

  claw_open();
  crane_down();
  craneUp = false;
  claw_close();

  right(speed);
  delay(200);
  left(speed);
  delay(200);
  stop();
  
  crane_up();  
  craneUp = true;
}

void loop() {
    if (Serial.available())
  {
    int n = ((int)Serial.read());
    n -= 48;
    Serial.println(n);
    if(n == 0)
      forward(255);
    if(n == 1)
      left(255);
    if(n == 2)
      right(255);
    if(n == 3)
      backward(255);
    if(n == 4)
      catch_item();
    delay(100);
    stop();
  }
}
