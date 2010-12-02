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

FileStore::FileStore() :
	baseFolder(QDir::home()), working(false), currentDom(NULL)
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
	model.beginExclusiveRead();

	try
	{
		if ( !baseFolder.exists(name) )
		{
			if ( !baseFolder.mkdir(name) ) throw FilePersistenceException("Could not create folder " + name + " for model.");
		}

		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + name);

		if ( !modelDir.exists() ) throw FilePersistenceException("Error opening model folder " + modelDir.path());

		QDomDocument doc("EnvisionFilePersistence");
		QDomElement rootDom = doc.createElement("root");
		doc.appendChild(rootDom);

		Model::Node* root = model.getRoot();
		if ( root ) root->save(*this);

		// DO This stuff
		QFile file( "test.xml" );
		  if( !file.open( IO_WriteOnly ) )
		  return -1;

		  QTextStream ts( &file );
		  ts << doc.toString();

		  file.close();
		 // UNTIL HERE    and think of fail safety.

	}
	catch (Model::ModelException& e)
	{
		model.endExclusiveRead();
		storeAccess.unlock();
		throw;
	}
	model.endExclusiveRead();
	storeAccess.unlock();
}

void FileStore::saveStringValue(const QString &value)
{
}

void FileStore::saveIntValue(int value)
{
}

void FileStore::saveFloatValue(double value)
{
}

void FileStore::saveNode(const Model::Node *node, const QString &name, bool partialLoadHint)
{
}

Model::Node* FileStore::loadRootNode(const QString &name)
{
}

QList<Model::LoadedNode> FileStore::loadAllSubNodes(Model::Node* parent)
{
}

int FileStore::loadIntValue()
{
}

QString FileStore::loadStringValue()
{
}

double FileStore::loadFloatValue()
{
}

}
