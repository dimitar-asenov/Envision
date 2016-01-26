/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "cppexport_api.h"

namespace Model
{
	class Node;
}

namespace OOModel
{
	class Declaration;
	class Method;
	class Class;
}

namespace CppExport {

class CodeComposite;

class CPPEXPORT_API ExportHelpers
{
	public:
		/**
		 * used to prevent exporting of declarations already provided in a meta call of the parent.
		 */
		static bool shouldExportMethod(OOModel::Method* method, bool isHeaderVisitor, bool isSourceVisitor);
		static bool methodSignaturesMatch(OOModel::Method* method, OOModel::Method* other);

		static QString pluginName(Model::Node* node);
		static QString exportFlag(Model::Node* node);
		static CodeComposite* apiInclude(Model::Node* node);

		static bool isSignalingDeclaration(OOModel::Declaration* declaration);
		static bool isEnumWithQtFlags(OOModel::Class* candidate);
};

}
