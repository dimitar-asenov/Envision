/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

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

namespace Model {

template <class T>
class PersistedValue: public PersistedNode
{
	public:
		PersistedValue();
		virtual ~PersistedValue();
		void set(const T& value);
		T& value();

	private:
		T value_;
};

template <class T> PersistedValue<T>::PersistedValue(){}
template <class T> PersistedValue<T>::~PersistedValue(){}
template <class T> inline void PersistedValue<T>::set(const T& value) { value_ = value; }
template <class T> inline T& PersistedValue<T>::value() { return value_; }

template < class T>
class PersistedValue < QList<T*> >: public PersistedNode
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
