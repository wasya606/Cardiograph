#ifndef INTERNAL_TYPES_H
#define INTERNAL_TYPES_H
#include <sys/_stdint.h>
#include <functional>

using CallbackPtr = void (*)();
using Callback = std::function<void(void)>;

#endif // INTERNAL_TYPES_H
