#include <VectorDisplay.h>

VectorDisplay<SerialUSBClass> SerialDisplay(Serial);

uint8_t initGraphics[] = { 'C', 'C'^0xFF, 'Z', 'Z'^0xFF, 0xFF, 0x01, 0xFF, 0x01, 1, 'L', 'L'^0xFF, 0, 0, 0,0, (byte)0xFF, 0x01, (byte)0xFF, 0x01, 'T', 'T'^0xFF, 10, 
  0, 10, 0, 'A', 'B', 'C', 0, 'M'^0xFF, 'H', 'e', 'l', 'l', 'o', 0};

void sendCommand(char c, const void* arguments, int argumentsLength) {
  Serial.write(c);
  Serial.write(c^0xFF);
  Serial.write((uint8_t*)arguments, argumentsLength);
  uint8_t sum = 0;
  for (int i = 0; i<argumentsLength; i++)
    sum += ((uint8_t*)arguments)[i];
  Serial.write(sum^0xFF);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);

  delay(5000);
  sendCommand('C', "", 0);
}

struct {
  uint16_t x0;
  uint16_t y0;
  uint16_t x1;
  uint16_t y1;
} __attribute__((packed)) line = { 0, 0, 511, 511 };

uint16_t y=0;

void loop() {

  digitalWrite(LED_BUILTIN, 0);
//  delay(100);
  digitalWrite(LED_BUILTIN, 1);
//  delay(100);
//  Serial.write("MHello", 7);
  y++;
  line.y0 = (line.y0+1)%512;
//  SerialDisplay.sendCommand('L', (void*)&line, sizeof(line));
  SerialDisplay.line(0, y, 511, 511);
  SerialDisplay.text(0, 0, String(y));
  delay(5);
}

