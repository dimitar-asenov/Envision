/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "HBinaryNode.h"
#include "../src/autocomplete/AutoComplete.h"
#include "../src/autocomplete/AutoCompleteEntry.h"
#include "../src/autocomplete/AutoCompleteVis.h"


namespace Interaction {

HBinaryNode::HBinaryNode()
{
}

HBinaryNode* HBinaryNode::instance()
{
	static HBinaryNode h;
	return &h;
}

void HBinaryNode::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->modifiers() == 0 && event->key() == Qt::Key_F1)
	{
		if (AutoComplete::isVisible())
			AutoComplete::hide();
		else
		{
			QList<AutoCompleteEntry*> entr;
			entr.append( new AutoCompleteEntry{"foo", "will foo"} );
			entr.append( new AutoCompleteEntry{"bar", "will bar"} );
			AutoComplete::show(entr);
		}
	}
	else HComposite::keyPressEvent(target, event);
}

}
