/***********************************************************************************************************************
 * selftest.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SELFTEST_H
#define SELFTEST_H

#include "EnvisionPlugin.h"

namespace SelfTest {

class SelfTest : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // SELFTEST_H
