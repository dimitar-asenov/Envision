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

#include "../visualizationbase_api.h"

#include "StaticStyle.h"
#include "../declarative/DeclarativeItemBaseStyle.h"
#include "../layouts/SequentialLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API VListStyle : public Super<DeclarativeItemBaseStyle>
{
	public:
		~VListStyle();

		Property<SequentialLayoutStyle> itemsStyle{this, "itemsStyle"};
		Property<bool> useBackgroundColors{this, "useBackgroundColors"};
		Property<QVector<QColor>> backgroundColors{this, "backgroundColors"};

		Property<bool> showTipWhenSelectedAndEmpty{this, "showTipWhenSelectedAndEmpty"};
		Property<Visualization::StaticStyle> selectedTip{this, "selectedTip"};

		Property<int> minWidth{this, "minWidth"};
		Property<int> minHeight{this, "minHeight"};
		Property<bool> stretchWhenEmpty{this, "stretchWhenEmpty"};

		Property<bool> linelikeMovementAlongListDirection{this, "linelikeMovementAlongListDirection"};

		/**
		 * Whther to create a child element, also when a letter is pressed, in addition to when Enter is pressed.
		 *
		 * The letter key press will be forwarded to the child element.
		 */
		Property<bool> createChildOnLetterPress{this, "createChildOnLetterPress"};
};

}
