/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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
 * CallStringComponents.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: Dimitar Asenov
 */

#include "string_components/CallStringComponents.h"

#include "OOModel/headers/expressions/MethodCallExpression.h"
#include "ModelBase/headers/adapter/AdapterManager.h"

namespace OOInteraction {

CallStringComponents::CallStringComponents(OOModel::MethodCallExpression* e )
	: exp_(e)
{
}

QStringList CallStringComponents::components()
{
	QStringList result;
	if (!exp_) return result;

	StringComponents* prefix = Model::AdapterManager::adapt<StringComponents>(exp_->prefix());
	if (prefix)
	{
		result.append( prefix->components().join("") );
		result.append(".");
		SAFE_DELETE(prefix);
	}

	result.append( exp_->ref()->path().split(':').last() );

	result.append("(");
	for (int i=0; i< exp_->arguments()->size(); ++i)
	{
		StringComponents* operand = Model::AdapterManager::adapt<StringComponents>(exp_->arguments()->at(i));
		if (operand)
		{
			result.append( operand->components().join("") );
			SAFE_DELETE(operand);
		}
		else result.append( QString() );

		if (i< exp_->arguments()->size() - 1)
			result.append(",");
	}

	result.append(")");
	return result;
}

} /* namespace OOInteraction */
