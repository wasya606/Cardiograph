#include "adc_buffer.h"
#include <stdio.h>

AdcBuffer::AdcBuffer(const uint16_t size) :
    size(size), count(0),
    firstPtr(nullptr), lastPtr(nullptr)
{

}

AdcBuffer::~AdcBuffer()
{
    reset();
}

void AdcBuffer::setSize(const uint16_t size)
{
    if (size != this->size)
    {
        this->size = size;
    }
}

uint16_t AdcBuffer::getSize() const
{
    return size;
}

uint16_t AdcBuffer::getCount() const
{
    return count;
}

bool AdcBuffer::isFull() const
{
    return count == size;
}

void AdcBuffer::push(const uint16_t value)
{
    AdcData* dataPtr = new AdcData(value);
    if (count == 0)
    {
        firstPtr = lastPtr = dataPtr;
        count++;
    }
    else
    {
        lastPtr->nextPtr = dataPtr;
        lastPtr = dataPtr;
        if (count == size)
        {
            AdcData* toDeletePtr = firstPtr;
            firstPtr = firstPtr->nextPtr;
            toDeletePtr->nextPtr = nullptr;
            delete toDeletePtr;
        }
        else
        {
            count++;
        }
    }

    //printf("Buffer count: %u\n", count);
/*    for (AdcData* it = firstPtr; it != nullptr; it = it->nextPtr)
        printf("%u ", it->value);

    printf("\n");
*/
}

void AdcBuffer::reset()
{
    if (firstPtr != nullptr)
        delete firstPtr;

    firstPtr = lastPtr = nullptr;
    count = 0;
}

void AdcBuffer::getBytes(uint8_t *buffer)
{
    uint16_t i = 0;
    for (AdcData* it = firstPtr; it != nullptr; it = it->nextPtr)
    {
        buffer[i++] = ((it->value & 0xFF00) >> 8) & 0xFF;
        buffer[i++] = it->value & 0xFF;
    }
}

AdcBuffer::AdcData::AdcData(uint16_t value, AdcBuffer::AdcData *nextPtr) :
    value(value),
    nextPtr(nextPtr)
{

}

AdcBuffer::AdcData::~AdcData()
{
    if (nextPtr != nullptr)
    {
        delete nextPtr;
    }
}
