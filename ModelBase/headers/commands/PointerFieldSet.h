/***********************************************************************************************************************
 * PointerFieldSet.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef POINTERFIELDSET_H_
#define POINTERFIELDSET_H_

#include "FieldSet.h"

namespace Model {

template<class T>
class PointerFieldSet: public FieldSet<T*>
{
	public:
		PointerFieldSet(Node *target, T* &fieldToSet, T* setTo) :
			FieldSet<T*>(target, fieldToSet, setTo)
		{
		}

		~PointerFieldSet()
		{
			if (this->isUndone() && this->newValue) delete this->newValue;
			if (!this->isUndone() && this->oldValue) delete this->oldValue;
		}
};

}

#endif /* POINTERFIELDSET_H_ */
