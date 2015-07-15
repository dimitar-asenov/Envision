#include <iostream>

#include <QtCore/QDebug>

#include "GenTool.h"

int main()
{
	GenTool t;
	t.setSubDirPath("/home/luke/Playground/Envision/");
	t.run();
	qDebug() << "Done!";
	return 0;
}

