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

#ifndef VisualizationBase_SEQUENTIALLAYOUTELEMENT_H_
#define VisualizationBase_SEQUENTIALLAYOUTELEMENT_H_

#include "../visualizationbase_api.h"
#include "LayoutElement.h"

namespace Model {
class Node;
class List;
}

namespace Visualization {

class Element;

class VISUALIZATIONBASE_API SequentialLayoutElement : public LayoutElement
{
	public:
		// Functions executable on element definition
		SequentialLayoutElement(Model::List* listNode);
		SequentialLayoutElement(std::function<QList<Model::Node*>()> nodeListGetter);
		SequentialLayoutElement(std::function<QList<Item*>()> itemListGetter);
		virtual ~SequentialLayoutElement();

		SequentialLayoutElement* setSpaceBetweenElements(int space);
		SequentialLayoutElement* setOrientation(Qt::Orientation o);
		SequentialLayoutElement* setHorizontal();
		SequentialLayoutElement* setVertical();

		// Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual void synchronizeWithItem(Item* item) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;

		// Recursive item destruction
		virtual void destroyChildItems(Item* item) override;

	private:
		Model::List* listNode_{};
		std::function<QList<Model::Node*>()> nodeListGetter_{};
		std::function<QList<Item*>()> itemListGetter_{};

		int spaceBetweenElements_{};
		Qt::Orientation orientation_{Qt::Horizontal};

		QList<Item*> computeItems(Item* item);
};

inline SequentialLayoutElement* SequentialLayoutElement::setSpaceBetweenElements(int space)
{
	spaceBetweenElements_ = space;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setOrientation(Qt::Orientation o)
{
	orientation_ = o;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setHorizontal()
{
	setOrientation(Qt::Horizontal);
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setVertical()
{
	setOrientation(Qt::Vertical);
	return this;
}

}

#endif /* VisualizationBase_SEQUENTIALLAYOUTELEMENT_H_ */
