/***********************************************************************************************************************
 * Statement.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STATEMENT_H_
#define STATEMENT_H_

#include "../oomodel_api.h"
#include "../StatementItem.h"

namespace OOModel {

class OOMODEL_API Statement: public StatementItem
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Statement);
};

}

#endif /* STATEMENT_H_ */
