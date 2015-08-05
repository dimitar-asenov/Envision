/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#pragma once

#include "../informationscripting_api.h"

#include "Property.h"

namespace InformationScripting {

class INFORMATIONSCRIPTING_API PropertyMap
{
	public:
		PropertyMap(QList<QPair<QString, Property>> initialValues);
		template <class DataType>
		void insert(const QString& key, const DataType& value);

		boost::python::object pythonAttribute(const QString& key);
		Property& operator[](const QString& key);

		bool contains(const QString& key) const;

		// Iterators
		using iterator = QList<QPair<QString, Property>>::Iterator;
		using const_iterator = QList<QPair<QString, Property>>::ConstIterator;

		const_iterator find(const QString& key) const;

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;
		iterator end();
		const_iterator end() const;
		const_iterator cend() const;

	private:
		QList<QPair<QString, Property>> properties_{};
};


template <class DataType>
inline void PropertyMap::insert(const QString& key, const DataType& value)
{
	properties_.push_back({key, Property(value)});
}

inline PropertyMap::iterator PropertyMap::begin() { return properties_.begin(); }
inline PropertyMap::const_iterator PropertyMap::begin() const { return properties_.begin(); }
inline PropertyMap::const_iterator PropertyMap::cbegin() const { return properties_.cbegin(); }
inline PropertyMap::iterator PropertyMap::end() { return properties_.end(); }
inline PropertyMap::const_iterator PropertyMap::end() const { return properties_.end(); }
inline PropertyMap::const_iterator PropertyMap::cend() const { return properties_.cend(); }

} /* namespace InformationScripting */
