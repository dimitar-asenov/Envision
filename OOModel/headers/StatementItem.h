/***********************************************************************************************************************
 * StatementItem.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STATEMENTITEM_H_
#define STATEMENTITEM_H_

#include "oomodel_api.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API StatementItem : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(StatementItem)
};

}

#endif /* STATEMENTITEM_H_ */
