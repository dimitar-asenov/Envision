#include <iostream>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include "GenTool.h"
#include "Config.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	GenTool t;
	t.setSubDirPath(Config::instance().envisionReadPath());
	t.run();
	qDebug() << "Done!";
	return a.exec();
}

