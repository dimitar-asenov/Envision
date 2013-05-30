/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "declarations/VProject.h"
#include "declarations/VModule.h"
#include "declarations/VClass.h"
#include "declarations/VMethod.h"
#include "declarations/VField.h"

#include "elements/VEnumerator.h"
#include "elements/VFormalArgument.h"
#include "elements/VFormalResult.h"
#include "elements/VFormalTypeArgument.h"
#include "elements/VStatementItemList.h"
#include "elements/VCatchClause.h"

#include "expressions/allOOExpressionVisualizations.h"

#include "statements/VStatementItem.h"
#include "statements/VBlock.h"
#include "statements/VReturnStatement.h"
#include "statements/VIfStatement.h"
#include "statements/VLoopStatement.h"
#include "statements/VForEachStatement.h"
#include "statements/VBreakStatement.h"
#include "statements/VContinueStatement.h"
#include "statements/VExpressionStatement.h"
#include "statements/VTryCatchFinally.h"

#include "alternative/VKeywordMethodCall.h"
