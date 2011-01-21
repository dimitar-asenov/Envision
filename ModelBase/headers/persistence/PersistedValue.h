/***********************************************************************************************************************
 * PersistedValue.h
 *
 *  Created on: Jan 20, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PERSISTEDSTRING_H_
#define PERSISTEDSTRING_H_

#include "../modelbase_api.h"
#include "PersistedNode.h"

#include "Core/headers/global.h"

namespace Model {

template <class T>
class MODELBASE_API PersistedValue: public PersistedNode
{
	public:
		void set(const T& value);
		T& value();

	private:
		T value_;
};

template <class T> inline void PersistedValue<T>::set(const T& value) { value_ = value; }
template <class T> inline T& PersistedValue<T>::value() { return value_; }

template < class T>
class MODELBASE_API PersistedValue < QList<T*> >: public PersistedNode
{
	public:
		~PersistedValue();
		QList<T*>& value();

	private:
		QList<T*> value_;
};

template <class T> PersistedValue< QList<T*> >::~PersistedValue()
{
	for( int i = 0; i< value_.size(); ++i) SAFE_DELETE(value_[i]);
	value_.clear();
}

template <class T> inline QList<T*>& PersistedValue< QList<T*> >::value() { return value_; }

}

#endif /* PERSISTEDSTRING_H_ */
