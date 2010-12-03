/***********************************************************************************************************************
 * BinaryNodeUnit.h
 *
 *  Created on: Dec 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYNODEUNIT_H_
#define BINARYNODEUNIT_H_

#include "BinaryNode.h"

namespace FilePersistence {

class BinaryNodeUnit: public BinaryNode
{
	EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS(BinaryNodeUnit)

	public:
		static void init();

		virtual bool isNewPersistenceUnit() const;
};

}

#endif /* BINARYNODEUNIT_H_ */
