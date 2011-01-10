/***********************************************************************************************************************
 * BinaryNodePersistenceUnit.h
 *
 *  Created on: Dec 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODEPERSISTENCEUNIT_H_
#define BINARYNODEPERSISTENCEUNIT_H_

#include "../modelbase_api.h"

#include "BinaryNode.h"

namespace TestNodes {

class MODELBASE_API BinaryNodePersistenceUnit: public BinaryNode
{
	EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS(BinaryNodePersistenceUnit)

	public:
		static void init();

		virtual bool isNewPersistenceUnit() const;
};

}

#endif /* BINARYNODEPERSISTENCEUNIT_H_ */
