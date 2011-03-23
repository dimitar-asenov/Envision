/***********************************************************************************************************************
 * Module.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODULE_H_
#define MODULE_H_

#include "../oomodel_api.h"

#include "../attributeMacros.h"
#include "Class.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/TypedList.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class Project;

class OOMODEL_API Module : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Module)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Model::TypedList<Module>, modules, setModules);
	ATTRIBUTE(Model::TypedList<Class>, classes, setClasses);

	public:
		virtual bool definesSymbol() const;
		virtual const QString& symbolName() const;

		virtual Model::Node* navigateTo(Model::Node* source, QString path);
};

}

#endif /* MODULE_H_ */
