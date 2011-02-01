/***********************************************************************************************************************
 * Static.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STATIC_H_
#define STATIC_H_

#include "../oomodel_api.h"

#include "ModelBase/headers/nodes/Node.h"
#include "ModelBase/headers/nodes/nodeMacros.h"
#include "ModelBase/headers/persistence/PersistentStore.h"

namespace OOModel {

class OOMODEL_API Static :  public Model::Node
{
	NODE_DECLARE_STANDARD_METHODS(Static)

	public:
		enum StaticType {INSTANCE_VARIABLE, CLASS_VARIABLE};

		StaticType get() const;
		void set(const StaticType& newval);

		virtual void save(Model::PersistentStore &store) const;
		virtual void load(Model::PersistentStore &store);

	private:
		StaticType fromInt(int val);
		StaticType stat;
};

inline Static::StaticType Static::get() const { return stat; }

}

#endif /* STATIC_H_ */
