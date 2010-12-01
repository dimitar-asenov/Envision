/***********************************************************************************************************************
 * TestEvent.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "TestEvent.h"

namespace Envision {

TestEvent::TestEvent(QString testTarget_, QString testId_) : QEvent(QEvent::User), testTarget(testTarget_), testId(testId_)
{
}

QString TestEvent::getTarget()
{
	return testTarget;
}

QString TestEvent::getId()
{
	return testId;
}

}
