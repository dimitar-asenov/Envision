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

#include "PropertyMap.h"

namespace InformationScripting {

PropertyMap::PropertyMap(QList<QPair<QString, Property> > initialValues)
{
	for (auto initialValue : initialValues)
		insert(initialValue.first, initialValue.second);
}

boost::python::object PropertyMap::pythonAttribute(const QString& key) const
{
	return pythonObject(operator[](key));
}

Property PropertyMap::operator[](const QString& key) const
{
	for (auto content : properties_)
		if (content.first == key) return content.second;
	qDebug() << "No object with name" << key;
	Q_ASSERT(false);
}

Property& InformationScripting::PropertyMap::operator[](const QString& key)
{
	for (auto& content : properties_)
		if (content.first == key) return content.second;
	qDebug() << "No object with name" << key;
	Q_ASSERT(false);
}

bool PropertyMap::contains(const QString& key) const
{
	for (auto content : properties_)
		if (content.first == key) return true;
	return false;
}

} /* namespace InformationScripting */
