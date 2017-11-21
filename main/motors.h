#include <AFMotor.h> // Подключаем библиотеку для работы с Motor Shield L293D
// библиотека для работы I²C
#include <Wire.h>
// библиотека для работы с модулями IMU
#include <TroykaIMU.h>

// множитель фильтра
#define BETA 0.22f
Madgwick filter;
Accelerometer accel;
Gyroscope gyro;
// переменные для данных с гироскопов, акселерометров
float gx, gy, gz, ax, ay, az;
// получаемые углы ориентации
int yaw = 0, pitch, roll;
// переменная для хранения частоты выборок фильтра
float fps = 100;

// создаём объект для работы с компасом
Compass compass;
// Придумываем имена моторам и объявляем клеммники к которым они подсоединены
AF_DCMotor my_motor1(2);// left
AF_DCMotor my_motor3(3);// right
//AF_DCMotor my_motor2(2);//захват
AF_DCMotor my_motor4(4);//кран
byte craneSensor = 23;
byte encoder_left_1 = 33, encoder_left_2 = 37, encoder_right_2 = 31, encoder_right_1 = 35;

long rotation_left = 0, rotation_right = 0;
bool pr_l_1 = 0, pr_l_2 = 0, pr_r_1 = 0, pr_r_2 = 0;


ISR(TIMER1_COMPA_vect)
{
  bool l_1 = digitalRead(encoder_left_1);
  bool l_2 = digitalRead(encoder_left_2);
  bool r_1 = digitalRead(encoder_right_1);
  bool r_2 = digitalRead(encoder_right_2);
  if(pr_l_1 && pr_l_2)
    if(l_1 && !l_2)
       rotation_left += 1;
    else if(!l_1 && l_2)
       rotation_left -= 1;
  else if(pr_l_1 && !pr_l_2)
    if(!l_1 && !l_2)
       rotation_left += 1;
    else if(l_1 && l_2)
       rotation_left -= 1;
  else if(!pr_l_1 && pr_l_2)
    if(l_1 && l_2)
       rotation_left += 1;
    else if(!l_1 && !l_2)
       rotation_left -= 1;
  else if(!pr_l_1 && !pr_l_2)
    if(!l_1 && l_2)
       rotation_left += 1;
    else if(l_1 && !l_2)
       rotation_left -= 1;
       
  if(pr_r_1 && pr_r_2)
    if(r_1 && !r_2)
       rotation_right += 1;
    else if(!r_1 && r_2)
       rotation_right -= 1;
  else if(pr_r_1 && !pr_r_2)
    if(!r_1 && !r_2)
       rotation_right += 1;
    else if(r_1 && r_2)
       rotation_right -= 1;
  else if(!pr_r_1 && pr_r_2)
    if(r_1 && r_2)
       rotation_right += 1;
    else if(!r_1 && !r_2)
       rotation_right -= 1;
  else if(!pr_r_1 && !pr_r_2)
    if(!r_1 && r_2)
       rotation_right += 1;
    else if(r_1 && !r_2)
       rotation_right -= 1;

  pr_l_1 = digitalRead(encoder_left_1);
  pr_l_2 = digitalRead(encoder_left_2);
  pr_r_1 = digitalRead(encoder_right_1);
  pr_r_2 = digitalRead(encoder_right_2);

}

/*
void encoder_right_changed() 
{ 
  
  if (millis() - right_last < 2)
     return;
  right_last = millis();
  
  
  //noInterrupts();
  //rotation_left += (digitalRead(encoder_right_1) && digitalRead(encoder_right_2) || 
  //             !digitalRead(encoder_right_1) && !digitalRead(encoder_right_2)) ? -1 : 1;
  //delay(10);
  //interrupts();
}

void encoder_left_changed()
{ 
  if (millis() - left_last < 10)
     return;
  left_last = millis();
    
  //noInterrupts();
  rotation_right += (digitalRead(encoder_left_1) && digitalRead(encoder_left_2) || 
               !digitalRead(encoder_left_1) && !digitalRead(encoder_left_2)) ? 1 : -1;
  //delay(1);
  //interrupts();
} */


int measure_angle()
{
  const int n = 10;
  int angles[n],max_angle = 0, min_angle = 360;
  for(int i = 0; i < n; i++)
  {
    angles[i] = compass.readAzimut();
    if(max_angle < angles[i])
      max_angle = angles[i];
    if(min_angle > angles[i])
      min_angle = angles[i];
  }
  bool shifted = false;
  if(max_angle - min_angle > 180)
  {
    shifted = true;
    for(int i = 0; i < n; i++)
      angles[i] = (angles[i]+180)%360;
  }
  int sum = 0;
  for(int i = 0; i < n; i++)
    sum += angles[i];
  return shifted ? (sum/n-180)%360 : sum/n; 
}

void forward(int speed)
{
  my_motor1.setSpeed(speed);
  my_motor3.setSpeed(speed);
  my_motor1.run(FORWARD);
  my_motor3.run(FORWARD);
}

void stop(){
  my_motor1.run(RELEASE);
  my_motor3.run(RELEASE);
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

void crane_up(){
  my_motor4.setSpeed(255);
  my_motor4.run(FORWARD);
  delay(7000);
  my_motor4.run(RELEASE);
}

void crane_down(){
  my_motor4.setSpeed(255);
  my_motor4.run(BACKWARD);
  while(digitalRead(craneSensor));
  my_motor4.run(RELEASE);
}

int angle_dist(int a, int b)
{
  if(abs(a-b) > 180){
    if (a > b)
      a -= 360;
    else
      b -= 360;
  }
  return abs(a-b);
}

void compensate(int dl, int dr)
{
  long l = rotation_left, r = rotation_right;

  my_motor1.setSpeed(255);
  my_motor3.setSpeed(255);
  while(abs(abs(l - rotation_left) - abs(dl)) > 5 || abs(abs(r - rotation_right) - abs(dr)) > 5)
  {
    Serial.print("");
    my_motor1.run(rotation_left - l < dl ?  FORWARD : BACKWARD);
    my_motor3.run(rotation_right - r < dr ?  FORWARD : BACKWARD);
  }
  stop();
}

void turn(int n)
{
  compensate((float)27/(float)90*n, (float)-27/(float)90*n);
}
  /*
  compensate(n*85/(float)90, -n*85/(float)90);
  long l=rotation_left, r=rotation_right;
  if(n < 0)
    left(255);
  else
    right(255);

  while((rotation_left+rotation_right)/2 < (r+l)/2 + (float)(85*abs(n))/(float)90);
  stop();
}
*/
/*void turn(int n)
{
  if(n < 0)
    left(250);
  else
    right(250);

  int currYaw = yaw;
  
  while(angle_dist(yaw%360, (currYaw + n)%360) > 5)
  {
      unsigned long startMillis = millis();
 
    // считываем данные с акселерометра в единицах G
    accel.readGXYZ(&ax, &ay, &az);
    // считываем данные с акселерометра в радианах в секунду
    gyro.readRadPerSecXYZ(&gx, &gy, &gz);
    // устанавливаем коэффициенты фильтра
    filter.setKoeff(fps, BETA);
    // обновляем входные данные в фильтр
    filter.update(gx, gy, gz, ax, ay, az);
    
    // получение углов yaw, pitch и roll из фильтра
    yaw =  filter.getYawDeg();
  
     // вычисляем затраченное время на обработку данных
    unsigned long deltaMillis = millis() - startMillis;
    // вычисляем частоту обработки фильтра
    fps = 1000 / (float)deltaMillis;
  }
  Serial.println(yaw);
  //delay(200);
  stop();
  */
  /*
  currAngle = (currAngle + n)%360;
  stop();
  int cmpAngle = compass.readAzimut();
  while(angle_dist(cmpAngle, currAngle) > 10)
  {
    int currAngle_cpy = currAngle;
    if(abs(cmpAngle - currAngle) > 180)
    {
     cmpAngle = (cmpAngle + 180)%360;
     currAngle_cpy = (currAngle_cpy + 180)%360; 
    }
    if(cmpAngle < currAngle_cpy)
    {
      right(200);
      Serial.print("Right ");
    }
    else
     {
      left(200);
      Serial.print("Left ");
     }

   Serial.print(cmpAngle);
   Serial.print(" ");
   Serial.println(currAngle_cpy);
   cmpAngle = compass.readAzimut();
  }
  stop();
*/
//}

// калибровочные значения компаса
// полученные в калибровочной матрице из примера «compassCalibrateMatrix»
/*const double compassCalibrationBias[3] = {
  4732.823,
  -6644.02,
  1630.859
};
 
const double compassCalibrationMatrix[3][3] = {
  {2.254, 0.603, 46.172},
  {0.259, 2.376, -55.12},
  {-0.085, 0.11, 16.83}
};
const double compassCalibrationBias[3] = {
  1793.867,
  -761.903,
  -343.369
};
 
const double compassCalibrationMatrix[3][3] = {
  {1.126, 0.067, 0.019},
  {0.025, 1.158, 0.061},
  {0.054, -0.033, 1.184}
};
int currAngle;*/

