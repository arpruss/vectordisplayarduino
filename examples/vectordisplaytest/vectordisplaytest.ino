#include <VectorDisplay.h>

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, 0);
  delay(2000);
  digitalWrite(LED_BUILTIN, 1);
  SerialDisplay.clear();
}

uint16_t y=0;

void loop() {

//  Serial.println("VECTORDISPLAYTEST");
//  delay(100);
//  digitalWrite(LED_BUILTIN, 1);
//  delay(100);
  y = (y + 1)%240;
  SerialDisplay.foreColor(0xFF000000 | ((uint32_t)y<<16));
  SerialDisplay.line(0, y, 319, 239);
  SerialDisplay.text(50, 20, String(y));  
  delay(15);
}

