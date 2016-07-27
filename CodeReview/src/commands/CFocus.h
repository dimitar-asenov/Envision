/***********************************************************************************************************************
**
** Copyright (c) 2016 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "../codereview_api.h"

#include "InteractionBase/src/commands/CommandWithFlags.h"
#include "InteractionBase/src/input_actions/ActionRegistry.h"

namespace CodeReview {

class CODEREVIEW_API CFocus : public Interaction::Command
{
	public:

		struct FocusInformation
		{
			enum FocusType {
				None = 0x0,
				Center = 0x1,
				Highlight= 0x2
			};

			using FocusTypes = QFlags<FocusType>;

			int step_{};
			FocusTypes type_{};
			Model::Node* node_{};
		};

		CFocus();

		virtual bool canInterpret(Visualization::Item* source, Visualization::Item* target,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor) override;
		virtual Interaction::CommandResult* execute(Visualization::Item* source, Visualization::Item* target,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor) override;

		virtual QList<Interaction::CommandSuggestion*> suggest(Visualization::Item* source, Visualization::Item* target,
				const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>& cursor) override;

		static void loadFocusInformation();

		static void clearFocusInformation();

		static bool focusStep(Visualization::Item *, QKeySequence, Interaction::ActionRegistry::InputState);

	private:
		static int currentStep_;
		static QHash<int, FocusInformation> focusList_;
		static FocusInformation extractFocusInformation(QString line);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(CFocus::FocusInformation::FocusTypes)

inline void CFocus::clearFocusInformation() { focusList_.clear(); }


}
