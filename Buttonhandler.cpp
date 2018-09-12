#include "Arduino.h"
#include "Buttonhandler.h"

/*
******************************************
  Reading buttons
******************************************
*/

void Buttonhandler::read_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    if (button_objects[i].wasPressed())
    {
      buttons[i] = true;
    }
  }
}

void Buttonhandler::read_menu_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    if (menu_button_objects[i].wasPressed())
    {
      buttons[i] = true;
    }
  }
}

void Buttonhandler::read_held_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    if (button_objects[i].isPressed())
    {
      buttons[i] = true;
    }
  }
}

byte Buttonhandler::get_last_pressed_menu_button(bool *menu_buttons)
{
  Buttonhandler::read_menu_buttons(menu_buttons, 4);
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

void Buttonhandler::read_held_menu_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    if (menu_button_objects[i].isPressed())
    {
      buttons[i] = true;
    }
  }
}

void Buttonhandler::clear_buttons(volatile bool *buttons, byte amount)
{
  for (byte i = 0; i < amount; ++i)
  {
    buttons[i] = false;
  }
}