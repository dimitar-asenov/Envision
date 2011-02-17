/***********************************************************************************************************************
 * Visibility.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VISIBILITY_H_
#define VISIBILITY_H_

#include "../oomodel_api.h"

#include "ModelBase/headers/nodes/Node.h"
#include "ModelBase/headers/nodes/nodeMacros.h"
#include "ModelBase/headers/persistence/PersistentStore.h"

namespace OOModel {

class OOMODEL_API Visibility :  public Model::Node
{
	NODE_DECLARE_STANDARD_METHODS(Visibility)

	public:
		enum VisibilityType {DEFAULT, PUBLIC, PRIVATE, PROTECTED};

		VisibilityType get() const;
		void set(const VisibilityType& newval);

		virtual void save(Model::PersistentStore &store) const;
		virtual void load(Model::PersistentStore &store);

	private:
		VisibilityType fromInt(int val);
		VisibilityType vis;
};

inline Visibility::VisibilityType Visibility::get() const { return vis; }

}

#endif /* VISIBILITY_H_ */
