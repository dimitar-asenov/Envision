/***********************************************************************************************************************
 * BinaryNodeUnit.h
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODEUNIT_H_
#define BINARYNODEUNIT_H_

#include "BinaryNode.h"
#include "NodeReadWriteLock.h"

namespace Model {

class BinaryNodeUnit: public BinaryNode
{
	EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS(BinaryNodeUnit)

	private:
		NodeReadWriteLock accessLock;

	public:
		static void init();

		virtual NodeReadWriteLock* getAccessLock() const;
};

}

#endif /* BINARYNODEUNIT_H_ */
