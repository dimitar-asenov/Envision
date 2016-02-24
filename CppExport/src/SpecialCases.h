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

namespace OOModel
{
	class Class;
	class Method;
	class MetaCallExpression;
	class MetaDefinition;
	class ReferenceExpression;
}

namespace Export
{
	class SourceFragment;
	class CompositeFragment;
}

namespace CppExport {

class CodeComposite;

const QString XMACRO_DATA_FILENAME = "StandardExpressionDefinitions";
const QString XMACRO_INSTANTIATION_FILENAME = "StandardExpressionVisualizations";

class CPPEXPORT_API SpecialCases
{
	public:
		static void handleQT_Flags(OOModel::Class* classs, Export::CompositeFragment* fragment);

		static bool isTestClass(OOModel::Class* classs);

		/*
		 * Transforms
		 *
		 * #define DECLARE_TYPE_ID_COMMON(OVERRIDE)\
		 * virtual const QString& typeName() const
		 * {
		 *		SET_OVERRIDE_FLAG(OVERRIDE);
		 * }
		 *
		 * into
		 *
		 * #define DECLARE_TYPE_ID_COMMON(OVERRIDE)\
		 * virtual const QString& typeName() const OVERRIDE;\
		 */
		static void overrideFlag(OOModel::Method* method, Export::CompositeFragment* fragment);

		/*
		 * Transforms
		 *
		 * #define DECLARE_TYPE_ID()\					#define DECLARE_TYPE_ID_BASE()\
		 * DECLARE_TYPE_ID_COMMON(true)\				DECLARE_TYPE_ID_COMMON(false)\
		 *
		 * into
		 *
		 * #define DECLARE_TYPE_ID()\					#define DECLARE_TYPE_ID_BASE()\
		 * DECLARE_TYPE_ID_COMMON(override)\		DECLARE_TYPE_ID_COMMON()\
		 */
		static Export::CompositeFragment* overrideFlagArgumentTransformation(OOModel::MetaCallExpression* metaCall);

		static bool hasTemplatePrefixArgument(OOModel::MetaDefinition* metaDefinition);

		static bool isMainMethod(OOModel::Method* method);

		static bool isTemplateArgumentNameOnlyDependency(OOModel::ReferenceExpression* parentReference,
																		 OOModel::ReferenceExpression* argumentReference);
		static Export::SourceFragment* printXMacroUsage(OOModel::MetaCallExpression* beginPartialMetaCall);
		static Export::CompositeFragment* printPartialBeginMacroSpecialization(OOModel::MetaDefinition* metaDefinition,
																									  bool isHeaderFile);
		static Export::CompositeFragment* printPartialBeginMacroBase(OOModel::MetaDefinition* beginPartialMetaDefinition,
																						 bool isHeaderFile);
		static Export::SourceFragment* includeXMacroData(CodeComposite* codeComposite,
																				  Export::SourceFragment* baseFragment,
																				  bool isSourceFile);
};

}
