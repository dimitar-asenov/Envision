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

#pragma once

#include "../oointeraction_api.h"

#include "InteractionBase/src/handlers/GenericHandler.h"

namespace OOModel {
	class Expression;
	class ExpressionStatement;
}

namespace OOInteraction {

class OOINTERACTION_API HExpression : public Interaction::GenericHandler {
	protected:
		HExpression();

	public:
		static HExpression* instance();

		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event);

		typedef std::function<void (OOModel::Expression*&, int&)> ExpressionChangeMonitorFunction;
		void appendExpressionMonitor(ExpressionChangeMonitorFunction monitor);

	private:
		OOModel::ExpressionStatement* parentExpressionStatement(OOModel::Expression* e);
		void toggleAutoComplete(Visualization::Item* target);
		void showAutoComplete(Visualization::Item* target, bool showIfEmpty = true, bool showIfPreciselyMatched = true);

		Visualization::Item* stringInfo(Visualization::Item* target, Qt::Key key, QString& str, int& index);

		void doAutoComplete(Visualization::Item* target, const QString& autoCompleteStr);
		void setNewExpression(Visualization::Item* target, Visualization::Item* topMostItem, const QString& text,
				int index);

		QList<ExpressionChangeMonitorFunction> expressionMonitors_;
};

inline void HExpression::appendExpressionMonitor(ExpressionChangeMonitorFunction monitor)
{
	expressionMonitors_.append(monitor);
}

} /* namespace OOInteraction */
