/***********************************************************************************************************************
 * BinaryNode.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODE_H_
#define BINARYNODE_H_

#include "nodes/ExtendableNode.h"
#include "nodeMacros.h"

namespace Model {

class Text;

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

		BinaryNode* makeLeftNode();
		BinaryNode* makeRightNode();

		void removeLeftNode();
		void removeRightNode();

};

}

#endif /* BINARYNODE_H_ */
