/**
 * Chat Application Entry Point - Phase 2: Qt/QML GUI
 * 
 * Main Qt application that loads QML interface
 * Integrates C++ networking backend with QML frontend
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

// TODO: Include message model
// #include "models/messagemodel.h"

// TODO: Include network client
// #include "backend/client.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // TODO: Register C++ types with QML
    // qmlRegisterType<MessageModel>("Chat", 1, 0, "MessageModel");
    
    // TODO: Create and expose network client to QML
    // Client networkClient;
    // engine.rootContext()->setContextProperty("networkClient", &networkClient);

    // Load QML
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
