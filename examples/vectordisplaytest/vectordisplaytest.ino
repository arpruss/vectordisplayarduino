#include <VectorDisplay.h>

SerialDisplayClass SerialDisplay;
VectorDisplayMessage msg;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, 0);
  delay(5000);
  digitalWrite(LED_BUILTIN, 1);
  SerialDisplay.clear();
  SerialDisplay.addButton('c', "Circle");
  SerialDisplay.addButton('l', "Clear");
}

uint16_t y=0;

void loop() {

//  Serial.println("VECTORDISPLAYTEST");
//  delay(100);
//  digitalWrite(LED_BUILTIN, 1);
//  delay(100);
  y = (y + 1)%240;
  SerialDisplay.foreColor(0xFF000000 | ((uint32_t)y<<16));
  SerialDisplay.line(0, y, 239, 319);
  //SerialDisplay.text(50, 20, String(y));  
  if (SerialDisplay.readMessage(&msg)) {
    
    switch(msg.what) {
      case MESSAGE_BUTTON:
        switch(msg.data.button) {
          case 'c':
            SerialDisplay.foreColor(0xFF00FF00);
            SerialDisplay.fillCircle(random(240),random(240),random(50));
            break;
          case 'l':
            SerialDisplay.clear();
            break;
        }
    }
  }
}

