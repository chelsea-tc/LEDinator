// Visual Micro is in vMicro>General>Tutorial Mode
//
/*
    Name:       AIO_LEDinator.ino
    Created:	2018-07-08 10:56:50
    Author:     Felix Lundblad
*/
#include "Adafruit_NeoPixel.h"
#include "GFButton.h"

/*
	PIN defines
*/
const byte BUTTONS[6] = {4, 5, 6, 7, 8, 9};
const byte MENU_BUTTONS[4] = {A0, A1, A2, A3};
const byte LEDs = 60;
const byte PIXEL_PIN = 10, INTERRUPT_PIN = 2;

/*
	General game variables
*/
bool game_ended;

// Create two button instances on pins 2 & 3
GFButton buttonRed(4);
GFButton buttonGreen(5);
GFButton buttonBlue(6);
GFButton buttonYellow(7);
GFButton buttonWhite(8);
GFButton buttonBlack(9);
//GFButton buttonUp(A0);
//GFButton buttonLeft(A1);
//GFButton buttonDown(A2);
//GFButton buttonRight(A3);

GFButton button_objects[6] = {buttonRed, buttonGreen, buttonBlue, buttonYellow, buttonWhite, buttonBlack};
//GFButton menu_button_objects[4] = {buttonUp, buttonLeft, buttonDown, buttonDown};

/*
	Setting the strip
*/
static const byte RED = 0, GREEN = 1, BLUE = 2, YELLOW = 3, BLACK = 4, WHITE = 5;
static const byte UP = 0, LEFT = 1, DOWN = 2, RIGHT = 3;
static const byte OFF = 0, LOW_INTENSITY = 7, MEDIUM_INTENSITY = 25, HIGH_INTENSITY = 255;

Adafruit_NeoPixel strip;

volatile byte color_intensity;
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
  if (!digitalRead(A0))
  {
    buttons[0] = true;
  }

  if (!digitalRead(A1))
  {
    buttons[1] = true;
  }

  if (!digitalRead(A2))
  {
    buttons[2] = true;
  }

  if (!digitalRead(A3))
  {
    buttons[3] = true;
  }
}

void clear_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    buttons[i] = false;
  }
}

void clear_menu_buttons(volatile bool *buttons, byte amount)
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
	********************************
	Spinning LED
	********************************
*/
namespace spinning_LED
{
/*
	Local function declaration
*/
void setup();
void opt_in();
void start_spinning();
/*
	Local variables
*/
static const byte player_area = 5;
byte offset = 5;
byte position = 0;
bool button_states[6];
bool playing[4];
bool hit[4];

void game_spinning_LED()
{
  setup();
  start_spinning();
}

void setup()
{
  set_all_to_color(yellow_c);
  read_buttons(button_states, 6);
  while (!button_states[BLACK] && !button_states[WHITE])
  {
    read_buttons(button_states, 6);
    if (button_states[BLACK])
    {
      set_all_off();
      return;
    }
  }
  opt_in();
}

void opt_in()
{
  for (byte i = 0; i < 6; ++i)
  {
    if (i < 4 && button_states[i])
    {
      byte start = offset + 15 * i - player_area / 2;
      byte end = offset + 15 * i + player_area - player_area / 2;
      for (byte i = start; i < end; ++i)
      {
        strip.setPixelColor(i, blue_c);
        delay(200);
        strip.show();
        //(i >= 0) ? strip.setPixelColor(i, blue_c) : strip.setPixelColor(i + 60, blue_c);
      }
      playing[i] = true;
    }
  }
}

void start_spinning()
{
  for (byte i = 0; i < 4; ++i)
  {
    if (playing[i])
      hit[i] = false;
    else
      hit[i] = true;
  }
  uint32_t old_color;
  byte player_counter = 0;
  while (!game_ended)
  {
    for (byte position = 0; position < LEDs && !game_ended; ++position)
    {
      old_color = strip.getPixelColor(position);
      set_one_to(position, red_c);

      // If the LED right after the player area is lit, check if player has pushed the button
      if (player_counter * 15 + offset + player_area == position)
      {
        if (playing[player_counter])
        {
          if (hit[player_counter])
            hit[player_counter] = false;
          else
          {
            set_all_to(player_counter);
            game_ended = true;
          }
        }
        player_counter = (player_counter + 1) % 4;
      }

      // Delay and speed up the spinning every X step
      delay(300 - position / 5);

      // Repaint the old pixel
      set_one_to_color(position, old_color);
    }
  }
}

} // namespace spinning_LED

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

void game_reaction_time()
{
  while (true)
  {
    bool button_states[6] = {0, 0, 0, 0, 0, 0};
    read_buttons(button_states, 6);
    if (button_states[BLACK])
      return;
    react = false;
    start_animation_reaction();
    game_ended = false;
    delay(random(7000) + 3000);
    for (byte i = 0; i < 6; ++i)
    {
      if (button_states[i])
      {
        blink_all(10, 100, i);
        react = true;
        break;
      }
    }
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
    delay(2000);
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
  }
}

void start_animation_reaction()
{
  for (byte i = 0; i < LEDs; ++i)
  {
    strip.setPixelColor(i, low_white_c);
    strip.show();
    delay(600 / 30);
  }

  for (byte i = 200; i > 0; i -= 10)
  {
    set_all_to_color(low_white_c);
    delay(i);
    set_all_off();
    delay(i);
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
  for (;round < LEDs - 1; round += 3 )
  {
    Serial.println("Setting up memory");
    game_ended = false;
    for (byte i = 0; i < round; ++i)
    {
      byte rand = random(3);
      set_one_to(i, rand);
      color_order[i] = rand;
      strip.show();
    }
    Serial.print("Delaying for ");
    Serial.print(showing_time / 1000);
    Serial.println(" seconds before turning off");
    delay(showing_time + round*00);
    set_all_off();
    Serial.println("Memory setup done");
    Serial.println("Playing memory");
    byte last_button_pressed = -1;
    for (byte i = 0; i < round; ++i)
    {
      Serial.print("LED number : ");
      Serial.println(i);
      bool buttons[6] = {0, 0, 0, 0, 0, 0};
      bool pressed = false;
      while (!pressed)
      {
        read_buttons(buttons, 6);
        for (byte j = 0; j < 6; ++j)
        {
          if (buttons[j])
          {
            Serial.println(j);
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
          Serial.print("Button number : ");
          Serial.print(j);
          Serial.println(" was pressed");
          break;
        }
      }
      /*if (game_ended) {
      end_game();
      detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN));
      return;
      }*/
      Serial.println("Continuing");
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
  for(byte i = 0; i < round; ++i){
    if((i+1)%5)
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
void get_ready(byte);

/*
  Local variables
*/

/*
	*********************************
	Pingpong
	*********************************
*/

void game_pingpong()
{
  setup();
  play();
}

void setup()
{
  Serial.println("Setting up pingpong");
  game_ended = false;
  for (byte i = 0; i < LEDs; ++i)
  {
    set_one_to(i, RED);
    delay(5);
  }
  blink_all(10, 50, RED);
  set_button_color_zones(15);
  Serial.println("Pingpong setup done");
}

void play()
{
  byte player_start = random(4);
  get_ready(player_start);
  byte pos = player_start * 15;
  byte redlosts = 0;
  byte greenlosts = 0;
  byte bluelosts = 0;
  byte yellowlosts = 0;
  long delay_ball = 300;
  int dir = 1;
  bool pointlost = false;
  byte loseonpos = pos + 15;
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
          dir = pos - ((pos / 15) * 15 - 1 + 8) > 0 ? 1 : -1;
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
      pos = pos > 0 ? pos : 59;
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

void get_ready(byte player_start)
{
  Serial.println("Get ready for pingpong!");
  delay(2000);
  String player_color;
  switch (player_start)
  {
  case 0:
    player_color = "red";
    break;
  case 1:
    player_color = "green";
    break;
  case 2:
    player_color = "blue";
    break;
  case 3:
    player_color = "yellow";
    break;
  default:
    player_color = "";
    break;
  }
  Serial.print("The ");
  Serial.print(player_color);
  Serial.print(" player serves");
}

void end_game()
{
  blink_all(10, 50, RED);
}

void game_won()
{
  set_all_off();
  for (byte i = 0; i < LEDs; ++i)
  {
    set_one_to(i, GREEN);
    delay(1000 / 60);
  }
  blink_all(3, 1000, GREEN);
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
using namespace spinning_LED;
using namespace memory;
using namespace pingpong;

// The setup() function runs once each time the micro-controller starts
void setup()
{
  Serial.begin(115200);

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
  blink_all(1, 400, RED);
  blink_all(1, 400, GREEN);
  blink_all(1, 400, BLUE);
}

/*
  void setup() {
  strip = Adafruit_NeoPixel(LEDs, PIXEL_PIN, NEO_GRBW + NEO_KHZ800);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  strip.setBrightness(50);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  set_all_to(1);
  }*/

// Add the main program code into the continuous loop() function
void loop()
{
  bool buttons[6] = {0, 0, 0, 0, 0, 0};
  bool menu_buttons[4] = {0, 0, 0, 0};
  blink_all(5, 200, GREEN);
  uint16_t snake_counter = 0;
  while (true)
  {
    random(10);
    if (snake_counter % 10 == 0)
    {
      for (byte i = 0; i < 10; ++i)
      {
        strip.setPixelColor((snake_counter / 10 + i) % LEDs, i * 1, i * 1, i * 1);
      }
    }
    ++snake_counter;
    strip.show();
    read_buttons(buttons, 6);
    for (byte i = 0; i < 6; ++i)
    {
      if (buttons[i])
      {
        clear_buttons(buttons, 6);
        switch (i)
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
          //spinning_LED::game_spinning_LED();
          break;
        }
        blink_all(5, 200, GREEN);
      }
      read_menu_buttons(menu_buttons, 4);
      for (byte i = 0; i < 4; ++i)
      {
        if (menu_buttons[i])
        {
          clear_menu_buttons(menu_buttons, 4);
          switch (i)
          {
          case UP:
            color_intensity += 3;
            if (color_intensity > 255)
            {
              color_intensity = 255;
            }
            red_c = strip.Color(color_intensity, 0, 0);
            blue_c = strip.Color(0, color_intensity, 0);
            green_c = strip.Color(0, 0, color_intensity);
            yellow_c = strip.Color(color_intensity / 2, 0, color_intensity / 2);
            Serial.println("UP");
            Serial.println(color_intensity);
            set_button_color_zones(0);
            break;
          case DOWN:
            color_intensity -= 3;
            if (color_intensity < 1)
            {
              color_intensity = 2;
            }
            red_c = strip.Color(color_intensity, 0, 0);
            blue_c = strip.Color(0, color_intensity, 0);
            green_c = strip.Color(0, 0, color_intensity);
            yellow_c = strip.Color(color_intensity / 2, 0, color_intensity / 2);
            Serial.println(color_intensity);
            set_button_color_zones(0);
            break;
          case LEFT:
            break;
          case RIGHT:
            break;
          }
        }
      }
    }
  }
}
