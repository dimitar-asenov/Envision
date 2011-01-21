/***********************************************************************************************************************
 * XMLModel.cpp
 *
 *  Created on: Jan 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "XMLModel.h"
#include "FilePersistenceException.h"

namespace FilePersistence {

static const char* XML_DOM_TYPE = "EnvisionFilePersistence";

static const int MAX_DOUBLE_PRECISION = 15;

static const int ATTRIBUTE_TRUE = 1;
static const int ATTRIBUTE_FALSE = 0;

static const QString PREFIX_STRING = QString("S_");
static const QString PREFIX_INTEGER = QString("I_");
static const QString PREFIX_DOUBLE = QString("D_");

XMLModel::XMLModel() :
	doc(XML_DOM_TYPE)
{
}

XMLModel::XMLModel(const QString& filename) :
	doc(XML_DOM_TYPE)
{
	QFile file(filename);
	if ( !file.open(QIODevice::ReadOnly) ) throw FilePersistenceException("Could not open file " + file.fileName() + ".");

	QString error;
	int line, col;
	if ( !doc.setContent(&file, &error, &line, &col) )
	{
		file.close();
		throw FilePersistenceException("Reading of the XML structure of file " + file.fileName() + " failed." + error + " line: " + line + " col: " + col);
	}
	file.close();
}

XMLModel::~XMLModel()
{
}

void XMLModel::saveStringValue(const QString &value)
{
	QDomText text = doc.createTextNode(PREFIX_STRING + value);
	elem.appendChild(text);
}

void XMLModel::saveIntValue(int value)
{
	QDomText text = doc.createTextNode(PREFIX_INTEGER + QString::number(value));
	elem.appendChild(text);
}

void XMLModel::saveFloatValue(double value)
{
	QDomText text = doc.createTextNode(PREFIX_INTEGER + QString::number(value, 'f', MAX_DOUBLE_PRECISION));
	elem.appendChild(text);
}

void XMLModel::beginSaveChildNode(const QString &tag)
{
	QDomElement child = doc.createElement(tag);
	if (elem.isNull()) doc.appendChild(child);
	else elem.appendChild(child);
	elem = child;
	elemStack.append(elem);
}

void XMLModel::endSaveChildNode()
{
	if (!elemStack.isEmpty())
	{
		elemStack.removeLast();
		if (elemStack.isEmpty()) elem = QDomElement();
		else elem = elemStack.last();
	}
	else throw FilePersistenceException("Invalid call to endSaveNode - element stack is empty.");
}

void XMLModel::importChildFromXML(QDomElement child)
{
	elem.appendChild(doc.importNode(child, true));
}

void XMLModel::saveNext(const QString& tag)
{
	elem = doc.createElement(tag);
	elemStack.removeLast();
	elemStack.append(elem);
}

void XMLModel::setId(Model::NodeIdType id)
{
	elem.setAttribute("id", QString::number(id));
}

void XMLModel::setName(const QString& name)
{
	elem.setAttribute("name", name);
}

void XMLModel::setPartialHint(bool partialHint)
{
	elem.setAttribute("partial", partialHint ? ATTRIBUTE_TRUE : ATTRIBUTE_FALSE);
}

int XMLModel::loadIntValue() const
{
	bool ok = true;

	int res = elem.firstChild().nodeValue().mid(PREFIX_INTEGER.size()).toInt(&ok);
	if ( !ok ) throw FilePersistenceException("Could read integer value " + elem.firstChild().nodeValue());

	return res;
}

QString XMLModel::loadStringValue() const
{
	return elem.firstChild().nodeValue().mid(PREFIX_STRING.size());
}

double XMLModel::loadFloatValue() const
{
	bool ok = true;

	double res = elem.firstChild().nodeValue().mid(PREFIX_DOUBLE.size()).toDouble(&ok);
	if ( !ok ) throw FilePersistenceException("Could read real value " + elem.firstChild().nodeValue());

	return res;
}

bool XMLModel::hasChildren() const
{
	return !elem.firstChildElement().isNull();
}

bool XMLModel::hasChild(const QString& nodeName) const
{
	QDomElement child = elem.firstChildElement();
	while (!child.isNull())
	{
		if (child.attribute("name") == nodeName) return true;
		child = child.nextSiblingElement();
	}
	return false;
}

void XMLModel::beginLoadChildNode(const QString& nodeName)
{
	QDomElement child;
	if (elem.isNull()) child = doc.firstChildElement();
	else child = elem.firstChildElement();

	while(!child.isNull())
	{
		if (child.attribute("name") == nodeName) break;
		child = child.nextSiblingElement();
	}

	if (child.isNull()) throw FilePersistenceException("No child with the name '" + nodeName + "' exists.");

	elem = child;
	elemStack.append(elem);
}

void XMLModel::endLoadChildNode()
{
	if (!elemStack.isEmpty())
	{
		elemStack.removeLast();
		if (elemStack.isEmpty()) elem = QDomElement();
		else elem = elemStack.last();
	}
	else throw FilePersistenceException("Invalid call to endLoadChildNode - element stack is empty.");
}

QStringList XMLModel::getChildrenNames() const
{
	QStringList names;

	QDomElement child = elem.firstChildElement();
	while ( !child.isNull() )
	{
		QString name = child.attribute("name", QString());
		if (!name.isEmpty()) names.append(name);

		child = child.nextSiblingElement();
	}

	return names;
}

bool XMLModel::hasNext() const
{
	return !elem.nextSiblingElement().isNull();
}

void XMLModel::loadNext()
{
	elem = elem.nextSiblingElement();
	if (elem.isNull()) throw FilePersistenceException("Element does not have a next sibling");
	elemStack.removeLast();
	elemStack.append(elem);
}

bool XMLModel::goToElement(Model::NodeIdType id, bool startFromRoot)
{
	if (startFromRoot)
	{
		elemStack.clear();
		elem = doc.firstChildElement();
		elemStack.append(elem);
	}

	if (elem.isNull()) return false;

	if ( elem.hasAttribute("id") && getId() == id) return true;

	QDomElement child = elem.firstChildElement();
	while ( !child.isNull() )
	{
		elem = child;
		elemStack.append(elem);

		if ( goToElement(id, false) ) return true;

		elemStack.removeLast();
		elem = elemStack.last();
		child = child.nextSiblingElement();
	}

	return false;
}

void XMLModel::goToFirstChild()
{
	if (elem.isNull()) elem = doc.firstChildElement();
	else
	{
		if (elem.firstChildElement().isNull()) throw FilePersistenceException("Element does not have child nodes");
		elem = elem.firstChildElement();
	}
	elemStack.append(elem);
}

void XMLModel::goToParent()
{
	if (elemStack.isEmpty()) throw FilePersistenceException("Can not go to parent in XMLModel::goToParent");
	elemStack.removeLast();
	elem = elemStack.last();
}

QString XMLModel::getType() const
{
	return elem.tagName();
}

Model::NodeIdType XMLModel::getId() const
{
	if ( elem.hasAttribute("id") )
	{
		bool ok = true;
		Model::NodeIdType id = elem.attribute("id", "error").toInt(&ok);
		if (ok) return id;
		else throw FilePersistenceException("Incorrect id '" + elem.attribute("id") + "' for node of type: " + elem.tagName());
	}
	else throw FilePersistenceException("Id not found for node of type: " + elem.tagName());
}

QString XMLModel::getName() const
{
	if ( elem.hasAttribute("name") ) return elem.attribute("name");
	else throw FilePersistenceException("Name not found for node of type: " + elem.tagName());
}

bool XMLModel::getPartialHint() const
{
	if ( elem.hasAttribute("partial") )
	{
		bool ok = true;
		int partial = elem.attribute("partial", "error").toInt(&ok);
		if (!ok || (partial != ATTRIBUTE_TRUE && partial != ATTRIBUTE_FALSE ) )
			throw FilePersistenceException("Incorrect partial hint '" + elem.attribute("partial") + "' for node of type: " + elem.tagName());
		else return partial == ATTRIBUTE_TRUE;
	}
	else throw FilePersistenceException("Partial hint not found for node of type: " + elem.tagName());
}

QDomElement XMLModel::getCurrentElement() const
{
	return elem;
}

bool XMLModel::isString() const
{
	return elem.firstChild().nodeValue().startsWith(PREFIX_STRING);
}

bool XMLModel::isInteger() const
{
	return elem.firstChild().nodeValue().startsWith(PREFIX_INTEGER);
}

bool XMLModel::isDouble() const
{
	return elem.firstChild().nodeValue().startsWith(PREFIX_DOUBLE);
}

QString XMLModel::documentText() const
{
	return doc.toString();
}

void XMLModel::setDocumentText(const QString& text)
{
	if ( !doc.setContent(text) )
		throw FilePersistenceException("Reading of the XML structure of the specified text failed in XMLModel::setDocument");

	elemStack.clear();
}

}
