#ifndef _VECTOR_DISPLAY_H
#define _VECTOR_DISPLAY_H

#include <Arduino.h>

#ifdef ESP8266
# include <ESP8266WiFi.h>
#endif

#define VECTOR_DISPLAY_MESSAGE_SIZE 8
#define VECTOR_DISPLAY_MAX_STRING 256

#define VECTOR_DISPLAY_DEFAULT_WIDTH  240
#define VECTOR_DISPLAY_DEFAULT_HEIGHT 320

#define ALIGN_LEFT 'l'
#define ALIGN_RIGHT 'r'
#define ALIGN_CENTER 'c'
#define ALIGN_TOP 't'
#define ALIGN_BOTTOM 'b'
#define ALIGN_BASELINE 'l'

#ifndef VECTOR_DISPLAY_SEND_DELAY
#define VECTOR_DISPLAY_SEND_DELAY 0
#endif

#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFD20      /* 255, 165,   0 */
#define TFT_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define TFT_PINK        0xF81F

// Color definitions for backwards compatibility
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

#define MESSAGE_DOWN   'D'
#define MESSAGE_UP     'U'
#define MESSAGE_MOVE   'M'
#define MESSAGE_BUTTON 'B'
#define MESSAGE_ACK    'A'

typedef uint32_t FixedPoint32;
#define TO_FP32(f) ((uint32_t)((f)*65536. + 0.5))

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
} __attribute__((packed));

class VectorDisplayClass : public Print {
private:
    static const uint32_t MAX_BUFFER = (uint32_t)1024*256;
    static const uint32_t MESSAGE_TIMEOUT = 3000;
    static const uint8_t FLAG_LOW_ENDIAN_BITMAP = 1;
    static const uint8_t FLAG_HAVE_MASK = 2;
    static const uint8_t FLAG_PAD_BYTE = 4;

    int gfxFontSize = 1;
    int curx = 0;
    int cury = 0;
    int readPos = 0;
    int32_t curForeColor565 = -1;
    uint32_t lastMessageStart = 0;
    uint16_t curTextColor565 = 0xFFFF;
    int pointerX;
    int pointerY;
    int curWidth = VECTOR_DISPLAY_DEFAULT_WIDTH;
    int curHeight = VECTOR_DISPLAY_DEFAULT_HEIGHT;
    uint8_t curRotation = 0;
    bool pointerDown;
    bool wrap = 1;
    uint16_t polyLineCount;
    uint8_t polyLineSum;
    uint32_t delayTime = 0;
    
    uint8_t readBuf[VECTOR_DISPLAY_MESSAGE_SIZE];
    union {
        uint32_t color;
        uint16_t twoByte[9];
        struct {
            uint16_t x;
            uint16_t y;
            char text[VECTOR_DISPLAY_MAX_STRING+1];
        } __attribute__((packed)) xyText;
        struct {
            uint16_t endianness;
            uint16_t width;
            uint16_t height;
            FixedPoint32 aspectRatio;
            uint16_t reserved[3];
        } __attribute__((packed)) initialize;
        struct {
            uint8_t c;
            char text[VECTOR_DISPLAY_MAX_STRING+1];
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
            uint32_t length;
            uint8_t depth;
            uint8_t flags;
            uint16_t x;
            uint16_t y;
            uint16_t w;
            uint16_t h;
            uint32_t foreColor; // only if depth==1
            uint32_t backColor; // only if depth==1
        } __attribute__((packed)) bitmap;
        struct {
            uint16_t x1;
            uint16_t y1;
            uint16_t x2;
            uint16_t y2;
            uint16_t r;
            uint8_t filled;
        } __attribute__((packed)) roundedRectangle;
        struct {
            char attr;
            uint16_t values[2];
        } __attribute__((packed)) attribute16x2;        
        uint8_t bytes[VECTOR_DISPLAY_MAX_STRING+1];
        char text[VECTOR_DISPLAY_MAX_STRING+1];
    } args;
    uint32_t lastSend = 0;
    
private:    
    inline void sendDelay() {
        if (delayTime>0) {
            while(millis()-lastSend < delayTime) ;
            lastSend = millis();
        }
    }
    
public:    
    void setDelay(uint32_t delayMillis) {
        delayTime = delayMillis;
        lastSend = millis();
    }
    
    virtual void remoteFlush() {
        while(remoteAvailable()) 
            remoteRead();
    }
    virtual int remoteRead() = 0; // must be non-blocking
    virtual void remoteWrite(uint8_t c) = 0;
    virtual void remoteWrite(const void* data, size_t n) = 0;
    virtual size_t remoteAvailable() = 0;

    void sendCommand(char c, const void* arguments, int argumentsLength) {
        sendDelay();
        remoteWrite(c);
        remoteWrite(c^0xFF);
        if (argumentsLength > 0) 
            remoteWrite((uint8_t*)arguments, argumentsLength);
        uint8_t sum = 0;
        for (int i = 0; i<argumentsLength; i++)
            sum += ((uint8_t*)arguments)[i];
        remoteWrite((uint8_t)(sum^0xFF));
    }
   
    
    void sendCommandWithAck(char c, const void* arguments, int argumentsLength) {
       readPos = 0;        
       bool done = false;
        do {
            uint32_t t0 = millis();
            
            sendCommand(c, arguments, argumentsLength);
            
            while ((millis()-t0) < 500) {
                if (readMessage(NULL) && !memcmp(readBuf, "Acknwld", 7) && readBuf[7]==c) {
                    done = true;
                    break;
                }
            }
            
        } while(!done);        
    }    
    
    uint16_t width() {
        return (curRotation%2)?curHeight:curWidth;
    }
    
    uint16_t height() {
        return (curRotation%2)?curWidth:curHeight;
    }

    uint8_t sumBytes(void* data, int length) {
        uint8_t* p = (uint8_t*)data;
        uint8_t s = 0;
        while(length-- > 0)
            s += *p++;
        return s;
    }
    
    void startPoly(char c, uint16_t n) {
        polyLineCount = n;
        remoteWrite(c);
        remoteWrite(c^0xFF);
        args.twoByte[0] = n;
        remoteWrite((uint8_t*)&args, 2);
        polyLineSum = args.bytes[0] + args.bytes[1];
    }

    void startFillPoly(uint16_t n) {
        startPoly('N', n);
    }

    void startPolyLine(uint16_t n) {
        startPoly('O', n);
    }

    void addPolyLine(int16_t x, int16_t y) {
        if (polyLineCount>0) {
            args.twoByte[0] = x;
            args.twoByte[1] = y;
            remoteWrite((uint8_t*)&args, 4);
            polyLineSum += args.bytes[0] + args.bytes[1] + args.bytes[2] + args.bytes[3];
            polyLineCount--;
            if (polyLineCount == 0) {
                remoteWrite(0xFF^polyLineSum);
            }
        }
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
    
    void roundedRectangle(int x1, int y1, int x2, int y2, int r, boolean fill) {
        args.roundedRectangle.filled = fill ? 1 : 0;
        args.roundedRectangle.x1 = x1;
        args.roundedRectangle.x2 = x2;
        args.roundedRectangle.y1 = y1;
        args.roundedRectangle.y2 = y2;
        args.roundedRectangle.r = r;
        sendCommand('Q', &args, 11);
    }
    
    void roundedRectangle(int x1, int y1, int x2, int y2, int r) {
        roundedRectangle(x1,y1,x2,y2,r,false);
    }
    
    void fillRoundedRectangle(int x1, int y1, int x2, int y2, int r) {
        roundedRectangle(x1,y1,x2,y2,r,true);
    }
    
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
        args.twoByte[0] = x1;
        args.twoByte[1] = y1;
        args.twoByte[2] = x2;
        args.twoByte[3] = y2;
        args.twoByte[4] = x3;
        args.twoByte[5] = y3;
        sendCommand('G', &args, 12);
    }
    
/*    void initialize() {
        args.twoByte[0] = 0x1234; // endianness detector
        args.twoByte[1] = 0;
        sendCommandWithAck('H', &args, 4);
    } */
    
    void initialize(int w=VECTOR_DISPLAY_DEFAULT_WIDTH, int h=VECTOR_DISPLAY_DEFAULT_HEIGHT) {
        args.initialize.endianness = 0x1234; // endianness detector
        args.initialize.width = w;
        args.initialize.height = h;
        args.initialize.aspectRatio = TO_FP32(1.);
        args.initialize.reserved[0] = 0;
        args.initialize.reserved[1] = 0;
        args.initialize.reserved[2] = 0;
        curWidth = w;
        curHeight = h;
        
        sendCommandWithAck('Z', &args, 16);
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
        sendCommand('P', &args, 4);
    }
    
    // 32-bit fixed point
    void textSize(FixedPoint32 s) {
        args.attribute32.attr = 's';
        args.attribute32.value = s;
        sendCommand('B', &args, 5);
    }
    
    void text(int x, int y, const char* str, int n) {
        args.xyText.x = x;
        args.xyText.y = y;
        if (n>VECTOR_DISPLAY_MAX_STRING)
            n = VECTOR_DISPLAY_MAX_STRING;
        strncpy(args.xyText.text, str, n);
        args.xyText.text[n] = 0;
        sendCommand('T', &args, 4+strlen(args.xyText.text)+1);
    }
    
    void text(int x, int y, const char* str) {
        text(x, y, str, strlen(str));
    }
    
    void text(int x, int y, String str) {
        text(x,y,str.c_str(), str.length());
    }
    
    void deleteButton(byte command) {
        sendCommand('D', &command, 1);
    }

    void addButton(uint8_t command, const char* str) {
        args.charText.c = command;
        strncpy(args.charText.text, str, VECTOR_DISPLAY_MAX_STRING);
        args.charText.text[VECTOR_DISPLAY_MAX_STRING] = 0;
        sendCommand('U', &args, 1+strlen(args.charText.text)+1);
    }

    void addButton(uint8_t command, String str) {
        addButton(command, str.c_str());
    }

    void toast(const char* str, unsigned n) {
        if (VECTOR_DISPLAY_MAX_STRING < n)
            n = VECTOR_DISPLAY_MAX_STRING;
        strncpy(args.text, str, n);
        args.text[n] = 0;
        sendCommand('M', &args, n+1);
    }
    
    void toast(const char* str) {
        toast(str, strlen(str));
    }
    
    void toast(String text) {
        toast(text.c_str(), text.length());
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
    
    void rounded(uint8_t value) {
        args.attribute8.attr = 'n';
        args.attribute8.value = value ? 1 : 0;
        sendCommand('Y', &args, 2);
    }
    
    void thickness(FixedPoint32 t) {
        args.attribute32.attr = 't';
        args.attribute32.value = t;
        sendCommand('B', &args, 5);
    }

    void pixelAspectRatio(FixedPoint32 a) {
        args.attribute32.attr = 'a';
        args.attribute32.value = a;
        sendCommand('B', &args, 5);
    }

#ifdef SUPPORT_FLOATING_POINT
    inline void setThickness(double thickness) {
        setThickness(TO_FP32(thickness));
    } 

    inline void setPixelAspectRatio(double aspect) {
        setThickness(TO_FP32(aspect));
    } 
#endif    

    void clear() {
        sendCommand('C', NULL, 0);
    }

    void update() {
        sendCommand('F', NULL, 0);
    }

/*    void reset() {
        sendCommandWithAck('E', NULL, 0);
    } */

    void coordinates(int width, int height) {
        args.attribute16x2.attr = 'c';
        curWidth = width;
        curHeight = height;
        args.attribute16x2.values[0] = width;
        args.attribute16x2.values[1] = height;
        sendCommandWithAck('B', &args, 5);
    }
    
    void continuousUpdate(bool value) {
        args.attribute8.attr = 'c';
        args.attribute8.value = value ? 1 : 0;
        sendCommand('Y', &args, 2);
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
    
    bool isTouchDown() {
        return pointerDown;
    }
    
    int getTouchX() {
        return pointerX;
    }
    
    int getTouchY() {
        return pointerY;
    }
        
    bool readMessage(VectorDisplayMessage* msg) {
        while (remoteAvailable()) {
            uint8_t c = remoteRead();

            if (0 < readPos && millis()-lastMessageStart > MESSAGE_TIMEOUT)
                readPos = 0;

            if (2 <= readPos) {
                readBuf[readPos++] = c;
                if (readPos >= VECTOR_DISPLAY_MESSAGE_SIZE) {
                    readPos = 0;
                    if (msg != NULL) 
                        memcpy(msg, readBuf, sizeof(VectorDisplayMessage));
                    else
                        msg = (VectorDisplayMessage*)readBuf;
                                        
                    if (msg->what == MESSAGE_DOWN || msg->what == MESSAGE_UP || msg->what == MESSAGE_MOVE) {
                        pointerDown = msg->what != MESSAGE_UP;
                        pointerX = msg->data.xy.x;
                        pointerY = msg->data.xy.y;
                    }
                    return true;
                }
                continue;
            }
            
            if (1 <= readPos) {
                if ( (*readBuf == 'U' && c == 'P') ||
                     (*readBuf == 'D' && c == 'N') ||
                     (*readBuf == 'M' && c == 'V') ||
                     (*readBuf == 'B' && c == 'T') ||
                     (*readBuf == 'A' && c == 'c') 
                     ) {
                    readBuf[readPos++] = c;
                    continue;
                }
                readPos = 0;
            }
            if (readPos == 0 && (c == 'U' || c == 'D' || c == 'M' || c == 'B' || c == 'A')) {
                readBuf[readPos++] = c;
                lastMessageStart = millis();
            }
        }
        return false;
    }
    
    uint32_t color565To8888(uint16_t c) {
        return 0xFF000000 | ((((c>>11) & 0x1F) * 255 / 0x1F) << 16) | ((((c>>5) & 0x3F) * 255 / 0x3F) << 8) | ((c & 0x1F) * 255 / 0x1F);
    }

    void bitmap(int16_t x, int16_t y, const uint8_t bmp[],
      int16_t w, int16_t h, uint32_t foreColor, uint32_t backColor, uint8_t flags=0) /* PROGMEM */ {
        uint32_t size = (flags & FLAG_PAD_BYTE) ? ((uint32_t)w+7)/8*h : ((uint32_t)w*h+7)/8;
        if (size + 22 + 1 > MAX_BUFFER)
            return;

        sendDelay();
        remoteWrite('K');
        remoteWrite('K'^0xFF);
        args.bitmap.length = 8+size;
        args.bitmap.depth = 1;
        args.bitmap.flags = flags;
        args.bitmap.x = x;
        args.bitmap.y = y;
        args.bitmap.w = w;
        args.bitmap.h = h;
        args.bitmap.foreColor = foreColor;
        args.bitmap.backColor = backColor;
        uint8_t sum = sumBytes(&args, 22);
        remoteWrite(&args,22);
        for (uint32_t i=0; i<size; i++) {
            uint8_t c = pgm_read_byte_near(bmp+i);
            remoteWrite(c);
            sum += c;
        }
        remoteWrite(sum^0xFF);
    }

    void bitmap(int16_t x, int16_t y, uint8_t *bmp,
      int16_t w, int16_t h, uint32_t foreColor, uint32_t backColor, uint8_t flags=0) {
        uint32_t size = (flags & FLAG_PAD_BYTE) ? ((uint32_t)w+7)/8*h : ((uint32_t)w*h+7)/8;
        if (size + 22 + 1 > MAX_BUFFER)
            return;

        sendDelay();
        remoteWrite('K');
        remoteWrite('K'^0xFF);
        args.bitmap.length = 8+size;
        args.bitmap.depth = 1;
        args.bitmap.flags = flags;
        args.bitmap.x = x;
        args.bitmap.y = y;
        args.bitmap.w = w;
        args.bitmap.h = h;
        args.bitmap.foreColor = foreColor;
        args.bitmap.backColor = backColor;
        remoteWrite(&args,22);
        remoteWrite(bmp,size);
        uint8_t sum = sumBytes(&args, 22) + sumBytes((void*)bmp, size);
        remoteWrite(sum^0xFF);
    }

    /* The following are meant to be compatible with Adafruit GFX */
    void cp437(boolean s) {
        args.attribute8.attr = 'i';
        args.attribute8.value = s ? 1 : 0;
        sendCommand('Y', &args, 2);
    }
    
    void setRotation(uint8_t r) {
        args.attribute8.attr = 'r';
        args.attribute8.value = r;
        curRotation = r & 3;
        sendCommand('Y', &args, 2);
    }
    
    void setTextSize(uint8_t size) {
        gfxFontSize = size;
        textSize((FixedPoint32)size * 8 * 65536);
    }
    
    void setTextColor(uint16_t f, uint16_t b) {
        curTextColor565 = f;
        textBackColor565(b);
        textOpaqueBackground(true);
    }
    
    void setTextColor(uint16_t f) {
        curTextColor565 = f;
        textOpaqueBackground(false);
    }
    
    void setCursor(int16_t x, int16_t y) {
        curx = x;
        cury = y;
    }
    
    void setTextWrap(boolean w) {
        wrap = w;
    }

        // TODO: fix back color handling
    virtual size_t write(uint8_t c) {
        if (curTextColor565 != curForeColor565) {
            foreColor565(curTextColor565);
        }
        if (wrap && curx + 5*gfxFontSize>width()) {
            curx = 0;
            cury += 8*gfxFontSize;
        }
        text(curx, cury, (char*)&c, 1);
        curx += 5*gfxFontSize;
        return 0;
    }

        // TODO: fix back color handling
    virtual size_t write(const char* s) {
        if (curTextColor565 != curForeColor565) {
            foreColor565(curTextColor565);
        }
        int l = strlen(s);
        int w = width();
        if (!wrap || curx + 5*gfxFontSize*l <= w) {
            text(curx, cury, s);
            curx += 5*gfxFontSize*l;
        }
        else {
            while(l>0) {
                int end = ((int)w-curx)/(5*gfxFontSize);
                if (end <= 0) {
                    curx = 0;
                    cury += 8*gfxFontSize;
                    end = w/(5*gfxFontSize);
                }
                if (end > l)
                    end = l;
                text(curx, cury, s, end);
                l-=end;
                s += end;
                curx = 5*gfxFontSize*end;
            }
        }
        return 0;
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
    
    void drawLine(int16_t x, int16_t y, int16_t x2, int16_t y2, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        line(x,y,x2,y2);
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
    
    virtual void begin(int width=VECTOR_DISPLAY_DEFAULT_WIDTH, int height=VECTOR_DISPLAY_DEFAULT_HEIGHT) {
        remoteFlush();
        initialize(width, height);
    }
    
    virtual void end() {
    }
    
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        fillTriangle(x0,y0,x1,y1,x2,y2);
    }

    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        line(x0,y0,x1,y1);
        line(x1,y1,x2,y2);
        line(x2,y2,x0,y0);
    }

    void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        roundedRectangle(x0,y0,w,h,radius,false);
    }
      
    void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color) {
        if (color != curForeColor565) {
            foreColor565(color);
        }
        roundedRectangle(x0,y0,w,h,radius,true);          
    }

    // bitmap functions not tested
    void drawBitmap(int16_t x, int16_t y, const uint8_t bmp[],
      int16_t w, int16_t h, uint16_t color) /* PROGMEM */ {
        bitmap(x,y,bmp,w,h,color565To8888(color),0); // transparent background
    }

    void drawBitmap(int16_t x, int16_t y, uint8_t *bmp,
      int16_t w, int16_t h, uint16_t color) {
        bitmap(x,y,bmp,w,h,color565To8888(color),0); // transparent background
    }

    void drawBitmap(int16_t x, int16_t y, const uint8_t bmp[],
      int16_t w, int16_t h, uint16_t color, uint16_t bg) {
        bitmap(x,y,bmp,w,h,color565To8888(color),color565To8888(bg)); 
    }
    
    void drawBitmap(int16_t x, int16_t y, uint8_t *bmp,
      int16_t w, int16_t h, uint16_t color, uint16_t bg) {
        bitmap(x,y,bmp,w,h,color565To8888(color),color565To8888(bg));       
    }

    void drawXBitmap(int16_t x, int16_t y, const uint8_t bmp[],
      int16_t w, int16_t h, uint16_t color) {
        bitmap(x,y,bmp,w,h,color565To8888(color),0,FLAG_PAD_BYTE|FLAG_LOW_ENDIAN_BITMAP);               
    }

      /* the following Adafruit GFX APIs are not implemented at present */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      uint16_t color) {}
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      int16_t delta, uint16_t color) {}
    void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bmp[],
      int16_t w, int16_t h) {}
    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bmp,
      int16_t w, int16_t h) {}
    void drawGrayscaleBitmap(int16_t x, int16_t y,
      const uint8_t bmp[], const uint8_t mask[],
      int16_t w, int16_t h) {}
    void drawGrayscaleBitmap(int16_t x, int16_t y,
      uint8_t *bmp, uint8_t *mask, int16_t w, int16_t h) {}
    void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bmp[],
      int16_t w, int16_t h) {}
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bmp,
      int16_t w, int16_t h) {}
    void drawRGBBitmap(int16_t x, int16_t y,
      const uint16_t bmp[], const uint8_t mask[],
      int16_t w, int16_t h) {}
    void drawRGBBitmap(int16_t x, int16_t y,
      uint16_t *bmp, uint8_t *mask, int16_t w, int16_t h) {}
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
      uint16_t bg, uint8_t size) {}
    void setFont(const void /*GFXfont*/ *f = NULL) {}
    void getTextBounds(const char *string, int16_t x, int16_t y,
      int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {}
    void getTextBounds(const void /*__FlashStringHelper*/ *s, int16_t x, int16_t y,
      int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {}    
#pragma GCC diagnostic pop
};

class SerialDisplayClass : public VectorDisplayClass {
    private:
        Stream& s;
        const bool doSerialBegin;
    
    public:
        virtual int remoteRead() override {
            return s.read();
        }
        
        virtual void remoteWrite(uint8_t c) override {
            s.write(c);
        }
        
        virtual void remoteWrite(const void* data, size_t n) override {
            s.write((uint8_t*)data, n);
        }

        /* only works with the Serial object; do not call externally without it */
        void begin(uint32_t speed, int width=VECTOR_DISPLAY_DEFAULT_WIDTH, int height=VECTOR_DISPLAY_DEFAULT_HEIGHT) {
#ifndef NO_SERIAL
            if (doSerialBegin) {
                Serial.begin(speed);
                while(!Serial) ;
            }
#endif
            VectorDisplayClass::begin(width, height);
        }
        
        virtual void begin(int width=VECTOR_DISPLAY_DEFAULT_WIDTH, int height=VECTOR_DISPLAY_DEFAULT_HEIGHT) override {
            begin(115200, width, height);
        }
        
        virtual size_t remoteAvailable() override {
            return s.available();
        }        
        
#ifndef NO_SERIAL
        SerialDisplayClass() : s(Serial), doSerialBegin(true)  {}
#endif

        SerialDisplayClass(Stream& _s) : s(_s), doSerialBegin(false) {}
};

#ifdef ESP8266
class WiFiDisplayClass : public SerialDisplayClass {
    private:
        WiFiClient client;
    public:    
        bool begin(const char* host, int width=VECTOR_DISPLAY_DEFAULT_WIDTH, int height=VECTOR_DISPLAY_DEFAULT_HEIGHT) {
            VectorDisplayClass::begin(width, height);
            return client.connect(host, 7788);
        }
        
        virtual void end() override {
            VectorDisplayClass::end();
            client.stop();
        }
        
        WiFiDisplayClass() : SerialDisplayClass(client) {            
        }
};
#endif
        
#endif
