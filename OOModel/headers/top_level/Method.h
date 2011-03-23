/***********************************************************************************************************************
 * Method.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHOD_H_
#define METHOD_H_

#include "../oomodel_api.h"

#include "../attributeMacros.h"
#include "../elements/Visibility.h"
#include "../elements/Static.h"
#include "../elements/StatementItem.h"
#include "../elements/FormalArgument.h"
#include "../elements/FormalResult.h"
#include "../elements/StatementItemList.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/TypedList.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Method : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Method)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(StatementItemList, items, setItems);
	ATTRIBUTE(Model::TypedList<FormalArgument>, arguments, setArguments);
	ATTRIBUTE(Model::TypedList<FormalResult>, results, setResults);
	ATTRIBUTE_OOP_VISIBILITY
	ATTRIBUTE_OOP_STATIC

	public:
		virtual bool definesSymbol() const;
		virtual const QString& symbolName() const;
};

}

#endif /* METHOD_H_ */
