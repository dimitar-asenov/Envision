/***********************************************************************************************************************
 * FilePersistenceException.h
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FILEPERSISTENCEEXCEPTION_H_
#define FILEPERSISTENCEEXCEPTION_H_

#include "filepersistence_api.h"
#include "EnvisionException.h"

namespace FilePersistence {

class FILEPERSISTENCE_API FilePersistenceException : public Envision::EnvisionException
{
		public:
	FilePersistenceException(const QString& message);
			const QString& name() const;
};

}

#endif /* FILEPERSISTENCEEXCEPTION_H_ */
