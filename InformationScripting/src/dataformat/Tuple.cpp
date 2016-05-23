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

Tuple::Tuple(const QString& tag, std::initializer_list<NamedProperty> initialValues)
	: values_{initialValues}, tag_{tag}
{}

Tuple::Tuple(const QString& tag, QList<NamedProperty> initialValues)
	: values_{initialValues}, tag_{tag}
{}

Tuple::Tuple(std::initializer_list<NamedProperty> initialValues)
	: values_{initialValues}
{
	Q_ASSERT(values_.size() > 0);
	tag_ = values_[0].first;
}

Tuple::Tuple(QList<NamedProperty> initialValues)
	: values_{initialValues}
{
	Q_ASSERT(!initialValues.isEmpty());
	tag_ = initialValues[0].first;
}

void Tuple::add(const NamedProperty& p)
{
	auto it = std::find_if(begin(), end(), [p](const auto& np) {return np.first == p.first;});
	Q_ASSERT(it == values_.end()); // TODO what should we do here?
	values_.append(p);
}

uint Tuple::hashValue(uint seed) const
{
	return qHashRange(begin(), end(), seed);
}

Tuple::const_iterator Tuple::find(const QString& name) const
{
	return std::find_if(begin(), end(), [name](const auto& np) { return np.first == name; });
}

Tuple::iterator Tuple::find(const QString& name)
{
	return std::find_if(begin(), end(), [name](const auto& np) { return np.first == name; });
}

uint qHash(const Tuple& t, uint seed)
{
	return t.hashValue(seed);
}

Property& Tuple::operator[](const QString& name)
{
	auto it = find(name);
	Q_ASSERT(it != end());
	return it->second;
}

const Property& Tuple::operator[](const QString& name) const
{
	auto it = find(name);
	Q_ASSERT(it != end());
	return it->second;
}

}
