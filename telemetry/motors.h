#include <AFMotor.h> // Подключаем библиотеку для работы с Motor Shield L293D
// библиотека для работы I²C
#include <Wire.h>
// библиотека для работы с модулями IMU
#include <TroykaIMU.h>

// создаём объект для работы с компасом
Compass compass;
// Придумываем имена моторам и объявляем клеммники к которым они подсоединены
AF_DCMotor my_motor1(1);//колесо
AF_DCMotor my_motor3(3);//колесо
AF_DCMotor my_motor2(2);//захват
AF_DCMotor my_motor4(4);//кран

// калибровочные значения компаса
// полученные в калибровочной матрице из примера «compassCalibrateMatrix»
const double compassCalibrationBias[3] = {
  526.967,
  -1646.903,
  88.669
};
 
const double compassCalibrationMatrix[3][3] = {
  {1.205, 0.016, -0.013},
  {+0.016, 1.198, -0.024},
  {0.021, -0.05, 1.282}
};

int currAngle;

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

void forward_ratio(int speed, float ratio)
{
  my_motor1.setSpeed(max(min(speed+int(speed*ratio), 255), 125));
  my_motor3.setSpeed(max(min(speed-int(speed*ratio), 255), 125));
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

void crane_up(){
  my_motor4.setSpeed(255);
  my_motor4.run(FORWARD);
  delay(4000);
  my_motor4.run(RELEASE);
}

void crane_down(){
  my_motor4.setSpeed(255);
  my_motor4.run(BACKWARD);
  delay(4000);
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

void turn(int n)
{
  currAngle = (currAngle + n)%360;
  stop();
  if(n < 0)
    left(250);
  else
    right(250);
  while(angle_dist(compass.readAzimut(), currAngle) > 5);
  Serial.println(angle_dist(compass.readAzimut(), currAngle));
  stop();
}

