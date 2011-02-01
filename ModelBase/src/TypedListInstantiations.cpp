/***********************************************************************************************************************
 * TypedListInstantiations.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/TypedListDefinition.h"

#include "nodes/Boolean.h"
#include "nodes/Float.h"
#include "nodes/Integer.h"
#include "nodes/Character.h"
#include "nodes/Reference.h"
#include "nodes/Text.h"

namespace Model {

template class TypedList<Boolean>;
template class TypedList<Float>;
template class TypedList<Integer>;
template class TypedList<Character>;
template class TypedList<Reference>;
template class TypedList<Text>;

}

