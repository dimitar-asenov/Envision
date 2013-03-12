#include "HelperSingleton.h"

HelperSingleton& HelperSingleton::getInstance()
{
    static HelperSingleton instance;

    return instance;
}


