/**
 * Network Client Wrapper - Phase 2: Qt/QML GUI
 * 
 * Wraps C++ networking code for QML integration
 * Uses Qt's signal/slot for thread-safe communication
 */

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QQueue>

class NetworkClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit NetworkClient(QObject *parent = nullptr);
    
    bool isConnected() const { return m_connected; }

    // Methods exposed to QML
    Q_INVOKABLE void connectToServer(const QString &host, quint16 port);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendMessage(const QString &message);

signals:
    void connectedChanged();
    void messageReceived(const QString &message, const QString &sender);
    void connectionError(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;
    bool m_connected = false;
    QQueue<QString> m_messageQueue;
};
