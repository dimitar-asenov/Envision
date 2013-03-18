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

#ifndef VisualizationBase_DECLARATIVEITEMBASE_H_
#define VisualizationBase_DECLARATIVEITEMBASE_H_

#include "../visualizationbase_api.h"
#include "../items/Item.h"
#include "../items/ItemStyle.h"
#include "ItemWrapperElement.h"

namespace Visualization {

class Element;

class VISUALIZATIONBASE_API DeclarativeItemBase: public Item
{
	ITEM_COMMON_CUSTOM_STYLENAME(DeclarativeItemBase, ItemStyle)

	public:
		DeclarativeItemBase(Item* parent);
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight);
		virtual QList<Element*>& forms() = 0; // array or forms
		virtual int determineForm(); // get the appropriate form for the situation

		// factory methods for ItemWrapperElement
		template <class VisualizationType, class ParentType>
			static ItemWrapperElement<ParentType, VisualizationType>*
			item(Item* ParentType::* item, const typename VisualizationType::StyleType* style);
		template <class ParentType>
			static ItemWrapperElement<ParentType>*
			item(Item* ParentType::* item, std::function<Model::Node* (ParentType* v)> nodeGetter);

	private:
		typedef Item BaseItemType;
};


template <class VisualizationType, class ParentType>
	ItemWrapperElement<ParentType, VisualizationType>*
	DeclarativeItemBase::item(Item* ParentType::* item, const typename VisualizationType::StyleType* style)
{
	return new ItemWrapperElement<ParentType, VisualizationType>(item, style);
}

template <class ParentType>
	ItemWrapperElement<ParentType>*
	DeclarativeItemBase::item(Item* ParentType::* item, std::function<Model::Node* (ParentType* v)> nodeGetter)
{
	return new ItemWrapperElement<ParentType>(item, nodeGetter);
}

}

#endif /* VisualizationBase_DECLARATIVEITEMBASE_H_ */
