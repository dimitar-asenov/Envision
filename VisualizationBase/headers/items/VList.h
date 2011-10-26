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
 * VList.h
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLIST_H_
#define VLIST_H_

#include "../visualizationbase_api.h"

#include "ItemWithNode.h"
#include "LayoutProvider.h"
#include "VListStyle.h"
#include "../layouts/SequentialLayout.h"
#include "ModelBase/headers/nodes/List.h"

namespace Visualization {

class VISUALIZATIONBASE_API VList: public ItemWithNode< LayoutProvider<>, Model::List>
{
	ITEM_COMMON(VList)

	protected:
		virtual void determineChildren();

	public:
		VList(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		int length() const;
		template <class T> T* at(int index);
		int focusedElementIndex() const;
};

inline int VList::length() const { return layout()->length(); }
inline int VList::focusedElementIndex() const { return layout()->focusedElementIndex(); }
template <class T> T* VList::at(int index) { return layout()->at<T>(index); }

}

#endif /* VLIST_H_ */
