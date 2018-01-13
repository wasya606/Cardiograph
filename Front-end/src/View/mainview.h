#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include "oscillview.h"

class MainView : public QQmlApplicationEngine
{
    Q_OBJECT
public:
    MainView();

private slots:
    void onObjectCreated(QObject* object, const QUrl& url);

private:
    void init();

private:
    QUrl mainWindowQmlUrl;
    QQuickWindow* mainWindow;
    OscillView* oscillView;

};

#endif // MAINVIEW_H
