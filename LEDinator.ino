// Visual Micro is in vMicro>General>Tutorial Mode
//
/*
    Name:       AIO_LEDinator.ino
    Created:	2018-07-08 10:56:50
    Author:     Felix Lundblad
*/
#include "Adafruit_NeoPixel.h"
#include "GFButton.h"
#include "LiquidCrystal_I2C.h"

/*
	PIN defines
*/
const byte BUTTONS[6] = {4, 5, 6, 7, 8, 9};
const byte MENU_BUTTONS[4] = {A0, A1, A2, A3};
const byte LEDs = 60;
const byte PIXEL_PIN = 10, INTERRUPT_PIN = 2;

/*
	Global variables
*/
bool game_ended;

/*
  The LCD
*/
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Create two button instances on pins 2 & 3
GFButton buttonRed(4);
GFButton buttonGreen(5);
GFButton buttonBlue(6);
GFButton buttonYellow(7);
GFButton buttonWhite(8);
GFButton buttonBlack(9);
GFButton buttonUp(A0);
GFButton buttonLeft(A1);
GFButton buttonDown(A2);
GFButton buttonRight(A3);

GFButton button_objects[6] = {buttonRed, buttonGreen, buttonBlue, buttonYellow, buttonWhite, buttonBlack};
GFButton menu_button_objects[4] = {buttonUp, buttonLeft, buttonDown, buttonRight};

/*
	Setting the strip
*/
static const byte RED = 0, GREEN = 1, BLUE = 2, YELLOW = 3, WHITE = 4, BLACK = 5;
static const byte UP = 0, LEFT = 1, DOWN = 2, RIGHT = 3;
static const byte OFF = 0, LOW_INTENSITY = 7, MEDIUM_INTENSITY = 25, HIGH_INTENSITY = 255;

Adafruit_NeoPixel strip;

volatile signed short color_intensity;
volatile uint32_t red_c;
volatile uint32_t blue_c;
volatile uint32_t green_c;
volatile uint32_t yellow_c;
volatile uint32_t low_white_c;
volatile uint32_t medium_white_c;
volatile uint32_t high_white_c;
volatile uint32_t off_c;
/*
******************************************
  Reading buttons
******************************************
*/

void read_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    if (button_objects[i].wasPressed())
    {
      buttons[i] = true;
    }
  }
}

void read_menu_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    if (menu_button_objects[i].wasPressed())
    {
      buttons[i] = true;
    }
  }
}

void read_held_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    if (button_objects[i].isPressed())
    {
      buttons[i] = true;
    }
  }
}

byte get_last_pressed_menu_button(bool *menu_buttons)
{
  read_menu_buttons(menu_buttons, 4);
  for (short i = 0; i < 4; ++i)
  {
    if (menu_buttons[i])
    {
      clear_buttons(menu_buttons, 4);
      return i;
    }
  }
  return -1;
}

void read_held_menu_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    if (menu_button_objects[i].isPressed())
    {
      buttons[i] = true;
    }
  }
}

void clear_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    buttons[i] = false;
  }
}

/*
******************************************
  Setting the strip
******************************************
*/

void set_all_to_color(uint32_t color)
{
  for (byte i = 0; i < LEDs; ++i)
  {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void set_all_to(const byte &color)
{
  switch (color)
  {
  case RED:
    set_all_to_color(red_c);
    break;
  case GREEN:
    set_all_to_color(green_c);
    break;
  case BLUE:
    set_all_to_color(blue_c);
    break;
  case YELLOW:
    set_all_to_color(yellow_c);
    break;
  default:
    break;
  }
  strip.show();
}

void set_all_off()
{
  set_all_to_color(off_c);
}

void set_one_to_color(byte i, uint32_t color)
{
  strip.setPixelColor(i, color);
  strip.show();
}

void set_one_to(byte i, const byte &color)
{
  switch (color)
  {
  case RED:
    set_one_to_color(i, red_c);
    break;
  case GREEN:
    set_one_to_color(i, green_c);
    break;
  case BLUE:
    set_one_to_color(i, blue_c);
    break;
  case YELLOW:
    set_one_to_color(i, yellow_c);
    break;
  case BLACK:
    set_one_to_color(i, off_c);
    break;
  case WHITE:
    set_one_to_color(i, medium_white_c);
    break;
  default:
    break;
  }
}

void blink_all_color(int times, int delay_time, uint32_t color)
{
  for (byte i = 0; i < times; ++i)
  {
    set_all_to(color);
    delay(delay_time / 2);
    set_all_off();
    delay(delay_time / 2);
  }
}

void blink_all(int times, int delay_time, const byte &color)
{
  for (byte i = 0; i < times; ++i)
  {
    switch (color)
    {
    case RED:
      set_all_to_color(red_c);
      break;
    case GREEN:
      set_all_to_color(green_c);
      break;
    case BLUE:
      set_all_to_color(blue_c);
      break;
    case YELLOW:
      set_all_to_color(yellow_c);
      break;
    default:
      break;
    }
    delay(delay_time / 2);
    set_all_off();
    delay(delay_time / 2);
  }
}

void set_button_color_zones(int delay_time)
{
  for (byte i = 0; i < LEDs; ++i)
  {
    set_one_to(i, i / 15);
    delay(delay_time);
  }
}

/*
	*********************************
  Menu handling
  *********************************
*/
namespace menu
{
/*
  Local functions
*/
void repaint_main(short);
void repaint_games(short);
void repaint_brightness();
short main_menu();
short games_menu();
void brightness_menu();

/*
  Local variables
*/
bool menu_buttons[4] = {0, 0, 0, 0};
short row = 0;
short column = 0;
const byte amount_of_main = 2;
String main[amount_of_main] = {"       Games",
                               "     Brightness"};
const byte amount_of_games = 3;
String games[amount_of_games] = {"       Memory",
                                 "      Reaction",
                                 "      Pingpong"};

void repaint_main(short row)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     **********     ");
  lcd.setCursor(0, 1);
  lcd.print("     Main menu");
  lcd.setCursor(0, 2);
  lcd.print(main[row]);
  lcd.setCursor(0, 3);
  lcd.print("     **********     ");
}

void repaint_games(short row)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     **********     ");
  lcd.setCursor(0, 1);
  lcd.print("     Games menu");
  lcd.setCursor(0, 2);
  lcd.print(games[row]);
  lcd.setCursor(0, 3);
  lcd.print("     **********     ");
}

void repaint_brightness(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Brightness settings");
  lcd.setCursor(0, 1);
  lcd.print("Up to inc");
  lcd.setCursor(0, 2);
  lcd.print("Down to dec");
  lcd.setCursor(0, 3);
  lcd.print("Left to go back");
}

short main_menu()
{
  repaint_main(0);
  row = 0;
  while (true)
  {
    set_all_off();

    switch (get_last_pressed_menu_button(menu_buttons))
    {
    case UP:
      if (row > 0)
        row -= 1;
      repaint_main(row);
      break;
    case DOWN:
      if (row < amount_of_main - 1)
        row += 1;
      repaint_main(row);
      break;
    case LEFT:
      break;
    case RIGHT:
      switch (row)
      {
      case 0:
        return games_menu();
        break;
      case 1:
        brightness_menu();
        repaint_main(row);
        break;
      }
      break;
    }
  }
}

short games_menu()
{
  row = 0;
  repaint_games(0);
  while (true)
  {
    set_all_off();

    switch (get_last_pressed_menu_button(menu_buttons))
    {
    case UP:
      if (row > 0)
        row -= 1;
      repaint_games(row);
      break;
    case DOWN:
      if (row < amount_of_games - 1)
        row += 1;
      repaint_games(row);
      break;
    case LEFT:
      return -1;
      break;
    case RIGHT:
      return row;
      break;
    }
  }
}

void brightness_menu()
{
  repaint_brightness();
  while (true)
  {
    read_held_menu_buttons(menu_buttons, 4);
    for (byte i = 0; i < 4; ++i)
    {
      if (menu_buttons[i])
      {
        clear_buttons(menu_buttons, 4);
        switch (i)
        {
        case UP:
          color_intensity += 2;
          if (color_intensity > 255)
          {
            color_intensity = 255;
          }
          red_c = strip.Color(color_intensity, 0, 0);
          blue_c = strip.Color(0, color_intensity, 0);
          green_c = strip.Color(0, 0, color_intensity);
          yellow_c = strip.Color(color_intensity / 2, 0, color_intensity / 2);
          set_button_color_zones(0);
          break;
        case DOWN:
          color_intensity -= 2;
          if (color_intensity < 1)
          {
            color_intensity = 2;
          }
          red_c = strip.Color(color_intensity, 0, 0);
          blue_c = strip.Color(0, color_intensity, 0);
          green_c = strip.Color(0, 0, color_intensity);
          yellow_c = strip.Color(color_intensity / 2, 0, color_intensity / 2);
          set_button_color_zones(0);
          break;
        case LEFT:
          return;
          break;
        case RIGHT:
          break;
        }
      }
    }
  }
}
} // namespace menu

/*
	********************************
	Reaction game
	********************************
*/
namespace reaction_time
{
/*
	Local functions
*/
void reaction_triggered();
void start_animation_reaction();

/*
	Local variables
*/
bool react;
bool cheat;
bool button_states[6];

void game_reaction_time()
{
  clear_buttons(button_states, 6);
  while (true)
  {
    start_animation_reaction();
    read_buttons(button_states, 6);
    react = false;
    cheat = false;
    game_ended = false;
    unsigned long del = millis() + random(10000);
    while (millis() < del && !cheat)
    {
      read_buttons(button_states, 6);
      if (button_states[BLACK])
      {
        return;
      }
      for (byte i = 0; i < 4; ++i)
      {
        if (button_states[i])
        {
          cheat = true;
          blink_all(10, 100, i);
        }
      }
      clear_buttons(button_states, 6);
    }
    if (!cheat)
    {
      set_all_to_color(medium_white_c);
      while (!react)
      {
        read_buttons(button_states, 6);
        if (button_states[BLACK])
          return;
        for (byte i = 0; i < 6; ++i)
        {
          if (button_states[i])
          {
            react = true;
            set_all_to(i);
          }
        }
      }
      delay(1000);
      clear_buttons(button_states, 6);
      bool again = false;
      while (!again)
      {
        read_buttons(button_states, 6);
        if (button_states[BLACK])
          return;
        for (byte i = 0; i < 6; ++i)
        {
          if (button_states[i] != 0)
          {
            again = true;
          }
        }
      }
      clear_buttons(button_states, 6);
    }
  }
}

void start_animation_reaction()
{
  for (byte i = 0; i < LEDs; ++i)
  {
    set_one_to_color(i, low_white_c);
    delay(10);
  }

  for (char i = LEDs; i >= 0; --i)
  {
    set_one_to_color(i, off_c);
    delay(10);
  }
}
} // namespace reaction_time

/*
	*********************************
	Memory game
	*********************************
*/
namespace memory
{
/*
  Local functions
*/
void setup();
void quit();
void play();
void end_game(byte);
void game_won();

/*
  Local variables
*/
int showing_time = 5000;

void game_memory()
{
  set_all_off();
  byte round = 3;
  byte color_order[round];
  for (; round < LEDs - 1; round += 3)
  {
    game_ended = false;
    for (byte i = 0; i < round; ++i)
    {
      byte rand = random(3);
      set_one_to(i, rand);
      color_order[i] = rand;
      strip.show();
    }
    delay(showing_time + round * 00);
    set_all_off();
    byte last_button_pressed = -1;
    for (byte i = 0; i < round; ++i)
    {
      bool buttons[6] = {0, 0, 0, 0, 0, 0};
      bool pressed = false;
      while (!pressed)
      {
        read_buttons(buttons, 6);
        for (byte j = 0; j < 6; ++j)
        {
          if (buttons[j])
          {
            pressed = true;
            break;
          }
        }
      }
      for (byte j = 0; true; j = (j + 1) % 6)
      {
        if (buttons[j])
        {
          last_button_pressed = j;
          break;
        }
      }
      if (color_order[i] == last_button_pressed)
      {
        set_one_to(i, last_button_pressed);
        clear_buttons(buttons, 6);
      }
      else
      {
        end_game(round);
        return;
      }
    }
    blink_all(3, 200, GREEN);
  }
  game_won();
}

void end_game(byte round)
{
  blink_all(10, 50, RED);
  for (byte i = 0; i < round; ++i)
  {
    if ((i + 1) % 5)
      set_one_to(i, RED);
    else
      set_one_to(i, GREEN);
  }
  delay(6000);
}

void game_won()
{
  set_all_off();
  for (byte i = 0; i < LEDs; ++i)
  {
    set_one_to(i, GREEN);
    delay(500 / 60);
    set_all_off();
    delay(500 / 60);
    set_one_to(i, GREEN);
    delay(500 / 60);
  }
}

void quit()
{
  game_ended = true;
}
} // namespace memory

/*
	*********************************
	Pingpong
	*********************************
*/
namespace pingpong
{
/*
  Local functions
*/
void setup();
void quit();
void play();
void end_game();
void game_won();

/*
  Local variables
*/

void game_pingpong()
{
  setup();
  play();
}

void setup()
{
  game_ended = false;
  for (byte i = 0; i < LEDs; ++i)
  {
    set_one_to(i, RED);
    delay(5);
  }
  blink_all(10, 50, RED);
  set_button_color_zones(15);
}

void play()
{
  byte player_start = random(4);
  char pos = player_start * 15;
  byte redlosses = 0;
  byte greenlosses = 0;
  byte bluelosses = 0;
  byte yellowlosses = 0;
  long delay_ball = 300;
  int dir = 1;
  bool pointlost = false;
  int loseonpos = pos + 15;
  loseonpos = loseonpos > 59 ? loseonpos % 60 : loseonpos;
  loseonpos = loseonpos < 0 ? loseonpos + 60 : loseonpos;
  long stamp;
  int counter = 0;
  uint32_t oldcolor;
  while (!game_ended)
  {
    set_button_color_zones(15);
    while (!pointlost)
    {
      oldcolor = strip.getPixelColor(pos);
      set_one_to(pos, WHITE);
      stamp = millis();
      while (stamp + delay_ball > millis())
      {
        if (button_objects[pos / 15].wasPressed())
        {
          if (pos % 15 != 7)
            dir = pos - ((pos / 15) * 15 - 1 + 8) > 0 ? 1 : -1;
          else
            dir = dir > 0 ? -1 : 1;
          delay_ball = abs(pos - ((pos / 15) * 15 - 1 + 8)) * (30 - counter / 8);
          delay_ball = delay_ball < 10 ? 10 : delay_ball;
          loseonpos = dir > 0 ? ((pos / 15 + 2) * 15) : ((pos / 15 - 1) * 15 - 1);
          loseonpos = loseonpos > 59 ? loseonpos % 60 : loseonpos;
          loseonpos = loseonpos < 0 ? loseonpos + 60 : loseonpos;
        }
        if (button_objects[BLACK].wasPressed())
        {
          return;
        }
      }
      set_one_to_color(pos, oldcolor);
      pos += dir;
      pos = pos >= 0 ? pos : 59;
      pos = pos == 60 ? 0 : pos;

      ++counter;

      if (pos == loseonpos)
      {
        pointlost = true;
        if (!pos && dir)
        {
          for (byte k = 0; k < 10; ++k)
          {
            for (byte i = 0; i < 15; ++i)
            {
              set_one_to((45 + i), RED);
            }
            delay(40);
            for (byte i = 0; i < 15; ++i)
            {
              set_one_to((45 + i), BLACK);
            }
            delay(40);
          }
        }
        else
        {
          for (byte k = 0; k < 10; ++k)
          {
            for (byte i = 0; i < 15; ++i)
            {
              set_one_to((pos - i * dir - dir) % 60, RED);
            }
            delay(40);
            for (byte i = 0; i < 15; ++i)
            {
              set_one_to((pos - i * dir - dir) % 60, BLACK);
            }
            delay(40);
          }
          //blink_all(10, 100, RED);
        }
        switch (pos / 15)
        {
        case RED:
          redlosses += 1;
          break;
        case GREEN:
          greenlosses += 1;
          break;
        case BLUE:
          bluelosses += 1;
          break;
        case YELLOW:
          yellowlosses += 1;
          break;
        }
      }
    }
    pointlost = false;
    counter = 0;
    set_button_color_zones(5);
    delay_ball = 300;
    loseonpos = pos + 15;
    loseonpos = loseonpos > 59 ? loseonpos % 60 : loseonpos;
    loseonpos = loseonpos < 0 ? loseonpos + 60 : loseonpos;
  }
}

void end_game()
{
  blink_all(10, 50, RED);
}

void quit()
{
  game_ended = true;
}
} // namespace pingpong

/*
	*********************************
	The main code
	*********************************
*/

using namespace reaction_time;
using namespace memory;
using namespace pingpong;

void setup()
{
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);

  color_intensity = 10;
  red_c = strip.Color(color_intensity, 0, 0);
  blue_c = strip.Color(0, color_intensity, 0);
  green_c = strip.Color(0, 0, color_intensity);
  yellow_c = strip.Color(color_intensity / 2, 0, color_intensity / 2);
  low_white_c = strip.Color(LOW_INTENSITY, LOW_INTENSITY, LOW_INTENSITY);
  medium_white_c = strip.Color(MEDIUM_INTENSITY, MEDIUM_INTENSITY, MEDIUM_INTENSITY);
  high_white_c = strip.Color(HIGH_INTENSITY, HIGH_INTENSITY, HIGH_INTENSITY);
  off_c = strip.Color(0, 0, 0);

  strip = Adafruit_NeoPixel(LEDs, PIXEL_PIN, NEO_BRGW + NEO_KHZ800);
  game_ended = false;
  strip.begin();
  for (byte i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, low_white_c);
    delay(5);
    strip.show();
  }
  blink_all(1, 100, RED);
  blink_all(1, 100, GREEN);
  blink_all(1, 100, BLUE);
}

void loop()
{
  switch (menu::main_menu())
  {
  case 0:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Playing: Memory");
    memory::game_memory();
    break;
  case 1:
    reaction_time::game_reaction_time();
    break;
  case 2:
    pingpong::game_pingpong();
    break;
  }
}