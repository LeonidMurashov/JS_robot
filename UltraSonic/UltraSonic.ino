#include <Ultrasonic.h>

//
// Ultrasonic HR-SC04 sensor test
//
// http://robocraft.ru
//

#include "Ultrasonic.h"

// sensor connected to:
// Trig - 12, Echo - 13Ь
Ultrasonic ultrasonic(53, 52);

void setup()
{
  Serial.begin(9600);                           // start the serial port
}

void loop()
{
  float dist_cm = ultrasonic.Ranging(CM);       // get distance
  Serial.println(dist_cm);                      // print the distance
                                    // arbitary wait time.
}
