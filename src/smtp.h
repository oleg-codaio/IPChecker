#ifndef SMTP_H
#define SMTP_H
 
#include <QTcpSocket>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
	
class Smtp : public QObject
{
	Q_OBJECT

public:
	Smtp( const QString &from, const QString &to,
	const QString &subject, const QString &body );
	~Smtp();

signals:
	void status( const QString &);

private slots:
	void stateChanged(QAbstractSocket::SocketState socketState);
	void errorReceived(QAbstractSocket::SocketError socketError);
	void disconnected();
	void connected();
	void readyRead();

private:
	QString message;
	QTextStream *t;
	QTcpSocket *socket;
	QString from;
	QString rcpt;
	QString response;
	enum states{Rcpt,Mail,Data,Init,Body,Quit,Close};
	int state;
};
#endif 
