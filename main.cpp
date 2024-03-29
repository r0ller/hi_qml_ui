#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#ifdef __NATIVE__
//#include "receiver.h"
#endif
#if defined(__ANDROID__) || defined(__NODEJS__)
#include <emscripten.h>
#endif
#include "messageboard.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	//TODO:instantiate messageboard with rootobject as parent?
	messageboard msgBoard;
	engine.rootContext()->setContextProperty("msgBoard", &msgBoard);
	engine.load(url);
	msgBoard.setRootObject(engine.rootObjects().first());
	//Calling callbackMsg on native makes no sense, put here just for debugging
	//msgBoard.callbackMsg("{\"returnObj\":\"menu\",\"returnFunction\":\"setLanguage\",\"language\":\"English US\"}");
	#ifdef __ANDROID__
	EM_ASM({
		window.onmessage=function(e){
			window.msgPort=e.ports[0];
			//console.log('js:'+e.data);
			//pass port init message to callbackMsg
			dynCall('vi',$0,[allocate(intArrayFromString(e.data),'i8',ALLOC_NORMAL)]);
			window.msgPort.onmessage=function(f){
				//console.log('js:'+f.data);
				//let result=JSON.parse(f.data);
				//TODO: https://stackoverflow.com/questions/29319208/call-c-function-pointer-from-javascript
				dynCall('vi',$0,[allocate(intArrayFromString(f.data),'i8',ALLOC_NORMAL)]);
			}
		}
	},messageboard::fptr_callbackMsg);
	#endif
	return app.exec();
}
