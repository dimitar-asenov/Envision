/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

/*
 * ModelManager.h
 *
 *  Created on: Sep 14, 2012
 *      Author: Dimitar Asenov
 */

#ifndef ModelBase_MODELMANAGER_H_
#define ModelBase_MODELMANAGER_H_

#include "../modelbase_api.h"

namespace Model {

class Model;
class Node;

/**
 * The ModelManager class contains all existing models.
 */
class MODELBASE_API ModelManager {
	friend class Model;

	public:
		~ModelManager();

		/**
		 * Registers types with the meta object system of Qt to allow signals and slots to work with lists.
		 */
		static void init();

		static ModelManager& instance();

		/**
		 * Returns the model object that has as its root node the node indicated.
		 */
		Model* find(Node* root) const;

		/**
		 * Returns a list to all currently loaded models.
		 */
		const QList<Model*>& loadedModels() const;

	private:
		ModelManager();

		/**
		 * Adds \a model the list of managed models. This manager will take ownership of \a model.
		 *
		 * Newly created instances of Model call this method in their constructors.
		 */
		void add(Model* model);

		/**
		 * Removes \a model from the list of managed models. This manager will give up ownership of \a model.
		 *
		 * The removed model is returned. This method is called in the desctructor of Model.
		 *
		 */
		Model* remove(Model* model);

		/**
		 * A list of all Model objects that are currently instantiated. This is used to find the Model corresponding to a
		 * particular root object.
		 */
		QList<Model*> loadedModels_;
};

inline const QList<Model*>& ModelManager::loadedModels() const { return loadedModels_; }

} /* namespace Model */
#endif /* ModelBase_MODELMANAGER_H_ */
