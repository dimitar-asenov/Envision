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

#include "Tuple.h"

namespace InformationScripting {

Tuple::Tuple(std::initializer_list<NamedProperty> initialValues)
	: values_{initialValues}
{}

QString Tuple::tag() const
{
	if (values_.size()) return values_[0].first;
	return {};
}

NamedProperty Tuple::get(int index) const
{
	return values_.at(index);
}

QList<NamedProperty> Tuple::getAll(const QString& key)
{
	QList<NamedProperty> result;
	for (auto p : values_)
		if (p.first == key) result << p;
	return result;
}

void Tuple::add(NamedProperty p)
{
	values_.append(p);
}

uint Tuple::hashValue(uint seed) const
{
	return qHashRange(values_.begin(), values_.end(), seed);
}

Tuple::const_iterator Tuple::find(const QString& key) const
{
	return std::find_if(values_.begin(), values_.end(), [key](auto v) {return v.first == key;});
}

uint qHash(const Tuple& t, uint seed)
{
	return t.hashValue(seed);
}

} /* namespace InformationScripting */
