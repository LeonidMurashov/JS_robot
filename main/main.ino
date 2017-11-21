
#include <Servo.h> // подключаем библиотеку для работы с сервоприводом
#include "motors.h"
#include "colors.h"
#include <Ultrasonic.h>
Servo claw; // restriction 0 - 150

byte startPin = 47, lineSensor = 22;
bool craneUp = true;
byte color_places [] = { 0, 1, 2, 3 };
byte color_distance [] = {10, 38, 61, 80};
Ultrasonic ultrasonics[] = {
  Ultrasonic(53, 52),
  Ultrasonic(51, 50),
  Ultrasonic(49, 48),
};

byte get_color_place(byte color)
{
  for(int i = 0; i < 4; i++)
    if(color_places[i] == color)
      return i;
}


byte trigPins [] = {53, 51, 49};
byte echoPins [] = {52, 50, 48};
/*int get_distance(byte n)
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
  if(cm > 255 || cm == 0)
    cm = 255;
  delay(30);
  return cm;
}*/


void move_along(int to_dist, int keep_dist, byte sensor)
{
  int d = 999, max_last = 0;
  long ml, mr;
  float error = 1, min_speed = 150, prev_dl = 0;
  my_motor1.run(FORWARD);
  my_motor3.run(FORWARD);
  while(d > to_dist)
  {
    d = measure_distance(1);
    int dist = get_distance(sensor);
   // Serial.println(dist);
    int k = (sensor == 0) ? 1 : -1;
    float dl = (dist-keep_dist)*k;
    
    Serial.println(d);
    /*Serial.print(" ");
    Serial.print(dl);
    Serial.print(" ");
    Serial.println(max_last);
    */
    if(prev_dl*dl <= 0 && abs(max_last) > 5)
    {
      int dl = rotation_left - ml, dr = rotation_right - mr;
      if(dl > dr)
        compensate(0, dl - dr);
      else
        compensate(dr - dl, 0);
      
     /* Serial.print(dr);
    Serial.print(" ");
    Serial.println(dl);
      */
      //delay(500);
      my_motor1.run(FORWARD);
      my_motor3.run(FORWARD);
      max_last = 0;
    }
    if (dl > 0)
    {
      my_motor1.setSpeed(255);
      my_motor3.setSpeed(max(255 - abs(dl)/error*(255-min_speed), min_speed));
    }
    else
    {      
      my_motor3.setSpeed(255);
      my_motor1.setSpeed(max(255 - abs(dl)/error*(255-min_speed), min_speed));
    }
    if(abs(max_last) < abs(dl))
    {
      ml = rotation_left;
      mr = rotation_right;
      max_last = dl;
    }
    prev_dl = dl;
  }
  stop();
}

/*int get_distance(byte n)
{
  delay(20);
  return ultrasonics[n].Ranging(CM);
}*/


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
  int dist = 0;
  for(int i = 0; i < 5; i++)
  {
    int d = get_distance(n);
    dist = max(dist, d == 255 ? 0 : d);
  }
  return dist;
}

void claw_close(){
   claw.write(10);
   delay(700);
}

void claw_open(){
   claw.write(90);
   delay(700);
}

void claw_halfopen(){
   claw.write(70);
   delay(700);
}

void print_all_sensors()
{
  Serial.print(get_distance(0));
  Serial.print(" ");
  Serial.print(get_distance(1));
  Serial.print(" ");
  Serial.print(get_distance(2));
  Serial.print(" ");
  //Serial.print(compass.readAzimut());
  Serial.println(" ");
}

/*
void move_along(int to_dist, int keep_dist, byte sensor)
{
  int d = 999;
  while(d > to_dist)
  {
    d = max(get_distance(1), get_distance(1));
    Serial.println(d);
    int dist = get_distance(sensor);
    byte k = (sensor == 0) ? 1 : -1;
    forward_ratio(200, k*(float)(dist-keep_dist)/(float)3);
  }
  stop();
}*/

void forward_to(int n)
{
  forward(255);
  while(measure_distance(1) > n);
  stop();
}

void backward_to(int n)
{
  backward(200);
  while(measure_distance(1) < n);
  stop();
}

byte bring_item()
{  
  move_along(25, 37, 0);  // Go to item   
  byte color = catch_item();
  //forward(200);
  //while(get_distance(1) > 15);
  forward(255);
  delay(1500);
  backward_to(20);
  turn(90);
  move_along(50, 37, 2);
  turn(-90);  
  return color;
}

void drop_cube(byte color)
{
  
  Serial.print("!!!!!!!!!!!!!!!!!! ");
  Serial.print(color);
  Serial.print(" ");
  Serial.print(get_color_place(color));
  Serial.print(" ");
  Serial.println(color_distance[get_color_place(color)]);
  forward(255);
  delay(2000);
  backward_to(color_distance[get_color_place(color)]);
  turn(90);

  stop();
  forward(200);
  delay(1500);
  stop();

  
  my_motor4.setSpeed(255);
  my_motor4.run(BACKWARD);
  delay(1000);
  my_motor4.run(RELEASE);
  
  claw_open();
  backward(255);
  delay(1000);
  stop();
  crane_up();
  claw_open();

  turn(-90);
  forward_to(5);
  forward(255);
  delay(300);
  backward_to(37);
  turn(-90);
}


void drop_cylinder(byte color)
{
  
  Serial.print("!!!!!!!!!!!!!!!!!! ");
  Serial.print(color);
  Serial.print(" ");
  Serial.print(get_color_place(color));
  Serial.print(" ");
  Serial.println(color_distance[get_color_place(color)]);
  forward(255);
  delay(2000);
  backward_to(color_distance[get_color_place(color)]);
  turn(85);

  backward(255);
  delay(300);
  stop();
  crane_down();
  forward(255);
  delay(1500);
  stop();
  claw_open();
  for(int i = 0; i < 10; i++)
  {
    backward(255);
    delay(110);
    forward(255);
    delay(100);
  }  

  backward(255);
  delay(1000);
  stop();
  crane_up();
  claw_open();

  turn(-90);
  forward_to(5);
  forward(255);
  delay(300);
  backward_to(37);
  turn(-90);
}

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);

  //attachInterrupt(digitalPinToInterrupt(encoder_left_1), encoder_left_changed, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(encoder_right_1), encoder_right_changed, CHANGE);


/////////////////////////////////////////////
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 30; // ~ 1KHz            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
////////////////////////////////////////////////////////////

  init_color_sensor();
  claw.attach(10);
  claw_open();

  for(int i = 0; i < 3; i++)
  {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
  
  pinMode(craneSensor, INPUT_PULLUP);
  pinMode(startPin, INPUT_PULLUP);
  pinMode(encoder_left_1, INPUT_PULLUP);
  pinMode(encoder_right_1, INPUT_PULLUP);
  pinMode(encoder_left_2, INPUT_PULLUP);
  pinMode(encoder_right_2, INPUT_PULLUP);

  if(!digitalRead(craneSensor))
    {
      crane_up();
      craneUp = true;
    } 


  for(int i = 0; i<4; i++)
  {
    while(digitalRead(startPin) && !Serial.available());  
    color_places[i] = read_color();
  } 
  while(digitalRead(startPin) && !Serial.available());
  Serial.read();
  delay(100);

  //compensate(-10, -10);
  //stop();
    //move_along(25, 34, 0);

  while(!true)
    Serial.println(read_color());


  
  //move_along(38, 40, 0);
  //turn(-90);

  
  forward(255);
  delay(2000);
  turn(-90);
  
  forward_to(5);
  forward(255);
  delay(300);
  backward_to(37);
  turn(-90);
  
  byte color = bring_item();
  drop_cylinder(color);
  
  color = bring_item();
  drop_cube(color);

  color = bring_item();
  drop_cylinder(color);
  
  color = bring_item();
  drop_cube(color);
  
  while(true)
    print_all_sensors();
  
  while(!true)
  {
    claw_close();
    delay(2000);
    claw_open();
    delay(2000);
    while(digitalRead(startPin) && !Serial.available());
    Serial.read();    
  }   
  /* 
  //claw_close();
  //while(true);
  // Follow from start to pallets
  move_along(40, 15, 0);
  turn(-90);
  move_along(38, 40, 0);
  turn(-90);

  for(int i = 0; i < 2; i++)
  {
    goToItem();
    catch_item();  
    move_along(32, 52, 2);
    stop();
    while(true)
      print_all_sensors();
    claw_open();
    turn(-90);
    turn(-90);
    
    // Counting lines
  }*/
  
  //claw_close();
  //catch_item();
}

byte catch_item()
{
  long l = rotation_left, r = rotation_right;  
  byte min_dist = 11;
  byte speed = 255;
  if(!craneUp)
    crane_up();
  claw_open();
  while(get_distance(1) > min_dist)
  {
    Serial.println(get_distance(1));
    forward(200);
  } 
  right(255);
  delay(300);
  stop();

  while(get_distance(1) > min_dist)
    left(230);
  delay(200);
  backward(255);
  if(get_distance(1) < 9)
    delay(150);
  stop();

  claw_open();
  crane_down();
  craneUp = false;
  claw_close();
  delay(200);
  byte color = read_color();
  
  crane_up();  
  craneUp = true;

  compensate(l-rotation_left, r-rotation_right);
  turn(45);
  right(250);
  while(get_distance(1) > 70)
    print_all_sensors();
  turn(75); 
   
  return color;
}
bool closed = false;
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
    if(n == 4){    
      my_motor4.setSpeed(255);
      my_motor4.run(FORWARD);  
    }
    if(n == 5)
    {
      my_motor4.setSpeed(255);
      my_motor4.run(BACKWARD);
    }  
    if(n == 6)
    {
      if(closed)
        claw_open();
      else
        claw_close();
      closed ^= 1;
    }
    delay(100);
    stop();
    my_motor4.run(RELEASE);
  }
  /*
  if (craneUp){
    delay(4000);
    craneUp = false;
    claw_open();
    crane_down();
  }*/
}
