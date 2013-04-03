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

#ifndef VisualizationBase_ELEMENT_H_
#define VisualizationBase_ELEMENT_H_

#include "../visualizationbase_api.h"

namespace Visualization {

class Item;
class ItemRegion;

class ElementCache
{
	public:
		ElementCache() {};
		void setSize(const QSize& size);
		void setPos(const QPoint& pos);
		QSize size() const;
		QPoint pos() const;

	private:
		QSize size_{};
		QPoint pos_{}; // Position relative to the parent element
};

class VISUALIZATIONBASE_API Element
{
	public: // Methods executable on element definition
		virtual ~Element();
		// set margins
		Element* setMargins(int left, int top, int right, int bottom);
		Element* setMargins(int margin);
		Element* setTopMargin(int top);
		Element* setBottomMargin(int bottom);
		Element* setLeftMargin(int left);
		Element* setRightMargin(int right);

	public: // Methods executable when items need to be rendered
		virtual void synchronizeWithItem(Item* item) = 0;
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) = 0;
		virtual void setItemPositions(Item* item, int parentX=0, int parentY=0) = 0;
		virtual bool sizeDependsOnParent(const Item* item) const = 0;
		virtual QList<ItemRegion> regions(Item* item, int parentX=0, int parentY=0) = 0;
		void setPos(Item* item, const QPoint& pos);
		QPoint pos(Item* item) const;
		QSize size(Item* item) const;

	public: // Recursive item destruction
		virtual void destroyChildItems(Item* item);

	protected:
		void setSize(Item* item, const QSize& size);
		int topMargin();
		int bottomMargin();
		int leftMargin();
		int rightMargin();

	private:
		int marginTop_{};
		int marginBottom_{};
		int marginLeft_{};
		int marginRight_{};

		ElementCache& getCache(Item* item) const;
		void clearCache(Item* item);
		mutable QHash<const Item*, ElementCache*> elementCache_{};
};

inline Element* Element::setMargins(int left, int top, int right, int bottom)
{
	marginLeft_ = left;
	marginTop_ = top;
	marginRight_ = right;
	marginBottom_ = bottom;
	return this;
}
inline Element* Element::setMargins(int margin)
{
	marginLeft_ = margin;
	marginTop_ = margin;
	marginRight_ = margin;
	marginBottom_ = margin;
	return this;
}
inline void ElementCache::setSize(const QSize& size) {size_ = size;}
inline void ElementCache::setPos(const QPoint& pos) {pos_ = pos;}
inline QSize ElementCache::size() const {return size_;}
inline QPoint ElementCache::pos() const {return pos_;}

inline Element* Element::setTopMargin(int top) {marginTop_ = top; return this;}
inline Element* Element::setBottomMargin(int bottom) {marginBottom_ = bottom; return this;}
inline Element* Element::setLeftMargin(int left) {marginLeft_ = left; return this;}
inline Element* Element::setRightMargin(int right) {marginRight_ = right; return this;}

inline QSize Element::size(Item* item) const {return getCache(item).size();}
inline void Element::setSize(Item* item, const QSize& size) {getCache(item).setSize(size);}

inline QPoint Element::pos(Item* item) const {return getCache(item).pos();}
inline void Element::setPos(Item* item, const QPoint& pos) {getCache(item).setPos(pos);}

inline int Element::topMargin() {return marginTop_;}
inline int Element::bottomMargin() {return marginBottom_;}
inline int Element::leftMargin() {return marginLeft_;}
inline int Element::rightMargin() {return marginRight_;}

}

#define FLUENT_ELEMENT_INTERFACE(ClassName)																									\
public:																																					\
	ClassName* setMargins(int left, int top, int right, int bottom) {																	\
		return static_cast<ClassName*>(Element::setMargins(left, top, right, bottom));											\
	}																																						\
	ClassName* setMargins(int margin) {return static_cast<ClassName*>(Element::setMargins(margin));}						\
	ClassName* setTopMargin(int top) {return static_cast<ClassName*>(Element::setTopMargin(top));}							\
	ClassName* setBottomMargin(int bottom) {return static_cast<ClassName*>(Element::setBottomMargin(bottom));}			\
	ClassName* setLeftMargin(int left) {return static_cast<ClassName*>(Element::setLeftMargin(left));}						\
	ClassName* setRightMargin(int right) {return static_cast<ClassName*>(Element::setRightMargin(right));}

#endif /* VisualizationBase_ELEMENT_H_ */
