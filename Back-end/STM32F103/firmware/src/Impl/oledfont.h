#ifndef OLEDFONT_H
#define OLEDFONT_H
#include <sys/_stdint.h>

class OledFont
{
public:
    static uint64_t getData(const char symbol);

private:
    static const uint64_t data[];
};

#endif // OLEDFONT_H
