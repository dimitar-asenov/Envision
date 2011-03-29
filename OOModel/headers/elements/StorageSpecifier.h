/***********************************************************************************************************************
 * StorageSpecifier.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STORAGESPECIFIER_H_
#define STORAGESPECIFIER_H_

#include "../oomodel_api.h"

#include "ModelBase/headers/nodes/Node.h"
#include "ModelBase/headers/nodes/nodeMacros.h"
#include "ModelBase/headers/persistence/PersistentStore.h"

namespace OOModel {

class OOMODEL_API StorageSpecifier :  public Model::Node
{
	NODE_DECLARE_STANDARD_METHODS(StorageSpecifier)

	public:
		enum StorageSpecifierTypes {INSTANCE_VARIABLE, CLASS_VARIABLE};

		StorageSpecifierTypes get() const;
		void set(const StorageSpecifierTypes& newval);

		virtual void save(Model::PersistentStore &store) const;
		virtual void load(Model::PersistentStore &store);

	private:
		StorageSpecifierTypes fromInt(int val);
		StorageSpecifierTypes specifier;
};

inline StorageSpecifier::StorageSpecifierTypes StorageSpecifier::get() const { return specifier; }

}

#endif /* STORAGESPECIFIER_H_ */
