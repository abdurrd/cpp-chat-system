/**
 * Message Model - Phase 2: Qt/QML GUI
 * 
 * QAbstractListModel to expose messages to QML
 * Handles message storage and updates
 */

#include <QAbstractListModel>
#include <QString>
#include <QDateTime>
#include <QVector>

struct Message {
    QString text;
    QString sender;
    QDateTime timestamp;
    bool isFromMe;
    bool isRead;
};

class MessageModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum MessageRoles {
        TextRole = Qt::UserRole + 1,
        SenderRole,
        TimestampRole,
        IsFromMeRole,
        IsReadRole
    };

    explicit MessageModel(QObject *parent = nullptr);

    // Basic QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // TODO: Add methods for QML
    // Q_INVOKABLE void addMessage(const QString &text, const QString &sender);
    // Q_INVOKABLE void markAsRead(int index);
    // Q_INVOKABLE void clear();

private:
    QVector<Message> m_messages;
};
