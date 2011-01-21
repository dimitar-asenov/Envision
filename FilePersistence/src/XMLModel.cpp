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

void XMLModel::beginSaveNode(const QString &tag)
{
	elem = doc.createElement(tag);
	elemStack.append(elem);
}

void XMLModel::endSaveNode()
{
	if (!elemStack.isEmpty())
	{
		elemStack.removeLast();
		if (elemStack.isEmpty()) elem = QDomElement();
		else elem = elemStack.last();
	}
	else throw FilePersistenceException("Invalid call to endSaveNode - element stack is empty.");
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

int XMLModel::loadIntValue()
{
	bool ok = true;

	int res = elem.firstChild().nodeValue().mid(PREFIX_INTEGER.size()).toInt(&ok);
	if ( !ok ) throw FilePersistenceException("Could read integer value " + elem.firstChild().nodeValue());

	return res;
}

QString XMLModel::loadStringValue()
{
	return elem.firstChild().nodeValue().mid(PREFIX_STRING.size());
}

double XMLModel::loadFloatValue()
{
	bool ok = true;

	double res = elem.firstChild().nodeValue().mid(PREFIX_DOUBLE.size()).toDouble(&ok);
	if ( !ok ) throw FilePersistenceException("Could read real value " + elem.firstChild().nodeValue());

	return res;
}

bool XMLModel::hasChildNode(const QString& nodeName)
{
	return !elem.firstChildElement(nodeName).isNull();
}

void XMLModel::beginLoadChildNode(const QString& nodeName)
{
	elem = elem.firstChildElement(nodeName);
	if (elem.isNull()) throw FilePersistenceException("No child with the name '" + nodeName + "' exists.");
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

bool XMLModel::hasNext()
{
	return !elem.nextSiblingElement().isNull();
}

void XMLModel::next()
{
	elem = elem.nextSiblingElement();
	if (elem.isNull()) throw FilePersistenceException("Element does not have a next sibling");
	elemStack.removeLast();
	elemStack.append(elem);
}

QString XMLModel::getType()
{
	return elem.tagName();
}

Model::NodeIdType XMLModel::getId()
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

QString XMLModel::getName()
{
	if ( elem.hasAttribute("name") ) return elem.attribute("name");
	else throw FilePersistenceException("Name not found for node of type: " + elem.tagName());
}

bool XMLModel::getPartialHint()
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

QString XMLModel::documentText()
{
	return doc.toString();
}

void XMLModel::setDocument(QFile file)
{
	if ( !file.open(QIODevice::ReadOnly) ) throw FilePersistenceException("Could not open file " + file.fileName() + ".");

	if ( !doc.setContent(&file) )
	{
		file.close();
		throw FilePersistenceException("Reading of the XML structure of file " + file.fileName() + " failed.");
	}
	file.close();
}

void XMLModel::setDocument(const QString& text)
{
	if ( !doc.setContent(text) )
		throw FilePersistenceException("Reading of the XML structure of the specified text failed in XMLModel::setDocument");
}

}
