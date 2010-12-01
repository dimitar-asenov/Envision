/***********************************************************************************************************************
 * NotificationListener.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "NotificationListener.h"

namespace Model {

NotificationListener::NotificationListener(Model& model) : root(NULL)
{
	QObject::connect(&model, SIGNAL(nodesModified(QList<Node*>)), this, SLOT(setModifiedNodes(QList<Node*>)));
	QObject::connect(&model, SIGNAL(rootCreated(Node*)), this, SLOT(rootCreated(Node*)));

}

void NotificationListener::setModifiedNodes( QList<Node*> nodes)
{
	modifiedNodes = nodes;
}

void NotificationListener::rootCreated(Node* root_)
{
	root = root_;
}

}
