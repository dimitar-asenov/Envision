/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#pragma once

#include "cppimport_api.h"

#include "NodeMapping.h"
#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

class CPPIMPORT_API StaticStuff
{
	public:
		static void orderNodes(QVector<Model::Node*>& input);

		static bool validContext(Model::Node* node);
		static OOModel::Declaration* actualContext(Model::Node* node);
		static OOModel::Declaration* createContext(OOModel::Declaration* actualContext);

		static QVector<Model::Node*> topLevelNodes(QVector<Model::Node*> input);

		static Model::Node* cloneWithMapping(Model::Node* node, NodeMapping* mapping);

		static void removeNode(Model::Node* node, bool removeMetaCalls = false);
		static void removeNodes(QVector<Model::Node*> nodes);

		static void addNodeToDeclaration(Model::Node* node, OOModel::Declaration* declaration);

		static OOModel::Expression* createNameExpressionFromString(const QString& input);
		static bool stringMatches(const QString& regex, const QString& value);

		/**
		 * returns the first Declaration* decl in list with decl->name() == name
		 */
		static OOModel::Declaration* findDeclaration(Model::List* list, const QString& name);

	private:
		static void buildMappingInfo(Model::Node* node, QList<Model::Node*>* info);
		static void useMappingInfo(Model::Node* node, QList<Model::Node*>* info, NodeMapping* mapping);

		static OOModel::MetaCallExpression* containsMetaCall(Model::Node* node);

};

inline void StaticStuff::removeNodes(QVector<Model::Node*> nodes) { for (auto n : nodes) removeNode(n); }

}
