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
#include "Buttonhandler.h"

/*
	PIN defines
*/
const byte LEDs = 60;
const byte PIXEL_PIN = 10, INTERRUPT_PIN = 2;

/*
	Global variables
*/
bool game_ended;

/*
  Buttons
*/
Buttonhandler buttons;

/*
  The LCD
*/
LiquidCrystal_I2C lcd(0x27, 20, 4);

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
const byte amount_of_games = 4;
String games[amount_of_games] = {"       Memory",
                                 "      Reaction",
                                 "      Pingpong",
                                 "   Throw the bomb"};
String stars = "     **********     ";

void repaint_main(short row)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(stars);
  lcd.setCursor(0, 1);
  lcd.print("     Main menu");
  lcd.setCursor(0, 2);
  lcd.print(main[row]);
  lcd.setCursor(0, 3);
  lcd.print(stars);
}

void repaint_games(short row)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(stars);
  lcd.setCursor(0, 1);
  lcd.print("     Games menu");
  lcd.setCursor(0, 2);
  lcd.print(games[row]);
  lcd.setCursor(0, 3);
  lcd.print(stars);
}

void repaint_brightness()
{
  lcd.clear();
  lcd.setCursor(0, 0);
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

    switch (buttons.get_last_pressed_menu_button(menu_buttons))
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

    switch (buttons.get_last_pressed_menu_button(menu_buttons))
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
    buttons.read_held_menu_buttons(menu_buttons, 4);
    for (byte i = 0; i < 4; ++i)
    {
      if (menu_buttons[i])
      {
        buttons.clear_buttons(menu_buttons, 4);
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
  buttons.clear_buttons(button_states, 6);
  while (true)
  {
    start_animation_reaction();
    buttons.read_buttons(button_states, 6);
    react = false;
    cheat = false;
    game_ended = false;
    unsigned long del = millis() + random(10000);
    while (millis() < del && !cheat)
    {
      buttons.read_buttons(button_states, 6);
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
      buttons.clear_buttons(button_states, 6);
    }
    if (!cheat)
    {
      set_all_to_color(medium_white_c);
      while (!react)
      {
        buttons.read_buttons(button_states, 6);
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
      buttons.clear_buttons(button_states, 6);
      bool again = false;
      while (!again)
      {
        buttons.read_buttons(button_states, 6);
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
      buttons.clear_buttons(button_states, 6);
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
int showing_time = 2000;

void setup()
{
  set_all_off();
}

void repaint_memory(int round)
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("       Memory");
  lcd.setCursor(0, 2);
  lcd.print("      Level " + String(round));
}

void game_memory()
{
  setup();
  int round = 5;
  for (; round < LEDs - 1; round += 1)
  {
    int color_order[round];
    repaint_memory(round);
    game_ended = false;
    set_all_off();
    for (byte i = 0; i < round; ++i)
    {
      byte rand = random(4);
      set_one_to(i, rand);
      color_order[i] = rand;
      strip.show();
    }
    delay(showing_time + round * 1000);
    set_all_off();
    short last_button_pressed = -1;
    for (byte i = 0; i < round; ++i)
    {
      bool button_array[6] = {0, 0, 0, 0, 0, 0};
      bool pressed = false;
      while (!pressed)
      {
        buttons.read_buttons(button_array, 6);
        for (byte j = 0; j < 6; ++j)
        {
          if (button_array[j])
          {
            Serial.println(j);
            pressed = true;
            break;
          }
        }
      }
      for (byte j = 0; true; j = (j + 1) % 6)
      {
        if (button_array[j])
        {
          last_button_pressed = j;
          break;
        }
      }
      if (color_order[i] == last_button_pressed)
      {
        set_one_to(i, last_button_pressed);
        buttons.clear_buttons(button_array, 6);
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
void display_score();
void end_game();
void game_won();

/*
  Local variables
*/
byte redlosses = 0;
byte greenlosses = 0;
byte bluelosses = 0;
byte yellowlosses = 0;

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
  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print("Pingpong");
}

void play()
{
  display_score();
  byte player_start = random(4);
  char pos = player_start * 15;
  long delay_ball = 300;
  int dir = 1;
  bool pointlost = false;
  int loseonpos = pos + 15;
  bool button_array[] = {0, 0, 0, 0, 0, 0};
  loseonpos = loseonpos > 59 ? loseonpos % 60 : loseonpos;
  loseonpos = loseonpos < 0 ? loseonpos + 60 : loseonpos;
  long stamp;
  int counter = 0;
  uint32_t oldcolor;
  while (!game_ended)
  {
    set_button_color_zones(15);
    display_score();
    while (!pointlost)
    {
      oldcolor = strip.getPixelColor(pos);
      set_one_to(pos, WHITE);
      stamp = millis();
      while (stamp + delay_ball > millis())
      {
        buttons.read_buttons(button_array, 6);
        if (button_array[pos / 15])
        {
          buttons.clear_buttons(button_array, 4);
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
        if (button_array[BLACK])
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
        }
        switch (pos == 0 ? YELLOW : dir < 0 ? (pos + 7) / 15 : (pos - 7) / 15)
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
        default:
          redlosses += 1;
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

void display_score()
{
  lcd.setCursor(0, 0);
  lcd.print(bluelosses);
  greenlosses < 10 ? lcd.setCursor(19, 0) : lcd.setCursor(18, 0);
  lcd.print(greenlosses);
  lcd.setCursor(0, 3);
  lcd.print(yellowlosses);
  redlosses < 10 ? lcd.setCursor(19, 3) : lcd.setCursor(18, 3);
  lcd.print(redlosses);
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
  ********************************
  Throw the bomb
  ********************************
*/
namespace throw_the_bomb
{
/*
  Local functions
*/
void setup();
void write_message(String, int);
void play();

/*
  Local variables
*/
int bomb_count;
String player_color = "";

void game_throw_the_bomb()
{
  setup();
  play();
}

void setup()
{
  //Reset
  set_all_off();
  lcd.clear();
  game_ended = false;

  Serial.println("Throw the bomb!");
  bomb_count = random(11, 20);

  //Light up edges of player areas
  set_one_to_color(0, red_c);
  set_one_to_color(14, red_c);
  set_one_to_color(15, green_c);
  set_one_to_color(29, green_c);
  set_one_to_color(30, blue_c);
  set_one_to_color(44, blue_c);
  set_one_to_color(45, yellow_c);
  set_one_to_color(59, yellow_c);

  //Turn on center lights in corresponding color
  for (int i = 0; i < 4; ++i)
  {
    set_one_to(7 + i * 15, i);
  }
  delay(2000);
}

void play()
{
  byte current_player = random(4);
  //write_message("Your turn ", current_player);

  //Start light at next player's first LED
  short pos = current_player * 15;
  short dir = 1;
  unsigned short del = 10;
  unsigned long stamp = 0;
  bool pressed = false;
  bool button_array[] = {0, 0, 0, 0, 0, 0};
  uint32_t old_color = strip.getPixelColor(pos);

  Serial.print("Bomb count: ");
  Serial.print(bomb_count); //to remove
  while (!game_ended)
  {
    old_color = strip.getPixelColor(pos);
    set_one_to_color(pos, medium_white_c);

    stamp = millis();
    /*
    if (bomb_count > 5)
      del = 10;
    else
      del = bomb_count * 2;
    */
    del = bomb_count + 2;
    while (stamp + del > millis())
    {
      buttons.read_buttons(button_array, 6);
      if (button_array[BLACK])
        return;
      if (button_array[current_player])
      {
        pressed = true;
        buttons.clear_buttons(button_array, 6);
        break;
      }
    }

    if (pressed)
    {
      pressed = false;
      //If pos is in upper half of range, remove 2 from count otherwise remove 1
      bomb_count -= (pos % 15 == 7) ? bomb_count : ((pos % 15) > 8) ? 2 : 1;
      if (pos % 15 == 7)
        bomb_count = 0;
      if (bomb_count <= 0)
      {
        game_ended = true;
        lcd.clear();
        lcd.setCursor(7, 1);
        lcd.print("BOOM!");
        blink_all(3, 1000, current_player);
      }
      else
      {
        lcd.clear();
        lcd.setCursor(9, 2);
        lcd.print(bomb_count);

        Serial.print("Bomb count: ");
        Serial.println(bomb_count);

        //Change current player to next one to press their button and move LED pos to next player's first LED
        current_player = ++current_player % 4;

        set_one_to_color(pos, old_color);
        pos = current_player * 15;
        old_color = strip.getPixelColor(pos);
      }
    }

    //Position should move forwards and backwards in next player's area
    //move position and reset LED
    if (pos % 15 == 0)
    {
      dir = 1;
    }
    else if (pos % 15 == 14)
    {
      dir = -1;
    }

    set_one_to_color(pos, old_color);
    pos += dir;
  }
}
} // namespace throw_the_bomb

namespace chaser
{
/*
  Local functions
*/
void setup();
void write_message(String, int);
void play();

/*
  Local variables
*/

bool button_states[6];
void game_chaser()
{
  setup();
  play();
}

void setup()
{
  //Reset
  set_all_off();
  lcd.clear();

  Serial.println("Chaser!");
  lcd.setCursor(0, 1);
  lcd.print("        Chaser");
}

void play()
{
  bool game_running = false;
  while (game_running)
  {
    buttons.read_buttons(button_states, 6);
    for (byte i = 0; i < 4; i++)
    {
      if (button_states[i])
      {
        game_running = true;
        //Move light
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Game over!");
} // chaser

} // namespace chaser
/*
	*********************************
	The main code
	*********************************
*/

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
  random(10);
  switch (menu::main_menu())
  {
  case 0:
    memory::game_memory();
    break;
  case 1:
    reaction_time::game_reaction_time();
    break;
  case 2:
    pingpong::game_pingpong();
    break;
  case 3:
    throw_the_bomb::game_throw_the_bomb();
    break;
  case 4:
    chaser::game_chaser();
    break;
  }
}
