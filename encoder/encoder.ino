byte pin1 = 2, pin2 = 18;

void setup() 
{
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(pin1, INPUT_PULLUP);  
  pinMode(pin2, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(pin1), interrupt1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pin2), interrupt2, CHANGE);
}

int rotation = 0;
byte st1 = 0, st2 = 0;

void loop() 
{
  //Serial.print(digitalRead(pin1));
  //Serial.print(" ");
  //Serial.println(digitalRead(pin2));
  Serial.println(rotation);
}

void interrupt1()
{
  noInterrupts();
  rotation += (digitalRead(pin1) && digitalRead(pin2) || 
               !digitalRead(pin1) && !digitalRead(pin2)) ? 1 : -1;
  delay(10);
  interrupts();
}

void interrupt2()
{
  noInterrupts();
  //rotation += digitalRead(pin1) ? 1 : -1;
  //delay(10);
  interrupts();
}

