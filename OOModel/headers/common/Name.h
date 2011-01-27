/***********************************************************************************************************************
 * Name.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NAME_H_
#define NAME_H_

#include "oomodel_api.h"

#include "NameBase.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"

namespace OOModel {

template <class T>
class OOMODEL_API Name: public NameBase
{
	static Model::ExtendableIndex nameIndex;
	public:
		Model::Text* name();
		void setName(const QString& newName);

		static void init();
};

template <class T> Model::ExtendableIndex Name<T>::nameIndex = Model::ExtendableIndex();

template <class T> void Name<T>::init()
{
	nameIndex = T::registerNewAttribute("name", "Text", false, false, true);
}

template <class T> Model::Text* Name<T>::name()
{
	Model::ExtendableNode* en = dynamic_cast<Model::ExtendableNode*> (this);
	return static_cast<Model::Text*> (en->get(nameIndex));
};

template <class T> void Name<T>::setName(const QString& newName)
{
	Model::ExtendableNode* en = dynamic_cast<Model::ExtendableNode*> (this);
	return static_cast<Model::Text*> (en->get(nameIndex))->set(newName);
}

}

#endif /* NAME_H_ */
