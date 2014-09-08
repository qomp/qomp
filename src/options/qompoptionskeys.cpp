/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#include "qompoptionskeys.h"
#include "qompactionslist.h"
#include "ui_qompoptionskeys.h"

#include <QCloseEvent>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

//--------------------------------------------------------
//---GrepShortcutKeyDialog from libpsi with some changes--
//--------------------------------------------------------
class GrepShortcutKeyDialog : public QDialog
{
	Q_OBJECT
public:
	GrepShortcutKeyDialog(QTreeWidgetItem *i, QWidget* p = 0):
		QDialog(p),
		gotKey(false),
		item_(i)
	{
		setAttribute(Qt::WA_DeleteOnClose);
		setModal(true);
		setWindowTitle(tr("New Shortcut"));
		QHBoxLayout *l = new QHBoxLayout(this);
		le = new QLineEdit();
		l->addWidget(le);
		QPushButton *cancelButton = new QPushButton(tr("Cancel"));
		l->addWidget(cancelButton);
		connect(cancelButton, SIGNAL(clicked()), SLOT(close()));

		displayPressedKeys(QKeySequence());

		adjustSize();
		setFixedSize(size());
	}

	void show()
	{
		QDialog::show();
		grabKeyboard();
	}

protected:
	void closeEvent(QCloseEvent *event)
	{
		releaseKeyboard();
		event->accept();
	}

	void keyPressEvent(QKeyEvent* event)
	{
		displayPressedKeys(getKeySequence(event));

		if (!isValid(event->key()) || gotKey)
			return;

		gotKey = true;
		emit newShortcutKey(item_, getKeySequence(event));
		close();
	}

	void keyReleaseEvent(QKeyEvent* event)
	{
		displayPressedKeys(getKeySequence(event));
	}

signals:
	void newShortcutKey(QTreeWidgetItem *i, const QKeySequence& key);

private:
	void displayPressedKeys(const QKeySequence& keys)
	{
		QString str = keys.toString(QKeySequence::NativeText);
		if (str.isEmpty())
			str = tr("Set Keys");
		le->setText(str);
	}

	QKeySequence getKeySequence(QKeyEvent* event) const
	{
		return QKeySequence((isValid(event->key()) ? event->key() : 0)
				    + (event->modifiers() & ~Qt::KeypadModifier));
	}

	bool isValid(int key) const
	{
		switch (key) {
		case 0:
		case Qt::Key_unknown:
			return false;
		}

		return !isModifier(key);
	}

	bool isModifier(int key) const
	{
		switch (key) {
		case Qt::Key_Shift:
		case Qt::Key_Control:
		case Qt::Key_Meta:
		case Qt::Key_Alt:
		case Qt::Key_AltGr:
		case Qt::Key_Super_L:
		case Qt::Key_Super_R:
		case Qt::Key_Menu:
			return true;
		}
		return false;
	}

	bool gotKey;
	QLineEdit* le;
	QTreeWidgetItem *item_;
};

class QompOptionsKeys::Private
{
public:
	Private(QompOptionsKeys* page) :
		page_(page),
		ui(new Ui::QompOptionsKeys),
		widget_(new QWidget)
	{
		ui->setupUi(widget_);
	}

	QompOptionsKeys* page_;
	Ui::QompOptionsKeys *ui;
	QWidget* widget_;
};

QompOptionsKeys::QompOptionsKeys(QObject *parent) :
	QompOptionsPage(parent)
{
	d = new Private(this);
	connect(d->ui->keysTree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), SLOT(getShortcut(QTreeWidgetItem*)));
	restoreOptions();
}

QompOptionsKeys::~QompOptionsKeys()
{
	delete d->ui;
	delete d;
}

void QompOptionsKeys::retranslate()
{
	d->ui->retranslateUi(d->widget_);
}

QObject *QompOptionsKeys::page() const
{
	return d->widget_;
}

void QompOptionsKeys::applyOptions()
{
	QTreeWidgetItem *i = d->ui->keysTree->topLevelItem(0);
	while(i) {
		QompActionsList::QompActionType t = QompActionsList::QompActionType(i->data(2, Qt::DisplayRole).toInt());
		QompActionsList::instance()->updateAction(t, QKeySequence::fromString( i->data(1, Qt::DisplayRole).toString()) );
		i = d->ui->keysTree->itemBelow(i);
	}
	QompActionsList::instance()->saveToOptions();
}

void QompOptionsKeys::restoreOptions()
{
	d->ui->keysTree->clear();
	QList<QompActionsList::QompAction> list = QompActionsList::instance()->actions();
	foreach(const QompActionsList::QompAction& act, list) {
		new QTreeWidgetItem(d->ui->keysTree, QStringList() << act.name
								<< act.shortcut.toString()
								<< QString::number(act.type));
	}
}

void QompOptionsKeys::getShortcut(QTreeWidgetItem *i)
{
	GrepShortcutKeyDialog* dlg = new GrepShortcutKeyDialog(i, d->widget_);
	connect(dlg, SIGNAL(newShortcutKey(QTreeWidgetItem*,QKeySequence)), SLOT(shortcutReady(QTreeWidgetItem*,QKeySequence)));
	dlg->show();
}

void QompOptionsKeys::shortcutReady(QTreeWidgetItem *i, const QKeySequence &key)
{
	i->setData(1, Qt::DisplayRole, key.toString());
}

#include "qompoptionskeys.moc"
