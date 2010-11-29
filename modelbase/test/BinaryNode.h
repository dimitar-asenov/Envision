/***********************************************************************************************************************
 * BinaryNode.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODE_H_
#define BINARYNODE_H_

#include "nodes/Extendable/ExtendableNode.h"
#include "nodes/nodeMacros.h"
#include "nodes/Text.h"

namespace Model {

class BinaryNode: public ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS(BinaryNode)

	private:
		static ExtendableIndex textIndex;
		static ExtendableIndex leftIndex;
		static ExtendableIndex rightIndex;

	public:
		static void init();

		Text* text();
		BinaryNode* left();
		BinaryNode* right();

		BinaryNode* makeLeftNode(const QString &type = QString());
		BinaryNode* makeRightNode(const QString &type = QString());

		void removeLeftNode();
		void removeRightNode();

};

}

#endif /* BINARYNODE_H_ */
