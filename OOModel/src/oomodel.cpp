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

#include "oomodel.h"
#include "allOOModelNodes.h"

#include "SelfTest/src/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( oomodel, OOModel::OOModel )

namespace OOModel {

Core::InitializationRegistry& nodeTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

bool OOModel::initialize(Core::EnvisionManager&)
{
	nodeTypeInitializationRegistry().initializeAll();

	// Register default element creation functions for some lists
	Model::TypedList<StatementItem>::setDefaultElementCreationFunction([]() -> StatementItem* {
		return new ExpressionStatement(new EmptyExpression()); } );
	Model::TypedList<FormalArgument>::setDefaultElementCreationFunction([]() -> FormalArgument* {
		auto arg = new FormalArgument();
		arg->setTypeExpression(new EmptyExpression());
		return arg;} );
	Model::TypedList<FormalResult>::setDefaultElementCreationFunction([]() -> FormalResult* {
		auto res = new FormalResult();
		res->setTypeExpression(new EmptyExpression());
		return res;} );
	Model::TypedList<FormalTypeArgument>::setDefaultElementCreationFunction([]() -> FormalTypeArgument* {
		return new FormalTypeArgument();} );
	Model::TypedList<Field>::setDefaultElementCreationFunction([]() -> Field* {
		auto field = new Field();
		field->setTypeExpression(new EmptyExpression());
		return field;} );
	Model::TypedList<Enumerator>::setDefaultElementCreationFunction([]() -> Enumerator* { return new Enumerator();} );
	Model::TypedList<Expression>::setDefaultElementCreationFunction([]() -> Expression* {
		return new EmptyExpression();} );

	return true;
}

void OOModel::unload()
{
}

void OOModel::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOModel>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOModel>::runTest(testid).printResultStatistics();
}

}
