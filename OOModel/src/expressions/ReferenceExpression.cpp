/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * ReferenceExpression.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/ReferenceExpression.h"
#include "top_level/Project.h"
#include "top_level/Library.h"
#include "top_level/Module.h"
#include "top_level/Class.h"
#include "../types/SymbolProviderType.h"
#include "../types/ClassType.h"
#include "../types/ErrorType.h"


namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(ReferenceExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(ReferenceExpression, Expression)

REGISTER_ATTRIBUTE(ReferenceExpression, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(ReferenceExpression, ref, OOReference, false, false, true)

ReferenceExpression::ReferenceExpression(const QString& name, Expression* prefix)
: Expression(nullptr, ReferenceExpression::getMetaData())
{
	ref()->setName(name);
	if (prefix != nullptr) setPrefix(prefix);
}

Type* ReferenceExpression::type()
{
	if ( auto project = dynamic_cast<Project*>( ref()->target() ) )
		return new SymbolProviderType(project, false);
	else if ( auto library = dynamic_cast<Library*>( ref()->target() ) )
		return new SymbolProviderType(library, false);
	else if ( auto module = dynamic_cast<Module*>( ref()->target() ) )
		return new SymbolProviderType(module, false);
	else if ( auto cl = dynamic_cast<Class*>( ref()->target() ) )
		return new ClassType(cl, false);
	else return new ErrorType("Invalid reference expression type");
}

}
