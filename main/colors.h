#define S0 40
#define S1 41
#define S2 42
#define S3 43
#define sensorOut 39

// Colors R, G, B
byte colors[4][3] = { { 30, 44, 80},    // yellow 0
                      { 135, 100, 120},  // green 1
                      { 44, 140, 110},  // red    2
                      { 142, 145, 72} };  // blue 3

void init_color_sensor() 
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
    
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
}

byte read_color() 
{
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  int r = pulseIn(sensorOut, LOW);
  delay(100);
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  int g = pulseIn(sensorOut, LOW);
  delay(100);
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  int b = pulseIn(sensorOut, LOW);
  delay(100);

  long long minErr = 1000000;
  byte minArg = 0;
  for(int i = 0; i < 4; i++)
  {
    int err = sqrt(pow(colors[i][0] - r, 2) + 
                   pow(colors[i][1] - g, 2) + 
                   pow(colors[i][2] - b, 2));
    if(minErr > err)
    {
      minErr = err;
      minArg = i;
    }
  }

  Serial.print(r);
  Serial.print(" ");
  Serial.print(g);
  Serial.print(" ");
  Serial.println(b);
  return minArg;
}
