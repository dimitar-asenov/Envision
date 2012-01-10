/***********************************************************************************************************************
**
** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * OperatorDescriptorList.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: Dimitar Asenov
 */

#include "expression_editor/OperatorDescriptorList.h"

#include "expression_editor/OperatorDescriptor.h"

namespace InteractionBase {

OperatorDescriptor* OperatorDescriptorList::findByName(QString name)
{
	for (OperatorDescriptor* d : ops_)
		if (d->name() == name) return d;

	return nullptr;
}

QList<OperatorDescriptor*> OperatorDescriptorList::findByPrefix(const QString&  prefix)
{
	QList<OperatorDescriptor*> list;
	for (OperatorDescriptor* d : ops_)
		if ( !d->prefix().isEmpty() && d->signature().first() == prefix)
			list.append(d);

	return list;
}

QList<OperatorDescriptor*> OperatorDescriptorList::findByInfixWithoutPrefix(const QString&  infix)
{
	QList<OperatorDescriptor*> list;
	for (OperatorDescriptor* d : ops_)
		if (d->prefix().isEmpty() && !d->infixes().isEmpty() && d->infixes().first() == infix)
			list.append(d);

	return list;
}

QList<OperatorDescriptor*> OperatorDescriptorList::findByPostfixWithoutPreInfix(const QString&  postfix)
{
	QList<OperatorDescriptor*> list;
	for (OperatorDescriptor* d : ops_)
		if (d->prefix().isEmpty() && d->infixes().isEmpty() && d->signature().last() == postfix)
			list.append(d);

	return list;
}

} /* namespace InteractionBase */
