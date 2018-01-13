#include "../Inc/timer.h"


Timer::Timer(const uint16_t delay, const Callback &&callback, const bool isOneShot) :
    delay(delay), callback(callback), isOneShot(isOneShot), isActive(false), counter(0)
{

}

void Timer::setDelay(const uint16_t delay)
{
    this->delay = delay;
}

void Timer::setCallback(const Callback &&callback)
{
    this->callback = callback;
}

void Timer::setOneShot(const bool isOneshot)
{
    this->isOneShot = isOneshot;
}

uint16_t Timer::getDelay() const
{
    return delay;
}

bool Timer::getOneShot() const
{
    return isOneShot;
}

void Timer::start(const bool isPerformreset)
{
    isActive = true;
    counter = isPerformreset ? 0 : counter;
}

void Timer::stop()
{
    isActive = false;
}

void Timer::reset()
{
    counter = 0;
}

void Timer::process()
{
    if (!isActive)
        return;

    if (counter == delay)
    {
        counter = 0;
        isActive = isOneShot ? false : isActive;
        callback();
    }
    else
    {
        counter++;
    }
}
