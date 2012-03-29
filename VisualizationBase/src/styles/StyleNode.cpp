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
 * StyleNode.cpp
 *
 *  Created on: Dec 20, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "styles/StyleNode.h"

#include "VisualizationException.h"

namespace Visualization {

const QString XML_DOM_TYPE = "EnvisionVisualizationStyle";

QString StyleNode::baseFolder = "styles";

void StyleNode::setBaseFolder(const QString &baseFolder_)
{
	baseFolder = baseFolder_;
}

StyleNode::StyleNode(StyleNode* parent_, QDomElement elem_) :
	parent(parent_), elem(elem_)
{
	init();
}

StyleNode::StyleNode(StyleNode* parent_, const QString& prototypeName, const QString& currentFolder) :
	parent(parent_)
{
	folder = currentFolder + "/" + prototypeName; // Try relative address
	if (!QFile::exists(folder)) folder = baseFolder + "/" + prototypeName; // Try absolute path (rooted at the styles folder)
	if (!QFile::exists(folder)) throw VisualizationException("Could not find the prototype style " + prototypeName);

	QString fileName = QFileInfo(folder).fileName();
	folder = QFileInfo(folder).path();
	doc = openStyleDoc(folder + "/" + fileName);
	elem = doc.documentElement();

	init();
}

StyleNode::StyleNode(const QString& rootStyleFileName, const QString& folderWithinStyles) :
	parent(nullptr)
{
	folder = QDir::current().absoluteFilePath(baseFolder + "/" + folderWithinStyles);
	doc = openStyleDoc(folder + "/" + rootStyleFileName);
	elem = doc.documentElement();
	init();
}

void StyleNode::init()
{
	// Initialize children
	QDomElement e = elem.firstChildElement();
	while ( !e.isNull() )
	{
		children.append(new StyleNode(this, e));
		e = e.nextSiblingElement();
	}

	// Initialize prototypes
	if ( elem.hasAttribute("prototypes") )
	{
		QStringList proto = elem.attribute("prototypes", QString()).split(',');
		for (int i = 0; i < proto.size(); ++i)
		{
			prototypes.append(new StyleNode(this, proto[i], getFolder()));
		}
	}
}

StyleNode::~StyleNode()
{
	for (int i = 0; i < prototypes.size(); ++i)
		SAFE_DELETE(prototypes[i]);
	prototypes.clear();

	for (int i = 0; i < children.size(); ++i)
		SAFE_DELETE(children[i]);
	children.clear();
}

QDomDocument StyleNode::openStyleDoc(const QString& path)
{
	QDomDocument doc = QDomDocument(XML_DOM_TYPE);
	QFile file(path);
	if ( !file.open(QIODevice::ReadOnly) ) throw VisualizationException("Could not open style file " + file.fileName() + ".");

	if ( !doc.setContent(&file) || doc.isNull() )
	{
		file.close();
		throw VisualizationException("Reading of the XML structure of file " + file.fileName() + " failed.");
	}

	file.close();
	return doc;
}

QString StyleNode::getFolder()
{
	if ( folder.isNull() && parent ) return parent->getFolder();
	else return folder;
}

QDomElement StyleNode::getElement(QStringList path)
{
	if (path.size() < 1 || path.first() != elem.tagName() ) return QDomElement();

	if ( path.size() == 1) return elem;

	// Try to find this element among the children
	QDomElement e;
	int index = 0;
	while ( e.isNull() && index < children.size() )
	{
		e = children[index]->getElement(path.mid(1));
		index++;
	}
	if (!e.isNull() ) return e;

	// Try to find this element among the prototypes
	index = 0;
	while ( e.isNull() && index < prototypes.size() )
	{
		QStringList newPath = path.mid(1);
		newPath.prepend("style");
		e = prototypes[index]->getElement(newPath);
		index++;
	}

	return e;
}

QString StyleNode::getProperty(QStringList path)
{
	QDomElement e = getElement(path);
	if (e.isNull())
		throw VisualizationException("Could not find the style property '"+ path.join("/") + "'.");
	return e.firstChild().nodeValue();
}

}
