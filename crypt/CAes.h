#ifndef ORGANIZER_AES_H
#define ORGANIZER_AES_H

#include <vars.h>
#include <Qca-qt5/QtCrypto/QtCrypto>
#include "CTools.h"

class CAes {
	QCA::SymmetricKey key;
	QCA::InitializationVector iv;

	QCA::Cipher cipher;

	bool encryption;

	static QCA::SecureArray fromBase(const QString &str) { return CTools::fromBase(str); }

	static QString toBase(const QCA::SecureArray &mem) { return CTools::toBase(mem); }

public:
	CAes(QString cipher_name, QString k, QString i = AES_IV);

	QString encrypt(QString message);

	QString decrypt(const QString &message);

	static QString createKey(int key_size = 32);

	static QString createKey(QString k);

	static QString createIV();

};


#endif //ORGANIZER_AES_H
