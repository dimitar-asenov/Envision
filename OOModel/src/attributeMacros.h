/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

/**
 * Declares a 'name' attribute of type Model::NameText.
 *
 * The attribute node can be accessed via nameNode()
 * The name string is accessible via name()
 * A new value can be set via setName()
 */
#define ATTRIBUTE_OOP_NAME_NOSYMBOL																											\
	ATTRIBUTE_VALUE_CUSTOM_RETURN(::Model::NameText, name, setName, QString, const QString&)

/**
 * Declares a 'name' attribute of type Model::NameText and makes the composite node provide this name as a symbol.
 *
 * The attribute node can be accessed via nameNode()
 * The name string is accessible via name()
 * A new value can be set via setName()
 */
#define ATTRIBUTE_OOP_NAME_SYMBOL																												\
		ATTRIBUTE_OOP_NAME_NOSYMBOL																												\
	public:																																				\
		virtual bool definesSymbol() const override;																							\
		virtual const QString& symbolName() const override;																				\
		virtual SymbolTypes symbolType() const override;																					\
	private:																																				\

/**
 * Registers a mandatory OOName attribute which does not declare a symbol.
 */
#define REGISTER_OONAME_NOSYMBOL_ATTRIBUTE(ClassName)																						\
DEFINE_ATTRIBUTE(ClassName, name, NameText, false, false, true)

/**
 * Registers a mandatory OOName attribute which declares a symbol.
 */
#define REGISTER_OONAME_SYMBOL_ATTRIBUTE(ClassName, symbolTypes)																		\
REGISTER_OONAME_NOSYMBOL_ATTRIBUTE(ClassName)																								\
bool ClassName::definesSymbol() const { return true;}																						\
const QString& ClassName::symbolName() const { return name(); }																		\
ClassName::SymbolTypes ClassName::symbolType() const { return (symbolTypes);}

/**
 * Declares an 'annotations' attribute of type OOModel::StatementItemList.
 *
 * The annotations list can be accessed via annotations()
 * A new value can be set via setAnnotations()
 */
#define ATTRIBUTE_OOP_ANNOTATIONS																												\
	ATTRIBUTE(::OOModel::StatementItemList, annotations, setAnnotations)
