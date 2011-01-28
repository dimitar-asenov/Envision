/***********************************************************************************************************************
 * TypedListInstantiations.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelBase/headers/nodes/TypedList.h"
#include "ModelBase/headers/nodes/TypedListDefinition.h"

#include "Project.h"
#include "Module.h"
#include "Class.h"

template class Model::TypedList<OOModel::Class>;
template class Model::TypedList<OOModel::Project>;
template class Model::TypedList<OOModel::Module>;

