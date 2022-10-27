#ifndef MESSAGEBOARD_H
#define MESSAGEBOARD_H
#include <QObject>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonObject>
#include <QTranslator>
#if defined(__ANDROID__) || defined(__NODEJS__)
#include <emscripten.h>
#endif

class messageboard : public QObject
{
    Q_OBJECT
    //https://wiki.qt.io/How_to_do_dynamic_translation_in_QML
    Q_PROPERTY(QString triggersTr READ emptyString NOTIFY languageChanged)
public:
    explicit messageboard(QObject *parent = nullptr);
    //Q_INVOKABLE bool postMessage(const QString &msg);
    static void (*fptr_callbackMsg)(const char* msg);
    static void callbackMsg(const char* msg);
    static void setRootObject(QObject*);
    static QObject* getRootObject();
    QString emptyString(){return "";}

public slots:
    void refresh();
    bool sendMessage(const QJsonObject& msg);
    void selectLanguage(const QString& language);

private:
    static QObject* rootObject;
    static QJsonObject lastCmdObj;
    QTranslator translator_en;//Is as many translator necessary as many languages we'd like to handle?
    unsigned int clickCounter=1;

signals:
    void languageChanged();
};
#endif // MESSAGEBOARD_H
