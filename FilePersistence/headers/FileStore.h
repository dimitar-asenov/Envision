/***********************************************************************************************************************
 * FileStore.h
 *
 *  Created on: Dec 2, 2010
 *      Author: dasenov
 **********************************************************************************************************************/

#ifndef FILESTORE_H_
#define FILESTORE_H_

#include "ModelBase/headers/PersistentStore.h"

namespace FilePersistence {

class FileStore : public Model::PersistentStore
{
	public:
		FileStore();
		virtual ~FileStore();
};

}

#endif /* FILESTORE_H_ */
