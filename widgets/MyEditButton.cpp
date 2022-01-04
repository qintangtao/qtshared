#include "MyEditButton.h"
#include <QVBoxLayout>
#include <QMouseEvent>

MyEditButton::MyEditButton(QWidget *parent) :
    QPushButton(parent)
{
	initUi();
}
MyEditButton::MyEditButton(const QString &text, QWidget *parent/* = nullptr*/)
	: QPushButton(text, parent)
{
	initUi();
}
MyEditButton::MyEditButton(const QIcon& icon, const QString &text, QWidget *parent/* = nullptr*/)
	: QPushButton(icon, text, parent)
{
	initUi();
}

MyEditButton::~MyEditButton()
{

}

void MyEditButton::setEditEnabled(bool b)
{
    m_pLineEdit->setVisible(b);
	m_pLineEdit->setFocus();
}
void MyEditButton::setEditText(const QString &text)
{
    m_pLineEdit->setText(text);
}

void MyEditButton::contextMenuEvent(QContextMenuEvent *event)
{
	emit contextMenu(event);
	//QPushButton::contextMenuEvent(event);
}


void MyEditButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        setEditEnabled(true);
        setEditText(text());
    }
    QPushButton::mouseDoubleClickEvent(event);
}

void MyEditButton::initUi()
{
	m_pLineEdit = new QLineEdit(this);
	m_pLineEdit->hide();

	//connect(m_pLineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
	connect(m_pLineEdit, static_cast<void (QLineEdit::*)()>(&QLineEdit::returnPressed), [=]() {
		emit returnPressed();
	});

	connect(m_pLineEdit, static_cast<void (QLineEdit::*)()>(&QLineEdit::editingFinished), [=]() {
		emit returnPressed();        
	});
	
	QVBoxLayout *vl = new QVBoxLayout(this);
	vl->addWidget(m_pLineEdit);
}
