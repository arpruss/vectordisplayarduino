#include <VectorDisplay.h>

VectorDisplayMessage msg;

void setup() {
  SerialDisplay.begin();
  SerialDisplay.addButton('c', "Circle");
  SerialDisplay.addButton('o', "Color");
}

void loop() {
  if (SerialDisplay.readMessage(&msg) && msg.what == MESSAGE_BUTTON) {
        if (msg.data.button == 'c') {
  		SerialDisplay.fillCircle(random(240),random(320),random(50));
  	}
  	else if (msg.data.button == 'o') {
  		SerialDisplay.foreColor(0xFF000000 | (random(256)<<16) | (random(256)<<8) | random(256));
  	}
  }
}