/***********************************************************************************************************************
 * BinaryNode.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODE_H_
#define BINARYNODE_H_

#include "modelbase_api.h"

#include "../nodes/Extendable/ExtendableNode.h"
#include "../nodes/nodeMacros.h"
#include "../nodes/Text.h"

namespace TestNodes {

class MODELBASE_API BinaryNode: public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS(BinaryNode)

	private:
		static Model::ExtendableIndex textIndex;
		static Model::ExtendableIndex leftIndex;
		static Model::ExtendableIndex rightIndex;

	public:
		static void init();

		Model::Text* text();
		BinaryNode* left();
		BinaryNode* right();

		BinaryNode* makeLeftNode(const QString &type = QString());
		BinaryNode* makeRightNode(const QString &type = QString());

		void removeLeftNode();
		void removeRightNode();

};

}

#endif /* BINARYNODE_H_ */
