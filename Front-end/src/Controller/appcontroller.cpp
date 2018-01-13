#include "appcontroller.h"

AppController::AppController() : mainView(nullptr)
{
    init();
}

void AppController::init()
{
    mainView = new MainView();
}

