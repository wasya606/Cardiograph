#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "mainview.h"

class AppController
{
public:
    AppController();

private:
    void init();

    MainView *mainView;
};

#endif // APPCONTROLLER_H
