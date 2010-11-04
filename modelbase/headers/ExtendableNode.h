/***********************************************************************************************************************
 * ExtendableNode.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXTENDABLENODE_H_
#define EXTENDABLENODE_H_

#include "Node.h"

namespace Model {

class MODELBASE_API ExtendableNode: public Model::Node
{
	public:
		ExtendableNode(Node *parent);
		virtual ~ExtendableNode();
};

}

#endif /* EXTENDABLENODE_H_ */
