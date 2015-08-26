/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#pragma once

#include "../informationscripting_api.h"

#include "Query.h"

namespace Model {
	class Node;
}

namespace OOModel {
	class Class;
	class Method;
}

namespace InformationScripting {

class Graph;
class InformationNode;

class INFORMATIONSCRIPTING_API AstQuery : public Query
{
	public:
		enum class QueryType : int {Classes, Methods, BaseClasses, ToClass, CallGraph};
		enum class Scope : int {Local, Global, Input};

		AstQuery(QueryType type, Model::Node* target, QStringList args);

		virtual QList<Graph*> execute(QList<Graph*> input) override;

	private:
		Model::Node* target_{};
		Scope scope_{};
		QueryType type_{};

		Graph* classesQuery(QList<Graph*>& input);
		Graph* methodsQuery(QList<Graph*>& input);
		Graph* baseClassesQuery(QList<Graph*>& input);
		Graph* toClassNode(QList<Graph*>& input);
		Graph* callGraph(QList<Graph*>& input);

		void addBaseEdgesFor(OOModel::Class* childClass, InformationNode* classNode, Graph* g);

		void addGlobalNodesOfType(Graph* g, const QString& typeName);

		void addCallInformation(Graph* g, OOModel::Method* method, QList<OOModel::Method*> callees);
};

} /* namespace InformationScripting */
