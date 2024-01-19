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

byte reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

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

#define DISPLAY_WIDTH 4
#define DISPLAY_HEIGHT 2
#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8
#define GAP_SIZE 1

#define LINE_WIDTH (DISPLAY_WIDTH * CHAR_WIDTH + (DISPLAY_WIDTH-1) * GAP_SIZE)
#define DISPLAY_LINES (DISPLAY_HEIGHT * CHAR_HEIGHT + (DISPLAY_HEIGHT-1) * GAP_SIZE)
#define BUFFER_SIZE ((LINE_WIDTH * DISPLAY_LINES)/ 8 + 2)

#define SCREEN_BYTE(x, y) ((x + y*LINE_WIDTH)/8)
#define SCREEN_OFFSET(x, y) ((x + y*LINE_WIDTH)%8)

#define CHAR_PIXEL_X(x) (x*(CHAR_WIDTH+GAP_SIZE))
#define CHAR_PIXEL_Y(y, line) (y*(CHAR_HEIGHT+GAP_SIZE) + line)

#define MID_SCREEN_X ((float) LINE_WIDTH / 2.0f - 1)
#define MID_SCREEN_Y ((float) DISPLAY_LINES / 2.0f - 1)

byte customChar[CHAR_HEIGHT];

byte screen[BUFFER_SIZE];

byte offsetMasks[8]  = {0b00011111, 0b00111110, 0b01111100, 0b11111000, 0b11110000, 0b11100000, 0b11000000, 0b10000000};
byte offset2Masks[8] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00000011, 0b00000111, 0b00001111};

int frame = 0;

void draw_line(byte * write_byte, int x, int y, int line) 
{
  byte *read_byte = &screen[SCREEN_BYTE(CHAR_PIXEL_X(x), CHAR_PIXEL_Y(y, line))];

  int offset = SCREEN_OFFSET(CHAR_PIXEL_X(x), CHAR_PIXEL_Y(y, line));

  *write_byte = (offsetMasks[offset] & *read_byte) >> offset;
  read_byte++;
  *write_byte |= (offset2Masks[offset] & *read_byte) << (8-offset);
  *write_byte = reverse(*write_byte) >> 3;
}

void draw_char(int x, int y)
{
  for (int i = 0; i < CHAR_HEIGHT; i++)
  {
    draw_line(&customChar[i], x, y, i);
    // p("Line: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(customChar[i]));
  }
  lcd.createChar(x + y*DISPLAY_WIDTH, customChar);
}

void draw_screen()
{
  for (int i = 0; i < 8; i++)
    draw_char(i%4, i/4);

  for (int i = 0; i < 8; i++)
  {
    lcd.setCursor(i%4, i/4);
    lcd.write(i);
  }
}

void print_screen()
{
  for (int y = 0; y < DISPLAY_LINES; y++)
  {
    for (int x = 0; x < LINE_WIDTH; x++)
    {
      // p("X: %d, Y: %d, Byte: %d, offset: %d", x, y, )
      Serial.print(((screen[SCREEN_BYTE(x, y)] >> SCREEN_OFFSET(x, y)) & 0b1 == 0b1) ? "1":"0");
    }
    Serial.println();
  }
}

void clear_screen()
{
  for (int i = 0; i < BUFFER_SIZE; i++)
    screen[i] = 0b0;
}

void render_circle(float cx, float cy, float r)
{
  
  for (int y = 0; y < DISPLAY_LINES; y++)
  {
    for (int x = 0; x < LINE_WIDTH; x++)
    {
      float distance = hypotf(fabs(cy - y)*2.0, fabs(cx - x));
      if (distance <= r)
        screen[SCREEN_BYTE(x, y)] |= (0b1 << SCREEN_OFFSET(x, y));
    }
  }
}

void render()
{
  clear_screen();

  float circle_x = ((float) LINE_WIDTH / 2.0f - 1) + 3 * sin(frame*.25);
  float circle_y = ((float) DISPLAY_LINES / 2.0f - 1) + 3 * sin((frame+3)*.4);

  // Filling Screen
  float max_distance = hypotf(MID_SCREEN_X, MID_SCREEN_Y);
  render_circle(circle_x, circle_y, max_distance*.5);

  draw_screen();
}

void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.home();

  // p("LINE_WIDTH: %d\n", LINE_WIDTH);

  // delay(5000);

  
}

void loop() 
{
  render();
  frame++;
  delay(200);
}