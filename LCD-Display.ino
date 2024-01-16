#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 2
#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8
#define GAP_SIZE 1

#define GAP_PIXELS GAP_SIZE*(DISPLAY_WIDTH-1) + GAP_SIZE*(DISPLAY_HEIGHT-1)
#define BUFFER_SIZE ((DISPLAY_WIDTH * DISPLAY_HEIGHT * CHAR_WIDTH * CHAR_HEIGHT + GAP_PIXELS)/ 8 + 1)

byte customChar[CHAR_HEIGHT];

byte screen[BUFFER_SIZE];

void draw_char(int x, int y) 
{

}

void setup() {

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.home();

  screen[0] = B0;

  char chars[17];
  chars[16] = '\0';
  for (int i = 0; i < 16; i++)
    chars[i] = 1;
  lcd.write(chars);
}

void loop() {}