#include "LedBlinker.h"

LedBlinker::LedBlinker(Thread &thr, uint32_t pin, uint32_t delay)
    : Actor(thr), blinkTimer(thr, delay, true)
{
  _pin = pin;

  blinkTimer >> ([&](const TimerMsg tm)
                 {
                   if (_mode == MANUAL)
                   {
                     blinkTimer.stop();
                   }
                   else if (_mode == BLINK)
                   {
                     if (_on)
                       on();
                     else
                       off();
                     _on = _on ? 0 : 1;
                   }
                   else if (_mode == PULSE)
                   {
                     off();
                     blinkTimer.stop();
                   } });

  blinkSlow >> [&](bool flag)
  {
    if (flag)
      blinkTimer.interval(BLINK_SLOW_INTERVAL);
    else
      blinkTimer.interval(BLINK_FAST_INTERVAL);
  };
}

void LedBlinker::on()
{
  gpio_set_level((gpio_num_t)_pin, 1);
  _on = 1;
}

void LedBlinker::off()
{
  gpio_set_level((gpio_num_t)_pin, 0);
  _on = 0;
}
void LedBlinker::init()
{
  gpio_config_t io_conf;
  io_conf.intr_type = (gpio_int_type_t)GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = 1 << _pin;
  io_conf.pull_down_en = (gpio_pulldown_t)1;
  io_conf.pull_up_en = (gpio_pullup_t)1;
  gpio_config(&io_conf);
  _mode = BLINK;
}

void LedBlinker::interval(uint32_t d) { blinkTimer.interval(d); }

void LedBlinker::mode(Mode m)
{
  _mode = m;
  if (m == BLINK)
  {
    blinkTimer.repeat(true);
    blinkTimer.start();
  }
  else if (m == MANUAL)
  {
    blinkTimer.stop();
  }
}

void LedBlinker::pulse()
{
  blinkTimer.start();
  on();
}
