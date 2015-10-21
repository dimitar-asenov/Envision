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

namespace Model {
	class Node;
}

namespace OOModel {
	class ReferenceExpression;
}

namespace CppExport {

class File;

class DependencyAnalyzer
{
	public:
		/**
		 * return node to file association with dependency information.
		 */
		QList<File*> files(Model::Node* node);

	private:
		QHash<QString, File*> nameToFileMap_;
		QHash<Model::Node*, File*> nodeToFileMap_;

		void allChildrenOfTypeReferenceExpression(Model::Node* node, QList<OOModel::ReferenceExpression*>& result);

		/**
		 * associates a node with the file with filename name.
		 */
		void associateNodeWithFile(Model::Node* node, QString name);

		/**
		 * associates the provided tree with files.
		 */
		void buildNodeToFileMap(Model::Node* node, QString namespaceName);

		bool isSoftDependency(OOModel::ReferenceExpression* reference);

		/**
		 * calculates dependency information for file and stores it in file.
		 */
		void calculateDependencies(File* file);

		File* headerFileForNode(Model::Node* node);

		Model::Node* resolveReference(OOModel::ReferenceExpression* referenceExpression);
};

}
