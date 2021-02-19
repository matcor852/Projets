#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QScopedPointer>
#include "LPclient.h"

#include <QQuickStyle>

int main(int argc, char *argv[])
{

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("LP");
    app.setOrganizationDomain("LP");
    app.setApplicationDisplayName("LP");
    app.setApplicationName("LP");
    app.setApplicationVersion("1.0");

    QScopedPointer<LPclient> lpClient(new LPclient);


    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    QQuickStyle::setStyle("Material");

    engine.rootContext()->setContextProperty("lpClient",lpClient.data());
    engine.load(url);

    return app.exec();
}
