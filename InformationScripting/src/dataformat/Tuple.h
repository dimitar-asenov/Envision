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

class INFORMATIONSCRIPTING_API Tuple
{
	public:
		Tuple() = delete;
		Tuple(const QString& tag, std::initializer_list<NamedProperty> initialValues);
		Tuple(const QString& tag, QList<NamedProperty> initialValues);
		Tuple(std::initializer_list<NamedProperty> initialValues);
		Tuple(QList<NamedProperty> initialValues);

		QString tag() const;

		void add(const NamedProperty& p);

		int size() const;
		bool contains(const QString& name) const;

		uint hashValue(uint seed = 0) const;

		bool operator==(const Tuple& other) const;

		Property& operator[](const QString& name);
		const Property& operator[](const QString& name) const;
		NamedProperty& operator[](int index);
		const NamedProperty& operator[](int index) const;

		template<typename T>
		QList<T> valuesOfType() const;

		// Iterators
		using iterator = QList<NamedProperty>::Iterator;
		using const_iterator = QList<NamedProperty>::ConstIterator;

		const_iterator find(const QString& name) const;
		iterator find(const QString& name);

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;
		iterator end();
		const_iterator end() const;
		const_iterator cend() const;
	private:
		QList<NamedProperty> values_;
		QString tag_;

};

uint qHash(const Tuple& t, uint seed = 0);

inline QString Tuple::tag() const { return tag_; }
inline int Tuple::size() const { return values_.size(); }
inline bool Tuple::contains(const QString& name) const { return find(name) != end(); }
inline bool Tuple::operator==(const Tuple& other) const { return values_ == other.values_; }

inline NamedProperty& Tuple::operator[](int index) { return values_[index]; }
inline const NamedProperty&Tuple::operator[](int index) const { return values_[index]; }

template<typename T>
inline QList<T> Tuple::valuesOfType() const
{
	QList<T> result;
	for (const auto& np : values_)
		if (np.second.isConvertibleTo<T>())
			result.push_back(np.second);
	return result;
}

inline Tuple::iterator Tuple::begin() { return values_.begin(); }
inline Tuple::const_iterator Tuple::begin() const { return values_.begin(); }
inline Tuple::const_iterator Tuple::cbegin() const { return values_.cbegin(); }
inline Tuple::iterator Tuple::end() { return values_.end(); }
inline Tuple::const_iterator Tuple::end() const { return values_.end(); }
inline Tuple::const_iterator Tuple::cend() const { return values_.cend(); }

} /* namespace InformationScripting */
