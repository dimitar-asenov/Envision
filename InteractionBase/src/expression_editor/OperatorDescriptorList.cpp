/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "OperatorDescriptorList.h"

#include "OperatorDescriptor.h"

namespace Interaction {

OperatorDescriptorList::~OperatorDescriptorList()
{
	for (OperatorDescriptor* d : ops_)
			SAFE_DELETE(d);
}

OperatorDescriptor* OperatorDescriptorList::findByName(QString name) const
{
	for (OperatorDescriptor* d : ops_)
		if (d->name() == name) return d;

	return nullptr;
}

inline bool OperatorDescriptorList::listStartsWith(const QStringList& longList, const QStringList& prefixToCheck)
{
	if (longList.isEmpty() || prefixToCheck.isEmpty() || longList.size() < prefixToCheck.size())
		return false;

	for (int i = 0; i<prefixToCheck.size(); ++i)
	{
		if (longList.at(i) != prefixToCheck.at(i)) return false;
	}

	return true;
}

QList<OperatorDescriptor*> OperatorDescriptorList::findByPrefix(const QStringList&  prefixTokens) const
{
	QList<OperatorDescriptor*> list;
	for (OperatorDescriptor* d : ops_)
		if ( listStartsWith(prefixTokens, d->prefix()) )
			list.append(d);

	return list;
}

QList<OperatorDescriptor*> OperatorDescriptorList::findByInfixWithoutPrefix(const QStringList&  infixTokens) const
{
	QList<OperatorDescriptor*> list;
	for (OperatorDescriptor* d : ops_)
		if (d->prefix().isEmpty() && !d->infixes().isEmpty() && listStartsWith(infixTokens, d->infixes().first()))
			list.append(d);

	return list;
}

QList<OperatorDescriptor*> OperatorDescriptorList::findByEmptyInfixWithoutPrefix() const
{
	QList<OperatorDescriptor*> list;
	for (OperatorDescriptor* d : ops_)
		if (d->prefix().isEmpty() && !d->infixes().isEmpty() && d->infixes().first().isEmpty())
			list.append(d);

	return list;
}

QList<OperatorDescriptor*> OperatorDescriptorList::findByPostfixWithoutPreInfix(const QStringList&  postfixTokens) const
{
	QList<OperatorDescriptor*> list;
	for (OperatorDescriptor* d : ops_)
		if (d->prefix().isEmpty() && d->infixes().isEmpty() &&  listStartsWith(postfixTokens, d->postfix()))
			list.append(d);

	return list;
}

}
