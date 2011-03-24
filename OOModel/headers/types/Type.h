/***********************************************************************************************************************
 * Type.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TYPE_H_
#define TYPE_H_

#include "../oomodel_api.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class Class;

class OOMODEL_API Type : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Type)

	public:

		/**
		 * Returns the class definition node that for this type node.
		 *
		 * In case this type node points to a class this method returns the class node, otherwise it returns NULL.
		 */
		virtual Class* classDefinition();
};

}

#endif /* TYPE_H_ */
