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
		Tuple() = default;
		Tuple(std::initializer_list<NamedProperty> initialValues);

		QString tag() const;

		NamedProperty get(int index) const;
		QList<NamedProperty> getAll(const QString& key);

		void add(NamedProperty p);

		int size() const;

		uint hashValue(uint seed = 0) const;

		bool operator==(const Tuple& other) const;

		// Iterators
		using iterator = QList<NamedProperty>::Iterator;
		using const_iterator = QList<NamedProperty>::ConstIterator;

		const_iterator find(const QString& key) const;

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;
		iterator end();
		const_iterator end() const;
		const_iterator cend() const;
	private:
		QList<NamedProperty> values_;

};

uint qHash(const Tuple& t, uint seed = 0);

inline bool Tuple::operator==(const Tuple& other) const { return values_ == other.values_; }
inline int Tuple::size() const { return values_.size(); }

inline Tuple::iterator Tuple::begin() { return values_.begin(); }
inline Tuple::const_iterator Tuple::begin() const { return values_.begin(); }
inline Tuple::const_iterator Tuple::cbegin() const { return values_.cbegin(); }
inline Tuple::iterator Tuple::end() { return values_.end(); }
inline Tuple::const_iterator Tuple::end() const { return values_.end(); }
inline Tuple::const_iterator Tuple::cend() const { return values_.cend(); }

} /* namespace InformationScripting */
