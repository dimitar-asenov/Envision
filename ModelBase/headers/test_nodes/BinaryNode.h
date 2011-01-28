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

	ATTRIBUTE(Model::Text, name)
	ATTRIBUTE(BinaryNode, left)
	ATTRIBUTE(BinaryNode, right)

	public:
		BinaryNode* makeLeftNode(const QString &type = QString());
		BinaryNode* makeRightNode(const QString &type = QString());

		void removeLeftNode();
		void removeRightNode();

};
inline BinaryNode* BinaryNode::makeLeftNode(const QString &type) { return createOptional<BinaryNode>(leftIndex, type); }
inline BinaryNode* BinaryNode::makeRightNode(const QString &type) { return createOptional<BinaryNode>(rightIndex, type); }

inline void BinaryNode::removeLeftNode() { removeOptional(leftIndex); }
inline void BinaryNode::removeRightNode() { removeOptional(rightIndex); }

}

#endif /* BINARYNODE_H_ */
