#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H
#include <sys/_stdint.h>

class AdcBuffer
{
public:
    AdcBuffer(const uint16_t size);
    ~AdcBuffer();

    void setSize(const uint16_t size);
    uint16_t getSize() const;
    uint16_t getCount() const;
    bool isFull() const;
    void push(const uint16_t value);
    void reset();
    void getBytes(uint8_t* buffer);

private:
    struct AdcData
    {
        AdcData(uint16_t value, AdcData* nextPtr = nullptr);

        ~AdcData();

        uint16_t value;
        AdcData* nextPtr;
    };

private:
    uint16_t size;
    uint16_t count;

    AdcData* firstPtr;
    AdcData* lastPtr;
};

#endif // ADC_BUFFER_H
