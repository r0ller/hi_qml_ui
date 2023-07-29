#include "messageboard.h"
#include <QJsonDocument>
#ifdef __NATIVE__
//#include "receiver.h"
#endif

#ifdef __NODEJS__
void onload(unsigned handle, void *cmdObj, void *data, unsigned size){
    std::string response((char*)data,size);
    QString qResponse=QString(response.c_str());
    qDebug()<<"response:"<<qResponse;
    QJsonObject* pCmdObj=static_cast<QJsonObject*>(cmdObj);
    QString returnObject=pCmdObj->value("returnObj").toString();
    qDebug()<<"returnObj:"<<returnObject;
    QString returnFunction=pCmdObj->value("returnFunction").toString();
    qDebug()<<"returnFunction:"<<returnFunction;
    delete pCmdObj;
    if(returnObject.isEmpty()==false&&returnFunction.isEmpty()==false){
        QObject* obj=messageboard::getRootObject()->findChild<QObject*>(returnObject);
        QMetaObject::invokeMethod(obj,returnFunction.toStdString().c_str(),Q_ARG(QString, qResponse));
    }
}
void onerror(unsigned handle, void *userData, int status, const char *message){
    qDebug()<<message;
}
void onprogress(unsigned handle, void *userData, int loaded, int total){
    qDebug()<<"onprogress";
}
#endif


void (*messageboard::fptr_callbackMsg)(const char* msg)=NULL;
QObject* messageboard::rootObject;
QJsonObject messageboard::lastCmdObj;

messageboard::messageboard(QObject *parent) : QObject(parent)
{
    messageboard::fptr_callbackMsg=&messageboard::callbackMsg;
}

//bool messageboard::postMessage(const QString &msg) {
bool messageboard::sendMessage(const QJsonObject& cmdObj) {
    lastCmdObj=cmdObj;
    #ifdef __NATIVE__
        //qDebug() << "Called the C++ method with" << msg;
        //QObject* obj=messageboard::rootObject->findChild<QObject*>("mouseArea");
        //qDebug()<<obj->objectName();
        //const char* response=execute(msg.toStdString().c_str());
        //QMetaObject::invokeMethod(obj,"msgIn",Q_ARG(QString, QString(response)));
        QString response="";
        if(clickCounter%2==1){
            response="{\"type\":\"text\",\"data\":{\"text\":\"hi!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\",\"speaker\":\"hi\"}}";
        }
        else{
            response="{\"type\":\"text\",\"data\":{\"text\":\"you!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\",\"speaker\":\"you\"}}";
        }
        ++clickCounter;
        QString returnObject=lastCmdObj.value("returnObj").toString();
        qDebug()<<"returnObj:"<<returnObject;
        QString returnFunction=lastCmdObj.value("returnFunction").toString();
        qDebug()<<"returnFunction:"<<returnFunction;
        if(returnObject.isEmpty()==false&&returnFunction.isEmpty()==false){
            QObject* obj=messageboard::rootObject->findChild<QObject*>(returnObject);
            QMetaObject::invokeMethod(obj,returnFunction.toStdString().c_str(),Q_ARG(QString, QString(response)));
        }
    #endif
    #ifdef __NODEJS__
        QJsonObject *cmdObjHeap=new QJsonObject(cmdObj);
        qDebug()<<"returnObj:"<<cmdObjHeap->value("returnObj").toString();
        qDebug()<<"returnFunction:"<<cmdObjHeap->value("returnFunction").toString();
        qDebug()<<"service:"<<cmdObjHeap->value("service").toString();
        qDebug()<<"method:"<<cmdObjHeap->value("method").toString();
        char *pUrl=(char*)EM_ASM_INT({
            return window.location.origin;
        });
        QString url=QString(pUrl);
        free(pUrl);
        if(url.endsWith("/")==false) url+="/";
        QString service=url+cmdObj.value("service").toString();
        QString method=cmdObj.value("method").toString();
        QString params;
        if(cmdObj.contains("params")==true){
            QJsonObject paramsObj=cmdObj.value("params").toObject();
            QStringList keys=paramsObj.keys();
            for(auto&& key:keys){
                QJsonValue value=paramsObj.value(key);
                if(value.isObject()==true){
                    params+=key+"="+QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact)+"&";
                }
                else{
                    params+=key+"="+value.toString()+"&";
                }
            }
            params.truncate(params.size()-1);
            if(method=="GET"){
                service+="?"+params;
            }
        }
        qDebug()<<"params:"<<params;
        emscripten_async_wget2_data(service.toStdString().c_str(),method.toStdString().c_str(),params.toStdString().c_str(),(void*)cmdObjHeap,true,&onload,&onerror,&onprogress);
    #endif
    #ifdef __ANDROID__
        QString msg=QJsonDocument(cmdObj).toJson(QJsonDocument::Compact);
        EM_ASM({
            //console.log("postMessage");
            window.msgPort.postMessage(UTF8ToString($0));
        },msg.toStdString().c_str());
    #endif
    return true;
}

void messageboard::refresh() {
    #if defined(__NATIVE__) || defined(__NODEJS__)
        qDebug() << "Called the C++ slot";
    #endif
    #ifdef __ANDROID__
        EM_ASM(
            console.log('Refresh');
        );
    #endif
}

void messageboard::callbackMsg(const char* msg){
    #ifdef __NATIVE__
    //Not necessary on native, just put here to debug what happens on Android
    qDebug()<<"callback msg:"<<QString(msg);
    QString returnObject;
    QString returnFunction;
    QJsonObject cmdObj=QJsonDocument::fromJson(QString(msg).toUtf8()).object();
    auto iCmdObj=cmdObj.find("returnObj");
    if(iCmdObj!=cmdObj.end()){
        returnObject=iCmdObj->toString();
        iCmdObj=cmdObj.find("returnFunction");
        if(iCmdObj!=cmdObj.end()){
            returnFunction=iCmdObj->toString();
        }
    }
    else{
        returnObject=lastCmdObj.value("returnObj").toString();
        returnFunction=lastCmdObj.value("returnFunction").toString();
    }
    qDebug()<<"returnObj:"<<returnObject;
    qDebug()<<"returnFunction:"<<returnFunction;
    if(returnObject.isEmpty()==false&&returnFunction.isEmpty()==false){
        QObject* obj=messageboard::rootObject->findChild<QObject*>(returnObject);
        QMetaObject::invokeMethod(obj,returnFunction.toStdString().c_str(),Q_ARG(QString, QString(msg)));
    }
    #endif
    #ifdef __ANDROID__
    qDebug()<<"callback msg:"<<QString(msg);
    QString returnObject;
    QString returnFunction;
    QJsonObject cmdObj=QJsonDocument::fromJson(QString(msg).toUtf8()).object();
    auto iCmdObj=cmdObj.find("returnObj");
    if(iCmdObj!=cmdObj.end()){
        returnObject=iCmdObj->toString();
        iCmdObj=cmdObj.find("returnFunction");
        if(iCmdObj!=cmdObj.end()){
            returnFunction=iCmdObj->toString();
        }
    }
    else{
        returnObject=lastCmdObj.value("returnObj").toString();
        returnFunction=lastCmdObj.value("returnFunction").toString();
    }
    qDebug()<<"returnObj:"<<returnObject;
    qDebug()<<"returnFunction:"<<returnFunction;
    if(returnObject.isEmpty()==false&&returnFunction.isEmpty()==false){
        QObject* obj=messageboard::rootObject->findChild<QObject*>(returnObject);
        QMetaObject::invokeMethod(obj,returnFunction.toStdString().c_str(),Q_ARG(QString, QString(msg)));
    }
//        EM_ASM({
//            console.log(UTF8ToString($0));
//        },obj->objectName().toStdString().c_str());//msg);
    #endif
}

void messageboard::setRootObject(QObject* rootObject){
    messageboard::rootObject=rootObject;
}

QObject* messageboard::getRootObject(){
    return messageboard::rootObject;
}

void messageboard::selectLanguage(const QString& language){
    qDebug()<<"language:"<<language;
    if(language=="en"){
        qDebug()<<"en";
        bool loaded=translator_en.load("procont_en");
        if(loaded==true){
            qDebug()<<"loaded";
            qApp->installTranslator(&translator_en);
        }
    }
    else{
        qDebug()<<"hu";
        qApp->removeTranslator(&translator_en);
    }
    emit languageChanged();
}
