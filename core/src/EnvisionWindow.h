#ifndef ENVISIONWINDOW_H
#define ENVISIONWINDOW_H

#include <QtGui/QWidget>

class EnvisionWindow: public QWidget
{
	Q_OBJECT

	public:
		EnvisionWindow(QWidget *parent = 0);
		~EnvisionWindow();
};

#endif // ENVISIONWINDOW_H
