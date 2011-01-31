/***********************************************************************************************************************
 * OOReference.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "OOReference.h"

namespace OOModel {

NODE_DEFINE_EMPTY_CONSTRUCTORS(OOReference, Reference)
NODE_DEFINE_TYPE_REGISTRATION_METHODS(OOReference)

Model::Node* OOReference::getTargetFromSymbolicPath(const QString &path)
{
	// TODO implement custom references handling
	return getTargetFromLocalPath(path);
}

QString OOReference::getSymbolicPathToTarget(Node *target)
{
	// TODO implement custom references handling
	return getLocalPathToTarget(target);
}

}
