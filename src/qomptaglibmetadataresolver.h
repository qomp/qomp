/*
 * Copyright (C) 2014-2017  Khryukin Evgeny
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef QOMPTAGLIBMETADATARESOLVER_H
#define QOMPTAGLIBMETADATARESOLVER_H

#include "qompmetadataresolver.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace TagLib {
class IOStream;
class ByteVector;
namespace MPEG {
class File;
}
}

class QompTagLibMetaDataResolver : public QompMetaDataResolver
{
	Q_OBJECT
public:
	explicit QompTagLibMetaDataResolver(QObject *parent = 0);

protected:
	virtual void start() Q_DECL_OVERRIDE;

private slots:
	void dataReady();

private:
	void resolveNextMedia();
	TagLib::MPEG::File* createFile(const QString &url, TagLib::IOStream *stream);
	void processData(const QUrl& url, const TagLib::ByteVector& data);
	void loadFullSize(const QUrl& url, const QByteArray& data);

private:
	QNetworkAccessManager* nam_;
	quint32 fullSize_;
	char step_;

};

#endif // QOMPTAGLIBMETADATARESOLVER_H
