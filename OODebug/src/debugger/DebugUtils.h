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

#include "../oodebug_api.h"
#include "jdwp/Protocol.h"

namespace Export {
	class TextToNodeMap;
}

namespace Model {
	class Node;
}

namespace OOModel {
	class Class;
	class Expression;
	class Method;
	class StatementItem;
	class StatementItemList;
	class VariableDeclaration;
}

namespace OODebug {

class DebugConnector;
struct Location;
struct Value;
struct VariableDetails;

class OODEBUG_API DebugUtils
{
	public:
		DebugUtils(DebugConnector* connector);

		void setExportMap(std::shared_ptr<Export::TextToNodeMap> map);

		QString jvmSignatureFor(OOModel::Class* theClass);
		/**
		 * Returns a String with all containing module names split by \a delimiter in front of the \a theClass name.
		 */
		QString fullNameFor(OOModel::Class* theClass, QChar delimiter);

		Location nodeToLocation(Model::Node* node);
		Model::Node* locationToNode(Location location, bool& isClosingBracket);

		double doubleFromValue(Value v);

		Protocol::Tag typeOfVariable(OOModel::Method* containingMethod, VariableDetails variable);
		Protocol::Tag typeExpressionToTag(OOModel::Expression* e);
		bool hasPrimitiveValueType(OOModel::VariableDeclaration* variableDeclaration);

		OOModel::VariableDeclaration* variableDeclarationFromStatement(OOModel::StatementItem* statement,
																							QString variableName = "");

		/**
		 * Tries to find information about the variables with names \a variableNames in the \a statementList.
		 * If a variable is not found in the \a statementList its parent list is searched until we have no more parent
		 * list then we try to find the name in the arguments of the containing Method. The \a indexFrom is the starting
		 * position from where we search, not that we search in the negative direction (upwards).
		 *
		 * The results are returned in a map variable name -> details.
		 */
		QHash<QString, OOModel::VariableDeclaration*> findVariableDetailsIn(OOModel::StatementItemList* statementList,
																				 QStringList variableNames, int indexFrom);

	private:
		DebugConnector* debugConnector_{};
		std::shared_ptr<Export::TextToNodeMap> exportMap_;
};

} /* namespace OODebug */
