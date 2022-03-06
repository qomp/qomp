/*
 * Copyright (C) 2017  Khryukin Evgeny
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

#include "extendedcombobox.h"

#include <QStandardItemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QKeyEvent>


//-------------------------------------//
//------------ComboLineEdit------------//
//-------------------------------------//
class ComboLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit ComboLineEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}

signals:
	void escapePressed();
	void keyPressed(int key);

protected:
	virtual void keyPressEvent(QKeyEvent* e) Q_DECL_OVERRIDE
	{
		switch (e->key()) {
		case Qt::Key_Escape:
			emit escapePressed();
			e->accept();
			window()->setFocus();
			break;
		case Qt::Key_Return:
		case Qt::Key_Enter:
			emit returnPressed();
			e->accept();
			window()->setFocus();
			break;
		default:
			QLineEdit::keyPressEvent(e);
		}

		emit keyPressed(e->key());
	}
};


//-------------------------------------//
//------ExtendedComboBox::Private------//
//-------------------------------------//
class ExtendedComboBox::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(ExtendedComboBox* parent) :
		QObject(parent),
		_parent(parent)
	{
	}

	void clearFiltering()
	{
		updateFilter(QString());
	}

	QSortFilterProxyModel *filterModel() const
	{
		return static_cast<QSortFilterProxyModel*>(_parent->model());
	}

	void updateFilter(const QString& text)
	{
		auto model = filterModel();
		auto view = _parent->view();

		const QModelIndex realIndex = model->mapToSource(view->currentIndex());
		_parent->blockSignals(true);

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
		model->setFilterRegExp(QString("^%1.").arg(text));
#else
		model->setFilterRegularExpression(QString("^%1.").arg(text));
#endif
		model->invalidate();
		_parent->blockSignals(false);

		//after sorting selected index may change
		const QModelIndex index = model->mapFromSource(realIndex);
		view->setCurrentIndex(index);
	}

public slots:
	void onLineEditTextEdited(int key)
	{

		QKeySequence ke((Qt::Key)key);
		bool isAlphaNum = (ke.toString().length() == 1);

		if(isAlphaNum || key == Qt::Key_Backspace || key == Qt::Key_Delete) {
			const QString text = _parent->lineEdit()->text();
			updateFilter(text); //after filtering text at lineedit will be cleared in most cases
			_parent->lineEdit()->setText(text);
		}

		if(isAlphaNum) {
			QSignalBlocker b(_parent->view());
			_parent->showPopup();
			_parent->view()->setCurrentIndex(filterModel()->index(0,0));
		}
	}

	void onLineEditReturnPressed()
	{
		_parent->hidePopup();
	}

	void onLineEditEscapePressed()
	{
		_parent->lineEdit()->undo();
		clearFiltering();
	}

	void currentIndexChanged(const QModelIndex& index)
	{
		auto le = _parent->lineEdit();
		le->setText(index.data().toString());
		le->selectAll();
	}

protected:
	virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE
	{
		if(watched == _parent && event->type() == QEvent::FocusOut) {
			if(!_parent->view()->isVisible() && !_parent->currentText().isEmpty())
				_parent->setCurrentText(_parent->itemText(_parent->currentIndex()));
		}
		else if(watched == _parent->view() && event->type() == QEvent::Hide) {
			clearFiltering();
		}

		return QObject::eventFilter(watched, event);
	}

public:
	ExtendedComboBox* _parent;
};


//-------------------------------------//
//----------ExtendedComboBox-----------//
//-------------------------------------//
ExtendedComboBox::ExtendedComboBox(QWidget *parent) :
	QComboBox(parent),
	d(new Private(this))
{
	auto model = new QStandardItemModel(this);

	auto proxy = new QSortFilterProxyModel(this);
	proxy->setSourceModel(model);
	proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	proxy->setDynamicSortFilter(false);
	setModel(proxy);

	auto *view = new QTreeView(this);
	setView(view);
	view->setIndentation(0);

	QHeaderView* header = view->header();
	header->setStretchLastSection(false);
	header->setSectionsMovable(false);
	header->setDefaultAlignment(Qt::AlignHCenter);

	setHeaderHidden(true);

	auto le = new ComboLineEdit(this);
	setLineEdit(le);
	le->setClearButtonEnabled(true);
	le->setCompleter(nullptr);
	le->disconnect(this);

	connect(le, &ComboLineEdit::keyPressed,      d, &ExtendedComboBox::Private::onLineEditTextEdited);
	connect(le, &ComboLineEdit::returnPressed,   d, &ExtendedComboBox::Private::onLineEditReturnPressed);
	connect(le, &ComboLineEdit::escapePressed,   d, &ExtendedComboBox::Private::onLineEditEscapePressed);
	connect(view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), d, SLOT(currentIndexChanged(QModelIndex)));

	installEventFilter(d);
	view->installEventFilter(d);
}

void ExtendedComboBox::setHeaderHidden(bool hide)
{
	static_cast<QTreeView*>(view())->setHeaderHidden(hide);
}

bool ExtendedComboBox::isHeaderHidden() const
{
	return static_cast<QTreeView*>(view())->isHeaderHidden();
}

void ExtendedComboBox::setHorizontalHeaderLabels(const QStringList &labels)
{
	static_cast<QStandardItemModel*>(d->filterModel()->sourceModel())->setHorizontalHeaderLabels(labels);
	QHeaderView* header =  static_cast<QTreeView*>(view())->header();
	header->setSectionResizeMode(QHeaderView::ResizeToContents);
	header->setSectionResizeMode(0, QHeaderView::Stretch);
}


void ExtendedComboBox::addItem(const QVariantList& item)
{
	auto model = d->filterModel();
	int cnt = model->columnCount();
	int columns = item.count();
	if(cnt < columns)
		model->insertColumns(cnt, columns - cnt);

	cnt = model->rowCount();
	model->insertRow(cnt);

	for(int i = 0; i < columns; ++i) {
		const QModelIndex ind = model->index(cnt, i);
		const QVariant& v = item.at(i);
		model->setData(ind, v);
	}
}


#include "extendedcombobox.moc"
