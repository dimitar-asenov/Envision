/***********************************************************************************************************************
 * Class.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CLASS_H_
#define CLASS_H_

#include "oomodel_api.h"

#include "common/Name.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API Class : public Model::ExtendableNode, public Name<Class>
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Class)

	private:
		static Model::ExtendableIndex fieldsIndex;
		static Model::ExtendableIndex methodsIndex;

	public:
		static void init();

		Model::List* fields();
		Model::List* methods();
};

inline Model::List* Class::fields() { return static_cast<Model::List*> (get(fieldsIndex)); }
inline Model::List* Class::methods() { return static_cast<Model::List*> (get(methodsIndex)); }

}

#endif /* CLASS_H_ */
