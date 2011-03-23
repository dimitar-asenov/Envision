/***********************************************************************************************************************
 * StatementItemList.h
 *
 *  Created on: Mar 23, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STATEMENTITEMLIST_H_
#define STATEMENTITEMLIST_H_

#include "../elements/StatementItem.h"
#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class StatementItemList : public Model::TypedList<StatementItem>
{
	NODE_DECLARE_STANDARD_METHODS(StatementItemList)

	virtual Model::Node* navigateTo(Model::Node* source, QString path);
};

}

#endif /* STATEMENTITEMLIST_H_ */
