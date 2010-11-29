/***********************************************************************************************************************
 * BinaryWithPosition.h
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYWITHPOSITION_H_
#define BINARYWITHPOSITION_H_

#include "BinaryNode.h"

namespace Model {


class BinaryWithPosition
{
	private:
		BinaryNode* binaryNode;
		static ExtendableIndex xIndex;
		static ExtendableIndex yIndex;

	public:
		BinaryWithPosition(BinaryNode* binary);
		virtual ~BinaryWithPosition();

		BinaryNode* binary();
		int x();
		int y();

		void set(int x, int y);

		static void init();
};

}

#endif /* BINARYWITHPOSITION_H_ */
