#include <LiquidCrystal.h>
#include <stdarg.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

void p(char *fmt, ... ){
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        Serial.print(buf);
}

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 2
#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8
#define GAP_SIZE 1

#define LINE_WIDTH (DISPLAY_WIDTH * CHAR_WIDTH + (DISPLAY_WIDTH-1) * GAP_SIZE)
#define DISPLAY_LINES (DISPLAY_HEIGHT * CHAR_HEIGHT + (DISPLAY_HEIGHT-1) * GAP_SIZE)
#define BUFFER_SIZE ((LINE_WIDTH * DISPLAY_HEIGHT)/ 8 + 2)

#define SCREEN_BYTE(x, y) ((x + y*LINE_WIDTH)/8)
#define SCREEN_OFFSET(x, y) ((x + y*LINE_WIDTH)%8)

#define CHAR_PIXEL_X(x) (x*(CHAR_WIDTH+GAP_SIZE))
#define CHAR_PIXEL_Y(y, line) (y*(CHAR_HEIGHT+GAP_SIZE) + line)

byte customChar[CHAR_HEIGHT];

byte screen[BUFFER_SIZE];

byte offsetMasks[8]  = {0b00011111, 0b00111110, 0b01111100, 0b11111000, 0b11110000, 0b11100000, 0b11000000, 0b10000000};
byte offset2Masks[8] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00000011, 0b00000111, 0b00001111};

void draw_line(byte * write_byte, int x, int y, int line) 
{
  byte *read_byte = &screen[SCREEN_BYTE(CHAR_PIXEL_X(x), CHAR_PIXEL_Y(y, line))];

  int offset = SCREEN_OFFSET(CHAR_PIXEL_X(x), CHAR_PIXEL_Y(y, line));

  *write_byte = (offsetMasks[offset] & *read_byte) >> offset;
  read_byte++;
  *write_byte |= (offset2Masks[offset] & *read_byte) << (8-offset);
}

void draw_char(int x, int y)
{
  for (int i = 0; i < CHAR_HEIGHT; i++)
  {
    draw_line(customChar[i], x, y, i);
  }
  lcd.createChar(x + y*DISPLAY_WIDTH, customChar);
}

void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.home();

  p("LINE_WIDTH: %d\n", LINE_WIDTH);

  delay(5000);
  Serial.println("Started");

  // Filling Screen
  for (int y = 0; y < 10; y++)
  {
    for (int x = 0; x < 5; x++)
    {
      screen[SCREEN_BYTE(x, y)] |= (0b1 << SCREEN_OFFSET(x, y));
    }

  }

  Serial.println("Finished");


  for (int x = 0; x < DISPLAY_WIDTH; x++)
  {
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
      // p("Cord %d %d\n", x, y);
      draw_char(x, y);
      // delay(100);
    }
  }

  Serial.println("Finished pt2");

  

  // Top row
  for (int i = 0; i < 16; i++)
  {
    lcd.setCursor(i, 0);
    lcd.write(i);
  }

  

  // lcd.println("Testing");

  // Bottom row
  // lcd.setCursor(0, 1);
  // for (int i = 0; i < 16; i++)
  //   chars[i] = i+17;
  // lcd.write(chars);

  Serial.println("Finished pt3");
}

void loop() {}