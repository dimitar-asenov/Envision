/***********************************************************************************************************************
 * TypedListInstantiations.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/
#ifndef TYPEDLISTINSTANTIATIONS_H_
#define TYPEDLISTINSTANTIATIONS_H_

#include "nodes/TypedList.h"

namespace Model {

class Boolean;
template class Q_DECL_IMPORT TypedList<Boolean>;

class Float;
template class Q_DECL_IMPORT TypedList<Float>;

class Integer;
template class Q_DECL_IMPORT TypedList<Integer>;

class Character;
template class Q_DECL_IMPORT TypedList<Character>;

class Reference;
template class Q_DECL_IMPORT TypedList<Reference>;

class Text;
template class Q_DECL_IMPORT TypedList<Text>;

}

#endif /* TYPEDLISTINSTANTIATIONS_H_ */
