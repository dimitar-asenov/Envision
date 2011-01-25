/***********************************************************************************************************************
 * BinaryNodeAccessUnit.h
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODEACCESSUNIT_H_
#define BINARYNODEACCESSUNIT_H_

#include "../modelbase_api.h"

#include "BinaryNode.h"
#include "../NodeReadWriteLock.h"

namespace TestNodes {

class MODELBASE_API BinaryNodeAccessUnit: public BinaryNode
{
	EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS(BinaryNodeAccessUnit)

	private:
		Model::NodeReadWriteLock accessLock_;

	public:
		static void init();

		virtual Model::NodeReadWriteLock* accessLock() const;
};

}

#endif /* BINARYNODEACCESSUNIT_H_ */
