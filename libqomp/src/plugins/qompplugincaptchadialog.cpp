/*
 * Copyright (C) 2016  Khryukin Evgeny
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

#include "qompplugincaptchadialog.h"

#include <QPixmap>

#ifdef QOMP_MOBILE
#include <QQuickItem>
#include <QEventLoop>
#include <QQuickImageProvider>
#include "qompqmlengine.h"
#else
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QLineEdit>
#endif

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

class QompPluginCaptchaDialog::Private
{
#ifdef QOMP_MOBILE
	class ImageProvider : public QQuickImageProvider
	{
	public:
		explicit ImageProvider(const QPixmap& p) :
			QQuickImageProvider(QQmlImageProviderBase::Pixmap),
			_pix(p)
		{
		}

		~ImageProvider() {}

		QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
		{
			Q_UNUSED(id)
			Q_UNUSED(size)
			Q_UNUSED(requestedSize)
			return _pix;
		}

		static QString provId()
		{
			return QStringLiteral("captcha");
		}

	private:
		QPixmap _pix;
	};
#endif

public:
	explicit Private(const QPixmap& pix) :
		_pix(pix)
	{
	}

	~Private()
	{
#ifdef QOMP_MOBILE
		if(_loop.isRunning())
			_loop.exit();

		QompQmlEngine::instance()->removeItem();
		QompQmlEngine::instance()->removeImageProvider(ImageProvider::provId());
#endif
	}

	bool start()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "QompPluginCaptchaDialog::Private::start()";
#endif
		if(_pix.isNull()) {
#ifdef DEBUG_OUTPUT
		qDebug() << "NULL Captcha!";
#endif
			return false;
		}
		bool res = false;

#ifdef QOMP_MOBILE

		QQuickItem* item = QompQmlEngine::instance()->createItem(QUrl("qrc:///qmlshared/QompCaptchaDlg.qml"));
		ImageProvider *prov = new ImageProvider(_pix);
		QompQmlEngine::instance()->addImageProvider(ImageProvider::provId(), prov);

		item->setProperty("captcha", QString("image://%1/image").arg(ImageProvider::provId()));
		item->setProperty("aspect", double(_pix.height()) / double(_pix.width()));

		connect(item, SIGNAL(accepted()), &_loop, SLOT(quit()));
		QompQmlEngine::instance()->addItem(item);
		_loop.exec();
		res = item->property("status").toBool();
		if(res) {
			_res = item->property("text").toString();
		}
#else
		QDialog dlg;

		dlg.setWindowTitle(tr("Enter the text from the image"));

		QLabel* lbl = new QLabel;
		lbl->setPixmap(_pix);
		lbl->setFixedSize(_pix.size());

		QHBoxLayout *lblLayout = new QHBoxLayout;
		lblLayout->addStretch();
		lblLayout->addWidget(lbl);
		lblLayout->addStretch();

		QLineEdit *le = new QLineEdit;
		QHBoxLayout *leLayout = new QHBoxLayout;
		leLayout->addWidget(le);

		QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(bb, SIGNAL(accepted()), &dlg, SLOT(accept()));
		connect(bb, SIGNAL(rejected()), &dlg, SLOT(reject()));

		QHBoxLayout *bbLayout = new QHBoxLayout;
		bbLayout->addStretch();
		bbLayout->addWidget(bb);


		QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);
		mainLayout->addLayout(lblLayout);
		mainLayout->addLayout(leLayout);
		mainLayout->addLayout(bbLayout);

		dlg.setLayout(mainLayout);
		dlg.adjustSize();

		le->setFocus();

		if(dlg.exec() == QDialog::Accepted) {
			_res = le->text();
			res = true;
		}
#endif
		return res;
	}

	QString result() const
	{
		return _res;
	}

private:
	QPixmap _pix;
	QString _res;
#ifdef QOMP_MOBILE
	QEventLoop _loop;
#endif
};

QompPluginCaptchaDialog::QompPluginCaptchaDialog(const QPixmap &captcha, QObject *parent) :
	QObject(parent),
	d(new Private(captcha))
{
}

QompPluginCaptchaDialog::~QompPluginCaptchaDialog()
{
	delete d;
}

bool QompPluginCaptchaDialog::start()
{
	return d->start();
}

QString QompPluginCaptchaDialog::result() const
{
	return d->result();
}
