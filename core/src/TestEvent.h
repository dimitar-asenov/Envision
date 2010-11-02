/***********************************************************************************************************************
 * TestEvent.h
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TESTEVENT_H_
#define TESTEVENT_H_

#include <QEvent>
#include <QString>

namespace Envision {

class TestEvent: public QEvent
{
	private:
		QString testTarget;
		QString testId;

	public:
		TestEvent(QString testTarget, QString testId);
		QString getTarget();
		QString getId();
};

}

#endif /* TESTEVENT_H_ */
