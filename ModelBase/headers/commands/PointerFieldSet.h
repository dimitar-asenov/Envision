/***********************************************************************************************************************
 * PointerFieldSet.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef POINTERFIELDSET_H_
#define POINTERFIELDSET_H_

#include "FieldSet.h"

#include "Core/headers/global.h"

namespace Model {

template<class T>
class MODELBASE_API PointerFieldSet: public FieldSet<T*>
{
	public:
		PointerFieldSet(Node *target, T* &fieldToSet, T* setTo) :
			FieldSet<T*>(target, fieldToSet, setTo)
		{
		}

		~PointerFieldSet()
		{
			if (this->isUndone() && this->newValue) SAFE_DELETE(this->newValue);
			if (!this->isUndone() && this->oldValue) SAFE_DELETE(this->oldValue);
		}
};

}

#endif /* POINTERFIELDSET_H_ */
