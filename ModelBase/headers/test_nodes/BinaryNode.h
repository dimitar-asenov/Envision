/***********************************************************************************************************************
 * BinaryNode.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODE_H_
#define BINARYNODE_H_

#include "../modelbase_api.h"

#include "../nodes/Extendable/ExtendableNode.h"
#include "../nodes/nodeMacros.h"
#include "../nodes/Text.h"

namespace TestNodes {

class MODELBASE_API BinaryNode: public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(BinaryNode)

	ATTRIBUTE(Model::Text, name, setName)
	ATTRIBUTE(BinaryNode, left, setLeft)
	ATTRIBUTE(BinaryNode, right, setRight)

	public:
		void removeLeftNode();
		void removeRightNode();

};

inline void BinaryNode::removeLeftNode() { removeOptional(leftIndex); }
inline void BinaryNode::removeRightNode() { removeOptional(rightIndex); }

}

#endif /* BINARYNODE_H_ */
