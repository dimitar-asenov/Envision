/***********************************************************************************************************************
 * StyleNode.cpp
 *
 *  Created on: Dec 20, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "StyleNode.h"

#include "VisualizationException.h"

#include "Core/headers/global.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

namespace Visualization {

const QString XML_DOM_TYPE = "EnvisionVisualizationStyle";

QString StyleNode::baseFolder;

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
	parent(NULL)
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
	if (e.isNull()) throw VisualizationException("Could not find the style property '"+ path.last() + "'.");
	return e.firstChild().nodeValue();
}

}
