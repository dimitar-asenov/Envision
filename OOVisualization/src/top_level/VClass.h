/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * VClass.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCLASS_H_
#define VCLASS_H_

#include "../oovisualization_api.h"
#include "VClassStyle.h"

#include "OOModel/src/top_level/Class.h"

#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/items/LayoutProvider.h"

namespace Visualization {
	class VText;
	class VList;
	class PanelBorderLayout;
	class Static;
	class PositionLayout;
}

namespace OOVisualization {

class OOVISUALIZATION_API VClass : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::Class>
{
	ITEM_COMMON(VClass)

	public:
		VClass(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VClass();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header_;
		Visualization::Static* icon_;
		Visualization::VText* name_;
		Visualization::VList* baseClasses_;
		Visualization::PositionLayout* content_;
		Visualization::SequentialLayout* fieldContainer_;
		Visualization::SequentialLayout* publicFieldArea_;
		Visualization::SequentialLayout* privateFieldArea_;
		Visualization::SequentialLayout* protectedFieldArea_;
		Visualization::SequentialLayout* defaultFieldArea_;
};

}

#endif /* VCLASS_H_ */
