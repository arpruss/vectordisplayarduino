#ifndef _VECTOR_DISPLAY_H
#define _VECTOR_DISPLAY_H

#include <Arduino.h>

#define VECTOR_DISPLAY_MESSAGE_SIZE 8
#define VECTOR_DISPLAY_MAX_STRING 256

#define ALIGN_LEFT 'l'
#define ALIGN_RIGHT 'r'
#define ALIGN_CENTER 'c'
#define ALIGN_TOP 't'
#define ALIGN_BOTTOM 'b'
#define ALIGN_BASELINE 'l'

#ifndef SERIAL_DISPLAY_SEND_DELAY
#define SERIAL_DISPLAY_SEND_DELAY 0
#endif

#define MESSAGE_DOWN   'D'
#define MESSAGE_UP     'U'
#define MESSAGE_MOVE   'M'
#define MESSAGE_BUTTON 'B'
#define MESSAGE_ACK    'A'

typedef uint32_t FixedPoint32;

struct VectorDisplayMessage {
    char what;
    char what2;
    union {
        uint8_t button;
        struct {
            int16_t x;
            int16_t y;
        } xy;
    } data;
};

class SerialDisplayClass : public Print {
private:
    const uint32_t MESSAGE_TIMEOUT = 2000;
    int gfxFontSize = 1;
    int curx;
    int cury;
    int readPos;
    int32_t curForeColor565 = -1;
    uint32_t lastMessageStart = 0;
    union {
        uint32_t color;
        uint16_t twoByte[4];
        struct {
            uint16_t x;
            uint16_t y;
            char text[VECTOR_DISPLAY_MAX_STRING];
        } __attribute__((packed)) xyText;
        struct {
            uint8_t c;
            char text[VECTOR_DISPLAY_MAX_STRING];
        } __attribute__((packed)) charText;
        struct {
            uint16_t width;
            uint16_t height;
        } __attribute__((packed)) coords;
        struct {
            char attr;
            uint8_t value;
        } __attribute__((packed)) attribute8;
        struct {
            char attr;
            uint16_t value;
        } __attribute__((packed)) attribute16;
        struct {
            char attr;
            uint32_t value;
        } __attribute__((packed)) attribute32;
        struct {
            char attr;
            uint16_t values[2];
        } __attribute__((packed)) attribute16x2;
        char text[VECTOR_DISPLAY_MAX_STRING];
    } args;
    uint32_t lastSend = 0;
public:    
    void sendCommand(char c, const void* arguments, int argumentsLength) {
#if SERIAL_DISPLAY_SEND_DELAY>0
        while(millis()-lastSend < SERIAL_DISPLAY_SEND_DELAY) ;
        lastSend = millis();
#endif        
        Serial.write(c);
        Serial.write(c^0xFF);
        if (argumentsLength > 0) 
            Serial.write((uint8_t*)arguments, argumentsLength);
        uint8_t sum = 0;
        for (int i = 0; i<argumentsLength; i++)
        sum += ((uint8_t*)arguments)[i];
        Serial.write(sum^0xFF);
    }

    void line(int x1, int y1, int x2, int y2) {
        args.twoByte[0] = x1;
        args.twoByte[1] = y1;
        args.twoByte[2] = x2;
        args.twoByte[3] = y2;
        sendCommand('L', &args, 8);
    }
    
    void fillRectangle(int x1, int y1, int x2, int y2) {
        args.twoByte[0] = x1;
        args.twoByte[1] = y1;
        args.twoByte[2] = x2;
        args.twoByte[3] = y2;
        sendCommand('R', &args, 8);
    }
    
    void fillCircle(int x, int y, int r) {
        args.twoByte[0] = x;
        args.twoByte[1] = y;
        args.twoByte[2] = r;
        sendCommand('J', &args, 6);
    }
    
    void circle(int x, int y, int r) {
        args.twoByte[0] = x;
        args.twoByte[1] = y;
        args.twoByte[2] = r;
        sendCommand('I', &args, 6);
    }
    
    void point(int x, int y) {
        args.twoByte[0] = x;
        args.twoByte[1] = y;
        sendCommand('P', &args, 2);
    }
    
    // 32-bit fixed point
    void textSize(FixedPoint32 s) {
        args.attribute32.attr = 's';
        args.attribute32.value = s;
    }
    
    void text(int x, int y, const char* str) {
        args.xyText.x = x;
        args.xyText.y = y;
        strncpy(args.xyText.text, str, VECTOR_DISPLAY_MAX_STRING);
        args.xyText.text[VECTOR_DISPLAY_MAX_STRING-1] = 0;
        sendCommand('T', &args, 4+strlen(args.xyText.text)+1);
    }
    
    void deleteButton(byte command) {
        sendCommand('D', &command, 1);
    }

    void addButton(uint8_t command, const char* str) {
        args.charText.c = command;
        strncpy(args.charText.text, str, VECTOR_DISPLAY_MAX_STRING);
        args.charText.text[VECTOR_DISPLAY_MAX_STRING-1] = 0;
        sendCommand('U', &args, 1+strlen(args.charText.text)+1);
    }

    void addButton(uint8_t command, String str) {
        addButton(command, str.c_str());
    }

    void text(int x, int y, String str) {
        text(x,y,str.c_str());
    }
    
    void message(const char* str) {
        strncpy(args.text, str, VECTOR_DISPLAY_MAX_STRING);
        args.text[VECTOR_DISPLAY_MAX_STRING-1] = 0;
        sendCommand('M', &args, strlen(args.text)+1);
    }
    
    void message(String text) {
        message(text.c_str());
    }
    
    void foreColor(uint32_t color) {
        args.attribute32.attr = 'f';
        args.attribute32.value = color;
        sendCommand('B', &args, 5);
        curForeColor565 = -1;
    }

    void backColor(uint32_t color) {
        args.attribute32.attr = 'b';
        args.attribute32.value = color;
        sendCommand('B', &args, 5);
    }

    void textBackColor(uint32_t color) {
        args.attribute32.attr = 'k';
        args.attribute32.value = color;
        sendCommand('B', &args, 5);
    }
    
    void foreColor565(uint16_t color) {
        args.attribute16.attr = 'f';
        args.attribute16.value = color;
        sendCommand('A', &args, 3);
        curForeColor565 = color;
    }

    void backColor565(uint16_t color) {
        args.attribute16.attr = 'b';
        args.attribute16.value = color;
        sendCommand('A', &args, 3);
    }

    void textBackColor565(uint16_t color) {
        args.attribute16.attr = 'k';
        args.attribute16.value = color;
        sendCommand('A', &args, 3);
    }
    
    void setThickness(FixedPoint32 thickness) {
        args.attribute32.attr = 't';
        args.attribute32.value = thickness;
        sendCommand('B', &args, 5);
    }

#ifdef SUPPORT_FLOATING_POINT
    void setThickness(double thickness) {
        setThickness((uint32_t)(thickness * 65536+0.5));
    } 
#endif    

    void clear() {
        sendCommand('C', NULL, 0);
    }

    void ack() {
        sendCommand('K', NULL, 0);
    }

    void reset() {
        readPos = 0;
            
        bool done = false;
        do {
            char msg[8];
            uint32_t t0 = millis();
            
            sendCommand('E', NULL, 0);
            
            while ((millis()-t0) < 500) {
                if (readMessage((VectorDisplayMessage*)msg) && !strncmp(msg, "Acknwldg", 8)) {
                    done = true;
                    break;
                }
            }
            
        } while(!done);        
    }

    void coordinates(int width, int height) {
        args.attribute16x2.attr = 'f';
        args.attribute16x2.values[0] = width;
        args.attribute16x2.values[1] = height;
        sendCommand('B', &args, 5);
    }
    
    void textHorizontalAlign(char hAlign) {
        args.attribute8.attr = 'h';
        args.attribute8.value = hAlign;
        sendCommand('Y', &args, 2);
    }

    void textVerticalAlign(char hAlign) {
        args.attribute8.attr = 'v';
        args.attribute8.value = hAlign;
        sendCommand('Y', &args, 2);
    }

    void textOpaqueBackground(bool opaque) {
        args.attribute8.attr = 'o';
        args.attribute8.value = opaque ? 1 : 0;
        sendCommand('Y', &args, 2);
    }
    
    void textBold(bool bold) {
        args.attribute8.attr = 'b';
        args.attribute8.value = bold ? 1 : 0;
        sendCommand('Y', &args, 2);
    }
    
    /* Roughly compatible with Adafruit GFX */
    void setRotation(uint8_t r) {
        args.attribute8.attr = 'r';
        args.attribute8.value = r;
        sendCommand('Y', &args, 2);
    }
    
    void setTextSize(uint8_t size) {
        gfxFontSize = size;
        textSize((FixedPoint32)size * 65536);
    }
    
    void setCursor(int16_t x, int16_t y) {
        curx = x;
        cury = y;
    }
    
    virtual size_t write(uint8_t c) {
        char s[2];
        s[0] = c;
        s[1] = 0;
        text(curx, cury, s);
        curx += 5*gfxFontSize;
        return 1;
    }

    virtual size_t write(const char* s) {
        size_t l = strlen(s);
        text(curx, cury, s);
        curx += 5*gfxFontSize*l;
        return l;
    }
    
    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        point(x, y);
    }

    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        line(x,y,x+w-1,y);
        line(x+w-1,y,x+w-1,y+h-1);
        line(x+w-1,y+h-1,x,y+h-1);
        line(x,y+h-1,x,y);
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        fillRectangle(x,y,x+w-1,y+h-1);
    }
    
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        line(x,y,x+w,y);
    }
    
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        line(x,y,x,y+h);
    }
    
    void fillScreen(uint16_t color) {
        backColor565(color);
        clear();
        backColor(0xFF000000);
    }
    
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        circle(x,y,r);
    }

    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        fillCircle(x,y,r);
    }
    
    void begin() {
        reset();
    }
    
    bool readMessage(VectorDisplayMessage* msg) {
        uint8_t* readBuf = (uint8_t*) msg;
        if (Serial.available()) {
            if (0 < readPos && millis()-lastMessageStart > MESSAGE_TIMEOUT)
                readPos = 0;

            uint8_t c = Serial.read();
            if (2 <= readPos) {
                readBuf[readPos++] = c;
                if (readPos >= VECTOR_DISPLAY_MESSAGE_SIZE) {
                    readPos = 0;
                    return true;
                }
                return false;
            }
            if (1 <= readPos) {
                if ( (*readBuf == 'U' && c == 'P') ||
                     (*readBuf == 'D' && c == 'N') ||
                     (*readBuf == 'M' && c == 'V') ||
                     (*readBuf == 'B' && c == 'T') ||
                     (*readBuf == 'A' && c == 'K') 
                     ) {
                    readBuf[readPos++] = c;
                    return false;
                }
                readPos = 0;
            }
            if (c == 'U' || c == 'D' || c == 'M' || c == 'B' || c == 'A') {
                readBuf[readPos++] = c;
                lastMessageStart = millis();
            }
        }
        return false;
    }
};

extern SerialDisplayClass SerialDisplay;

#endif
