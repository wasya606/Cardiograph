#ifndef TIMER_H
#define TIMER_H
#include "internal_types.h"

class Timer
{
public:
    Timer(const uint32_t delay = 1000, const Callback&& callback = [](){}, const bool isOneShot = false);

    void setDelay(const uint32_t delay);
    void setCallback(const Callback&& callback);
    void setOneShot(const bool isOneshot);
    uint32_t getDelay() const;
    bool getOneShot() const;
    bool isStarted() const;

    void start(const bool isPerformreset = true);
    void stop();
    void reset();

    void process();

private:
    uint32_t delay;
    Callback callback;
    bool isOneShot;
    bool isActive;
    uint32_t counter;
};

#endif // TIMER_H
