#ifndef MyEditButton_H
#define MyEditButton_H

#include <QPushButton>
#include <QLineEdit>

class MyEditButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MyEditButton(QWidget *parent = nullptr);
	explicit MyEditButton(const QString &text, QWidget *parent = nullptr);
	MyEditButton(const QIcon& icon, const QString &text, QWidget *parent = nullptr);

    ~MyEditButton();

	bool isEditEnabled();
	void setEditEnabled(bool b);

	QString editText() const;
	void setEditText(const QString &text);

Q_SIGNALS:
	void returnPressed();
	void contextMenu(QContextMenuEvent *);

protected:
	void contextMenuEvent(QContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
	void initUi();

private:
    QLineEdit *m_pLineEdit;
};

inline bool MyEditButton::isEditEnabled() {
	return m_pLineEdit->isVisible();
}
inline QString MyEditButton::editText() const {
	return m_pLineEdit->text();
}

#endif // MyEditButton_H
