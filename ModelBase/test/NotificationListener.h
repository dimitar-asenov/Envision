/***********************************************************************************************************************
 * NotificationListener.h
 *
 *  Created on: Nov 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NOTIFICATIONLISTENER_H_
#define NOTIFICATIONLISTENER_H_

#include "Model.h"
#include "nodes/Node.h"

#include <QtCore/QList>
#include <QtCore/QObject>

namespace Model {

class NotificationListener : public QObject
{
	Q_OBJECT

	public:
		QList<Node*> modifiedNodes;
		Node* root;

		NotificationListener(Model& model);

	public slots:
		void setModifiedNodes( QList<Node*> nodes);
		void rootCreated(Node* root);
};

}

#endif /* NOTIFICATIONLISTENER_H_ */
