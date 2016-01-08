/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "../informationscripting_api.h"

#include "InteractionBase/src/handlers/GenericHandler.h"

namespace InformationScripting {

class QueryNodeContainer;
class QueryNode;

class HQuery : public Interaction::GenericHandler {
	protected:
		HQuery() = default;

	public:
		static HQuery* instance();

		static void initStringComponents();

		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event);

	private:
		QueryNodeContainer* parentContainer(QueryNode* e);

		Visualization::Item* stringInfo(Visualization::Item* target, Qt::Key key, QString& str, int& index);

		void setNewQuery(Visualization::Item* target, Visualization::Item* topMostItem, const QString& text, int index);

		static bool processDeleteOrBackspace(Qt::Key key, QString& exp, int& index);
		static bool canBeRemoved(const QString& exp, int index);

		static int processEnter(QString& exp, int index);
		static int removeListsWithOneElement(QString& exp, int& index, int iteratorIndex = -1);

		static bool isOperatorAtIndex(const QString& exp, int index);
};

}
