/***********************************************************************************************************************
 * Block.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BLOCK_H_
#define BLOCK_H_

#include "Statement.h"

#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class OOMODEL_API Block: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Block)

	ATTRIBUTE(Model::TypedList<Statement>, statements, setStatements);
};

}

#endif /* BLOCK_H_ */
