/***********************************************************************************************************************
 * BinaryNode.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODE_H_
#define BINARYNODE_H_

#include "nodes/ExtendableNode.h"
#include "nodes/Text.h"
#include "nodeMacros.h"

namespace Model {

class BinaryNode: public ExtendableNode<BinaryNode>
{
	NODE_DECLARE_STANDARD_CONSTRUCTORS(BinaryNode)

	private:
		static int textIndex;
		static int leftIndex;
		static int rightIndex;

	public:
		static void init();

		Text* text();
		BinaryNode* left();
		BinaryNode* right();

};

}

#endif /* BINARYNODE_H_ */
