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
 * CommaExpression.h
 *
 *  Created on: Feb 24, 2012
 *      Author: Dimitar Asenov
 */

#ifndef OOModel_COMMAEXPRESSION_H_
#define OOModel_COMMAEXPRESSION_H_

#include "Expression.h"

namespace OOModel {

class OOMODEL_API CommaExpression: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(CommaExpression)

	ATTRIBUTE(Expression, left, setLeft)
	ATTRIBUTE(Expression, right, setRight)

	public:
		/**
		 * \brief Returns in a single list all expressions of this and nested CommaExpression operators.
		 *
		 * If \a detachOperands is set all returned expressions will have no parents and the CommaExpression can be
		 * deleted.
		 */
		QList<Expression*> allSubOperands(bool detachOperands);

		virtual Type* type();
		virtual QList<Node*> findSymbol(const QString& symbol, Node* source, FindSymbolMode mode);

};

} /* namespace OOModel */
#endif /* OOModel_COMMAEXPRESSION_H_ */
