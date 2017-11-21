/* Arduino 101: timer and interrupts
   1: Timer1 compare match interrupt example 
   more infos: http://www.letmakerobots.com/node/28278
   created by RobotFreak 
*/
#define ledPin 13


void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;

  OCR0A = 200;            // compare match register 16MHz/256/2Hz
  TCCR0A |= (1 << WGM01);   // CTC mode
  TCCR0B |= (1 << CS02);    // 256 prescaler
  TCCR0B |= (1 << CS00);    // 256 prescaler
  TIMSK0 |= (1 << OCIE0A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
  
}
int a = 0;

ISR(TIMER0_COMPA_vect)          // timer compare interrupt service routine
{
  if(a % 4 == 0)
  {
  //Serial.print(a);
  //Serial.println("hello");
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
  }
  a += 1;
}

void loop()
{
  // your program here...
}

