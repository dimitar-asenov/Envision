/***********************************************************************************************************************
 * BinaryWithPosition.h
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYWITHPOSITION_H_
#define BINARYWITHPOSITION_H_

#include "../modelbase_api.h"
#include "../nodes/Integer.h"

#include "BinaryNode.h"

namespace TestNodes {

class MODELBASE_API BinaryWithPosition
{
	private:
		BinaryNode* binaryNode;
		static Model::ExtendableIndex xIndex;
		static Model::ExtendableIndex yIndex;

	public:
		BinaryWithPosition(BinaryNode* binary);
		virtual ~BinaryWithPosition();

		BinaryNode* binary();
		int x();
		int y();

		void set(int x, int y);

		static void init();
};

inline BinaryNode* BinaryWithPosition::binary() { return binaryNode; }
inline int BinaryWithPosition::x() { return static_cast<Model::Integer*> (binaryNode->get(xIndex))->get(); }
inline int BinaryWithPosition::y() { return static_cast<Model::Integer*> (binaryNode->get(yIndex))->get(); }

}

#endif /* BINARYWITHPOSITION_H_ */
