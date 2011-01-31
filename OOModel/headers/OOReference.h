/***********************************************************************************************************************
 * OOReference.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef OOREFERENCE_H_
#define OOREFERENCE_H_

#include "oomodel_api.h"

#include "ModelBase/headers/nodes/Reference.h"

namespace OOModel {

class OOMODEL_API OOReference : public Model::Reference
{
	NODE_DECLARE_STANDARD_METHODS( OOReference )

	protected:
		virtual Node* getTargetFromSymbolicPath(const QString &path);
		virtual QString getSymbolicPathToTarget(Node* target);
};

}

#endif /* OOREFERENCE_H_ */
