#include <iostream>

#include <QtCore/QCoreApplication>

#include "GenTool.h"
#include "Config.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	GenTool t;
	t.setSubDirPath(Config::instance().envisionReadPath());
	t.run();
	std::cout << "Done!" << std::endl;
	a.quit();
	return 0;
}

