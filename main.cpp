#include <iostream>

#include <QtCore/QDebug>

#include "GenTool.h"

int main()
{
	GenTool t;
	t.initPath("/home/luke/Playground/Envision/OOModel/");
	t.run();
	qDebug() << "Done!";
	return 0;
}

