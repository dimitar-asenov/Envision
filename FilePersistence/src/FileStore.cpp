/***********************************************************************************************************************
 * FileStore.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "FileStore.h"
#include "FilePersistenceException.h"
#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/nodes/Node.h"
#include "ModelBase/headers/ModelException.h"

#include <QtCore/QDebug>

namespace FilePersistence {

const char* XML_DOM_TYPE = "EnvisionFilePersistence";
const char* XML_NEWUNIT_NODE_TAG = "persistencenewunit";

const int MAX_DOUBLE_PRECISION = 15;

const int ATTRIBUTE_TRUE = 1;
const int ATTRIBUTE_FALSE = 0;

FileStore::FileStore() : // TODO the Envision folder should be taken from the environment not hardcoded.
	baseFolder(QDir::home().path() + QDir::toNativeSeparators("/Envision/projects")), working(false), currentDoc(NULL), currentParent(NULL)
{
}

FileStore::~FileStore()
{
}

void FileStore::setBaseFolder(const QString& path)
{
	baseFolder = path;
}

//**********************************************************************************************************************
// Methods from Persistent Store
//**********************************************************************************************************************

void FileStore::saveModel(Model::Model& model, const QString &name)
{
	storeAccess.lock();
	working = true;
	model.beginExclusiveRead();

	try
	{
		if ( !baseFolder.exists(name) )
		{
			if ( !baseFolder.mkpath(name) ) throw FilePersistenceException("Could not create folder " + baseFolder.path() + " for model.");
		}

		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + name);

		if ( !modelDir.exists() ) throw FilePersistenceException("Error opening model folder " + modelDir.path());

		saveNewPersistenceUnit(model.getRoot(), name, false);
	}
	catch (Model::ModelException& e)
	{
		model.endExclusiveRead();
		working = false;
		storeAccess.unlock();
		throw;
	}
	model.endExclusiveRead();
	working = false;
	storeAccess.unlock();
}

void FileStore::saveStringValue(const QString &value)
{
	checkIsWorking();

	QDomText text = currentDoc->createTextNode(value);
	currentParent->appendChild(text);
}

void FileStore::saveIntValue(int value)
{
	checkIsWorking();

	QDomText text = currentDoc->createTextNode(QString::number(value));
	currentParent->appendChild(text);
}

void FileStore::saveFloatValue(double value)
{
	checkIsWorking();

	QDomText text = currentDoc->createTextNode(QString::number(value, 'f', MAX_DOUBLE_PRECISION));
	currentParent->appendChild(text);
}

void FileStore::saveNewPersistenceUnit(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	checkIsWorking();

	QDomDocument* oldDoc = currentDoc;
	QDomElement* oldParent = currentParent;

	if ( currentDoc != NULL ) // If this is not the root node, then we should put a reference to this node
	{
		QDomElement container = currentDoc->createElement(XML_NEWUNIT_NODE_TAG);
		QDomText text = currentDoc->createTextNode(QString::number(node->getId()));
		container.appendChild(text);
		currentParent->appendChild(container);
	}

	// TODO make sure that partially loaded nodes are correctly loaded before we rewrite the info.
	// At this point we still have not overwritten the old file, so maybe we can expand the nodes as we meet them.
	QDomDocument doc(XML_DOM_TYPE);
	currentDoc = &doc;

	QDomElement rootElem = currentDoc->createElement(node->getTypeName());
	rootElem.setAttribute("name", name);
	rootElem.setAttribute("id", QString::number(node->getId()));
	rootElem.setAttribute("partial", partialLoadHint ? ATTRIBUTE_TRUE : ATTRIBUTE_FALSE);

	currentParent = &rootElem;
	node->save(*this);
	currentDoc->appendChild(rootElem);

	QString filename;
	if ( oldDoc == NULL ) filename = name; // This is the root of the model, save the file name
	else
		filename = QString::number(node->getId()); // This is not the root, so save by id

	QFile file(modelDir.absoluteFilePath(filename));
	if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) ) throw FilePersistenceException("Could not open file " + file.fileName());

	QTextStream ts(&file);
	ts << doc.toString();
	file.close();

	currentParent = oldParent;
	currentDoc = oldDoc;
}

void FileStore::saveNode(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	checkIsWorking();

	if ( node->isNewPersistenceUnit() )
	{
		saveNewPersistenceUnit(node, name, partialLoadHint);
		return;
	}

	// TODO make sure that partially loaded nodes are correctly loaded before we rewrite the info.
	// At this point we still have not overwritten the old file, so maybe we can expand the nodes as we meet them.

	QDomElement* oldParent = currentParent;

	QDomElement elem = currentDoc->createElement(node->getTypeName());
	elem.setAttribute("name", name);
	elem.setAttribute("id", QString::number(node->getId()));
	elem.setAttribute("partial", partialLoadHint ? ATTRIBUTE_TRUE : ATTRIBUTE_FALSE);

	currentParent = &elem;
	node->save(*this);
	currentParent = oldParent;

	currentParent->appendChild(elem);
}

Model::Node* FileStore::loadRootNode(const QString &name)
{
	storeAccess.lock();
	working = true;
	Model::LoadedNode ln;

	try
	{
		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + name);
		if ( !modelDir.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir.path());

		ln = loadNewPersistenceUnit(name, NULL);
	}
	catch (Model::ModelException& e)
	{
		working = false;
		storeAccess.unlock();
		throw;
	}

	working = false;
	storeAccess.unlock();

	return ln.node;
}

Model::LoadedNode FileStore::loadNewPersistenceUnit(const QString& name, Model::Node* parent)
{
	QDomDocument doc(XML_DOM_TYPE);
	QFile file(modelDir.absoluteFilePath(name));
	if ( !file.open(QIODevice::ReadOnly) ) throw FilePersistenceException("Could not open file " + file.fileName() + ".");

	if ( !doc.setContent(&file) )
	{
		file.close();
		throw FilePersistenceException("Reading of the XML structure of file " + file.fileName() + " failed.");
	}
	file.close();

	QDomElement elem = doc.firstChildElement();
	return loadNode(elem, parent);
}

QList<Model::LoadedNode> FileStore::loadAllSubNodes(Model::Node* parent)
{
	checkIsWorking();

	QList < Model::LoadedNode > result;

	QDomElement elem = currentParent->firstChildElement();
	while ( !elem.isNull() )
	{
		Model::LoadedNode ln;
		if ( elem.tagName() == XML_NEWUNIT_NODE_TAG ) ln = loadNewPersistenceUnit(elem.firstChild().nodeValue(), parent);
		else
			ln = loadNode(elem, parent);

		result.append(ln);

		elem = elem.nextSiblingElement();
	}

	return result;
}

Model::LoadedNode FileStore::loadNode(QDomElement& nodeElement, Model::Node* parent)
{
	QDomElement* oldParent = currentParent;
	currentParent = &nodeElement;

	QString nodeType = nodeElement.tagName();

	bool ok = true;
	Model::NodeIdType id = nodeElement.attribute("id", "error").toLongLong(&ok);
	if ( !ok ) throw FilePersistenceException("Could not read node id " + nodeElement.attribute("id"));

	int partial = nodeElement.attribute("partial", "error").toInt(&ok);
	if ( !ok ) throw FilePersistenceException("Could not read node partial hint " + nodeElement.attribute("partial"));
	if ( partial != ATTRIBUTE_TRUE && partial != ATTRIBUTE_FALSE ) throw FilePersistenceException("Invalid partial hint " + nodeElement.attribute("partial"));

	Model::LoadedNode node;
	node.name = nodeElement.attribute("name");
	node.node = Model::Node::createNewNode(nodeType, parent, id, *this, partial == ATTRIBUTE_TRUE);

	currentParent = oldParent;

	return node;
}

int FileStore::loadIntValue()
{
	checkIsWorking();

	bool ok = true;

	int res = currentParent->firstChild().nodeValue().toInt(&ok);
	if ( !ok ) throw FilePersistenceException("Could read integer value " + currentParent->firstChild().nodeValue());

	return res;
}

QString FileStore::loadStringValue()
{
	checkIsWorking();

	return currentParent->firstChild().nodeValue();
}

double FileStore::loadFloatValue()
{
	checkIsWorking();

	bool ok = true;

	double res = currentParent->firstChild().nodeValue().toDouble(&ok);
	if ( !ok ) throw FilePersistenceException("Could read real value " + currentParent->firstChild().nodeValue());

	return res;
}

void FileStore::checkIsWorking()
{
	if ( !working ) throw FilePersistenceException("Performing an illegal file persistence operation.");
}
}
