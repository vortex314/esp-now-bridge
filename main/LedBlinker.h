#ifndef LEDBLINKER_H
#define LEDBLINKER_H

#include <limero.h>

#include "driver/gpio.h"

#define BLINK_SLOW_INTERVAL 500
#define BLINK_FAST_INTERVAL 50

class LedBlinker : public Actor {
  uint32_t _pin;
  int _on = 0;

 public:
  ValueFlow<bool> blinkSlow;

  typedef enum { MANUAL,BLINK,PULSE } Mode;
  Mode _mode = MANUAL;

  static const int BLINK_TIMER_ID = 1;
  TimerSource blinkTimer;
  LedBlinker(Thread& thr, uint32_t pin, uint32_t delay);
  void init();
  void interval(uint32_t d);
  void onNext(const TimerMsg&);
  void pulse();
  void on();
  void off();
  void mode(Mode m);
};

#endif  // LEDBLINKER_H
