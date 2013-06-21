/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "CustomizationVisitor.h"
#include "../expression_editor/CommandDescriptor.h"
#include "../expression_editor/commands/CreateMethodCall.h"

#include "OOVisualization/src/alternative/VKeywordMethodCall.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/expressions/StringLiteral.h"
#include "OOModel/src/expressions/IntegerLiteral.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "VisualizationBase/src/renderer/VisualizationGroup.h"

namespace OOInteraction {

// This is to force the vtable to be generated once only in OOInteraction
CustomizationVisitor::~CustomizationVisitor(){}

Visualization::VisualizationGroup* CustomizationVisitor::customizationGroup_{};
QList<CommandExpression*> CustomizationVisitor::registeredCommands_;

void CustomizationVisitor::init(Visualization::VisualizationGroup* customizationGroup)
{
	customizationGroup_ = customizationGroup;
	addType<OOModel::Method>(visitMethod);
}

Model::Node* CustomizationVisitor::visitMethod(CustomizationVisitor*, OOModel::Method* met)
{
	for(auto annotation : *met->annotations())
	{
		if ( auto sti = dynamic_cast<OOModel::ExpressionStatement*>(annotation) )
			if (auto call = dynamic_cast<OOModel::MethodCallExpression*>(sti->expression()) )
			{
				if (auto ref = dynamic_cast<OOModel::ReferenceExpression*>(call->callee() ))
				{
					if (ref->prefix() == nullptr && ref->name() == "EnvisionKeywordVisualization"
							&& call->arguments()->size() == 1)
					{
						if (auto styleName = dynamic_cast<OOModel::StringLiteral*>(call->arguments()->first()))
						{

							// Register Visualizations in the group
							QString styleNameString = styleName->value();
							customizationGroup_->addVisualization(
									[=](Visualization::Item* parent, Model::Node* node) -> Visualization::Item*
									{
										return new OOVisualization::VKeywordMethodCall(
												parent, static_cast<OOModel::MethodCallExpression*> (node),
												OOVisualization::VKeywordMethodCall::itemStyles().get(styleNameString));
									},
									[=](Visualization::Item*, Model::Node* node) -> bool
									{
										auto call = static_cast<OOModel::MethodCallExpression*>(node);
										return call->methodDefinition() == met;
									});
						}
					}
					else if  (ref->prefix() == nullptr && ref->name() == "EnvisionShortcut"
							&& call->arguments()->size() >= 1 && call->arguments()->size() <=2)
					{
						if (auto keyword = dynamic_cast<OOModel::StringLiteral*>(call->arguments()->first()))
						{
							CommandExpression* command{};
							if (call->arguments()->size() == 1)
								command = new CreateMethodCall(keyword->value(), met->fullyQualifiedName());
							else if (auto numArgs = dynamic_cast<OOModel::IntegerLiteral*>(call->arguments()->last()))
								command = new CreateMethodCall(keyword->value(), met->fullyQualifiedName(), numArgs->value());

							if (command)
							{
								registeredCommands_.append(command);
								CommandDescriptor::registerCommand(command);
							}
						}
					}
				}
			}
	}

	return met;
}

void CustomizationVisitor::resetCustomizations()
{
	if (customizationGroup_) customizationGroup_->clear();
	for (auto command : registeredCommands_) CommandDescriptor::unregisterCommand(command);
	registeredCommands_.clear();
}

void CustomizationVisitor::onSceneRefresh(Visualization::Scene* scene)
{
	resetCustomizations();
	CustomizationVisitor customizations;
	for (auto top : scene->topLevelItems())
		if (top->hasNode()) customizations.visit(top->node());
}

} /* namespace OOInteraction */
