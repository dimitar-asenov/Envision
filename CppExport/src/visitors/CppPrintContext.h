/***********************************************************************************************************************
**
** Copyright (c) 2011, 2016 ETH Zurich
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

#include "../cppexport_api.h"
#include "../ExportHelpers.h"

#include "Export/src/visitor/PrintContext.h"
#include "Core/src/reflect/Reflect.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/MetaDefinition.h"

namespace Model {
	class Node;
}

namespace CppExport {

class CPPEXPORT_API CppPrintContext : public Super<Export::PrintContext>
{
	public:
		enum Option {
			None = 0,
			PrintMethodBody = 0x1,
			PrintMethodBodyIfNotEmpty = 0x2,
			PrintMethodBodyInline = 0x4,
			PrintExternKeyword = 0x8,
			PrintDefaultArgumentValues = 0x10,
			PrintTemplatePrefix = 0x20,
			PrintDeclarationCommentWithMethodBody = 0x40,
			NoApiInclude = 0x80,
			IsHeaderPart = 0x100,
			IsTemplateImplementationSeparateFile = 0x200,
			XMacro = 0x400
		};
		using Options = QFlags<Option>;

		CppPrintContext(Model::Node* context) : Super{context}
		{ Q_ASSERT(isValidPrintContext(context)); }
		CppPrintContext(Model::Node* context, Options options) : Super{context}, options_{options}
		{ Q_ASSERT(isValidPrintContext(context)); }

		bool isClass();
		bool hasOption(Option option);

		OOModel::Declaration* declaration() { return static_cast<OOModel::Declaration*>(node()); }

		static bool isValidPrintContext(Model::Node* context)
		{
			if (!context) return true;

			auto parentMetaDefinition = DCast<OOModel::MetaDefinition>(context->parent());

			return context->definesSymbol() && !context->isTransparentForNameResolution() &&
					!DCast<OOModel::MetaDefinition>(context) &&
					(!parentMetaDefinition || parentMetaDefinition->context()->name() != "Context");
		}

		Options options();

	private:
		Options options_{};
};
Q_DECLARE_OPERATORS_FOR_FLAGS(CppPrintContext::Options)

inline bool CppPrintContext::isClass() { return DCast<OOModel::Class>(node()); }
inline bool CppPrintContext::hasOption(Option option) { return options_.testFlag(option); }
inline CppPrintContext::Options CppPrintContext::options() { return options_; }

}
