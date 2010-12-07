#ifndef ENVISIONWINDOW_H
#define ENVISIONWINDOW_H

#include <QtGui/QMainWindow>

class EnvisionWindow: public QMainWindow
{
	Q_OBJECT

	public:
		EnvisionWindow(QWidget *parent = 0);
		~EnvisionWindow();
};

#endif // ENVISIONWINDOW_H
