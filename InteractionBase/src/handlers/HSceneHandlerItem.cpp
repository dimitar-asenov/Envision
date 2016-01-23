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

#include "HSceneHandlerItem.h"

#include "../commands/CSceneHandlerItemExit.h"
#include "../commands/CFind.h"
#include "../commands/CSceneHandlerLoad.h"
#include "../commands/CSceneHandlerSave.h"
#include "../commands/CNewView.h"
#include "../commands/CSwitchView.h"
#include "../commands/CRemoveNodeFromView.h"
#include "../commands/CToggleArrowLayer.h"
#include "../commands/CAddInfoNode.h"
#include "../commands/CToggleInfoEntry.h"
#include "../commands/CSaveView.h"
#include "../commands/CAddNodeToViewByName.h"
#include "../commands/CAddNodeToView.h"

namespace Interaction {

HSceneHandlerItem::HSceneHandlerItem()
{
	// TODO: is it appropriate to add commands in the constructor or should they be registered somewhere else?
	addCommand(new CSceneHandlerItemExit{});
	addCommand(new CFind{});
	addCommand(new CSceneHandlerLoad{});
	addCommand(new CSceneHandlerSave{});
	addCommand(new CNewView{});
	addCommand(new CSwitchView{});
	addCommand(new CRemoveNodeFromView{});
	addCommand(new CToggleArrowLayer{});
	addCommand(new CAddInfoNode{});
	addCommand(new CToggleInfoEntry{});
	addCommand(new CSaveView{});
	addCommand(new CAddNodeToViewByName{});
	addCommand(new CAddNodeToView{});
}

HSceneHandlerItem* HSceneHandlerItem::instance()
{
	static HSceneHandlerItem h;
	return &h;
}

}
