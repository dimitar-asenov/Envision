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
#include "Method.h"
#include "MethodItem.h"
#include "Statement.h"

template class Model::TypedList<OOModel::Class>;
template class Model::TypedList<OOModel::Project>;
template class Model::TypedList<OOModel::Module>;
template class Model::TypedList<OOModel::Method>;
template class Model::TypedList<OOModel::MethodItem>;
template class Model::TypedList<OOModel::Statement>;

