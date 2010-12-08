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

QString FileStore::getPersistenceUnitName(const Model::Node *node, int* atDepth)
{
	// Find the parent which is a persistent unit, if any
	// Record the depth of the current node relative to the persistent unit root.
	int depth = 0;
	const Model::Node* persistentUnitNode = node;
	while ( persistentUnitNode && persistentUnitNode->isNewPersistenceUnit() == false )
	{
		persistentUnitNode = persistentUnitNode->getParent();
		++depth;
	}

	// Find the name of the unit. This is either the id or the model name
	QString name;
	if ( persistentUnitNode ) name = QString::number(persistentUnitNode->getId());
	else
		name = node->getModel()->getName();

	if ( atDepth ) *atDepth = depth;

	return name;
}

QStringList FileStore::getChildrenNames(const QDomElement* elem) const
{
	QStringList names;

	QDomElement child = elem->firstChildElement();
	while ( !child.isNull() )
	{
		QString name = child.attribute("name", QString());
		if (!name.isEmpty()) names.append(name);

		child = child.nextSiblingElement();
	}

	return names;
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
		container.setAttribute("name", name);
		QDomText text = currentDoc->createTextNode(QString::number(node->getId()));
		container.appendChild(text);
		currentParent->appendChild(container);
	}

	QDomDocument doc(XML_DOM_TYPE);
	currentDoc = &doc;

	// We need a fake element since currentParent is of type QDomElement and QDomDocument is not
	QDomElement fakeElem = currentDoc->createElement(node->getTypeName());
	currentParent = &fakeElem;

	saveNodeDirectly(node, name, partialLoadHint);
	currentDoc->appendChild(fakeElem.firstChild());

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

	if ( node->isNewPersistenceUnit() ) saveNewPersistenceUnit(node, name, partialLoadHint);
	else
		saveNodeDirectly(node, name, partialLoadHint);
}

void FileStore::saveNodeDirectly(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	QDomElement* oldParent = currentParent;

	QDomElement elem = currentDoc->createElement(node->getTypeName());
	elem.setAttribute("name", name);
	elem.setAttribute("id", QString::number(node->getId()));
	elem.setAttribute("partial", partialLoadHint ? ATTRIBUTE_TRUE : ATTRIBUTE_FALSE);

	currentParent = &elem;
	node->save(*this);

	if ( !node->isFullyLoaded() )
	{
		// Persist sub nodes which have not been loaded. Each subnode has a unique name. If a node was previously
		// persisted and was not saved so far, we will read it back from the persistent store and store it again.

		// Get a list of sub nodes which have already been persisted.
		QStringList persistedChildren = getChildrenNames(currentParent);

		// Load the old persisted version of this node
		int depth;
		QString filename = getPersistenceUnitName(node, &depth);
		QDomElement rootElem = loadDoc(filename).firstChildElement();

		// Search through the content in order to find the requested node.
		QDomElement* oldElem = findElementById(&rootElem, QString::number(node->getId()), depth);

		if (!oldElem) throw FilePersistenceException("Could not find the persistent unit for partial node with id " + QString::number(node->getId()));

		QDomElement child = oldElem->firstChildElement();
		while ( !child.isNull() )
		{
			QString name = child.attribute("name", QString());
			if (!persistedChildren.contains(name))
			{
				currentParent->appendChild( currentDoc->importNode(child, true) );
			}

			child = child.nextSiblingElement();
		}

	}
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
	QDomElement elem = loadDoc(name).firstChildElement();
	return loadNode(elem, parent);
}

QDomDocument FileStore::loadDoc(const QString& name) const
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
	return doc;
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

Model::Node* FileStore::loadSubNode(Model::Node* parent, const QString& name)
{
	checkIsWorking();

	QDomElement elem = currentParent->firstChildElement();
	while ( !elem.isNull() )
	{
		if ( elem.attribute("name", QString()) == name )
		{
			Model::LoadedNode ln;
			if ( elem.tagName() == XML_NEWUNIT_NODE_TAG ) ln = loadNewPersistenceUnit(elem.firstChild().nodeValue(), parent);
			else
				ln = loadNode(elem, parent);

			return ln.node;
		}

		elem = elem.nextSiblingElement();
	}

	return NULL;
}

QList<Model::LoadedNode> FileStore::loadPartialNode(Model::Node* partialNode)
{
	storeAccess.lock();
	working = true;
	Model::LoadedNode ln;

	QList < Model::LoadedNode > result;

	try
	{
		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + partialNode->getModel()->getName());
		if ( !modelDir.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir.path());

		int depth;
		QString filename = getPersistenceUnitName(partialNode, &depth);
		QDomElement rootElem = loadDoc(filename).firstChildElement();

		// Search through the content in order to find the requested node it.
		currentParent = findElementById(&rootElem, QString::number(partialNode->getId()), depth);

		// Load all subnodes and return them
		result = loadAllSubNodes(partialNode);
	}
	catch (Model::ModelException& e)
	{
		working = false;
		storeAccess.unlock();
		throw;
	}

	working = false;
	storeAccess.unlock();

	return result;
}

QDomElement* FileStore::findElementById(QDomElement* root, const QString& id, int depthLimit)
{
	if ( depthLimit <= 0 && root->attribute("id", QString()) == id ) return root;

	// If we are at the depth limit and the previous check was unsuccessful return NULL
	// If the procedure was started with depthLimit == -1 we will continue searching
	if ( depthLimit == 0 ) return NULL;

	QDomElement elem = root->firstChildElement();
	while ( !elem.isNull() )
	{
		QDomElement* r = findElementById(&elem, id, depthLimit - 1);
		if ( r ) return r;

		elem = elem.nextSiblingElement();
	}

	return NULL;
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
