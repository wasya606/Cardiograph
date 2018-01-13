#include "mainview.h"
#include <QDebug>
#include "constants.h"

MainView::MainView() :
    mainWindowQmlUrl(QUrl(QString(Constants::QML_PATH_PREFIX).append(Constants::QML_MAIN_WINDOW_FILE_NAME))),
    mainWindow(nullptr),
    oscillView(nullptr)
{
    connect(this, SIGNAL(objectCreated(QObject*, QUrl)), SLOT(onObjectCreated(QObject*,QUrl)));
    load(mainWindowQmlUrl);
}

void MainView::onObjectCreated(QObject *object, const QUrl &url)
{
    if (url == mainWindowQmlUrl)
    {
        mainWindow = dynamic_cast<QQuickWindow*>(object);
        init();
    }
}

void MainView::init()
{
    if (mainWindow == nullptr)
        return;

    QQuickItem* oscillViewItem = mainWindow->findChild<QQuickItem*>(Constants::OSCILL_VIEW_OBJECT_NAME);

    oscillView = new OscillView(oscillViewItem);
    oscillView->init();

}
