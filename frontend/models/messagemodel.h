/**
 * Message Model Header - Phase 2: Qt/QML GUI
 */

#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

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

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	// Methods for QML
	Q_INVOKABLE void addMessage(const QString &text, const QString &sender, bool fromMe = false);
	Q_INVOKABLE void markAsRead(int index);
	Q_INVOKABLE void clear();

private:
	QVector<Message> m_messages;
};

#endif // MESSAGEMODEL_H
