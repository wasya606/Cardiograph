#ifndef TIMER_H
#define TIMER_H
#include "internal_types.h"

class Timer
{
public:
    Timer(const uint16_t delay, const Callback&& callback = [](){}, const bool isOneShot = true);

    void setDelay(const uint16_t delay);
    void setCallback(const Callback&& callback);
    void setOneShot(const bool isOneshot);
    uint16_t getDelay() const;
    bool getOneShot() const;

    void start(const bool isPerformreset = true);
    void stop();
    void reset();

    void process();

private:
    uint16_t delay;
    Callback callback;
    bool isOneShot;
    bool isActive;
    uint16_t counter;
};

#endif // TIMER_H
