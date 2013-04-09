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

/**
 * Class to cache information about an element, that varies form item to item.
 */
class ElementCache
{
	public:
		ElementCache() {};
		virtual ~ElementCache() {};
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
		virtual void synchronizeWithItem(Item* item);
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) = 0;
		virtual void setItemPositions(Item* item, int parentX=0, int parentY=0);
		virtual bool sizeDependsOnParent(const Item* item) const = 0;
		virtual QList<ItemRegion> regions(Item* item, int parentX=0, int parentY=0) = 0;
		void setPos(Item* item, const QPoint& pos);
		QPoint pos(Item* item) const;
		QSize size(Item* item) const;
		int x(Item* item) const;
		int y(Item* item) const;
		int width(Item* item) const;
		int height(Item* item) const;
		int xEnd(Item* item) const;
		int yEnd(Item* item) const;
		virtual bool elementOrChildHasFocus(Item* item) const;

	public: // Recursive item destruction
		virtual void destroyChildItems(Item* item);

	protected:
		void setSize(Item* item, const QSize& size);
		int topMargin();
		int bottomMargin();
		int leftMargin();
		int rightMargin();
		void addChild(Element* child);
		void removeChild(Element* child);
		const QVector<Element*>& children() const;
		Element* parent();

	private:
		int marginTop_{};
		int marginBottom_{};
		int marginLeft_{};
		int marginRight_{};

		QVector<Element*> children_{};
		Element* parent_{};

		ElementCache& getCache(const Item* item) const;
		void clearCache(const Item* item);
		mutable QHash<const Item*, ElementCache*> elementCache_{};
};

/**
 * Sets the size property of this element cache.
 * @param size The size to set the property to
 */
inline void ElementCache::setSize(const QSize& size) {size_ = size;}
/**
 * Sets the position property for this element cache.
 * @param pos The position to set the property to
 */
inline void ElementCache::setPos(const QPoint& pos) {pos_ = pos;}
/**
 * Gets the size property of this element cache.
 * @return The cached size
 */
inline QSize ElementCache::size() const {return size_;}
/**
 * Gets the position property of this element cache.
 * @return The cached position
 */
inline QPoint ElementCache::pos() const {return pos_;}

/**
 * Sets all the margins of this element
 * @param left Left margin
 * @param top Top margin
 * @param right Right margin
 * @param bottom Bottom margin
 * @return A pointer to this Element
 */
inline Element* Element::setMargins(int left, int top, int right, int bottom)
{
	marginLeft_ = left;
	marginTop_ = top;
	marginRight_ = right;
	marginBottom_ = bottom;
	return this;
}
/**
 * Sets all the margins of this element
 * @param margin Margin for all four sides
 * @return A pointer to this Element
 */
inline Element* Element::setMargins(int margin)
{
	marginLeft_ = margin;
	marginTop_ = margin;
	marginRight_ = margin;
	marginBottom_ = margin;
	return this;
}
/**
 * Sets the top margin of this element
 * @param top Top margin
 * @return A pointer to this Element
 */
inline Element* Element::setTopMargin(int top) {marginTop_ = top; return this;}
/**
 * Sets the bottom margin of this element
 * @param bottom Bottom margin
 * @return A pointer to this Element
 */
inline Element* Element::setBottomMargin(int bottom) {marginBottom_ = bottom; return this;}
/**
 * Sets the left margin of this element
 * @param left Left margin
 * @return A pointer to this Element
 */
inline Element* Element::setLeftMargin(int left) {marginLeft_ = left; return this;}
/**
 * Sets the right margin of this element
 * @param right Right margin
 * @return A pointer to this Element
 */
inline Element* Element::setRightMargin(int right) {marginRight_ = right; return this;}

/**
 * This method is called while rendering the item.
 *
 * Gets the cached size of this element for the specified item.
 * @param item The item for which to get the size of the element
 * @return The size of this element, as cached for the specified item
 */
inline QSize Element::size(Item* item) const {return getCache(item).size();}
/**
 * This method is called while rendering the item.
 *
 * Caches the size of this element for the specified item.
 * @param item Item for which to cache the element's size
 * @param size The size to cache for this element
 */
inline void Element::setSize(Item* item, const QSize& size) {getCache(item).setSize(size);}

/**
 * This method is called while rendering the item.
 *
 * Gets the cached position of this element for the specified item.
 * @param item The item for which to get the position of the element
 * @return The position of this element, as cached for the specified item
 */
inline QPoint Element::pos(Item* item) const {return getCache(item).pos();}
/**
 * This method is called while rendering the item.
 *
 * Caches the position of this element for the specified item.
 * @param item Item for which to cache the element's position
 * @param size The position to cache for this element
 */
inline void Element::setPos(Item* item, const QPoint& pos) {getCache(item).setPos(pos);}

/**
 * Gets the top margin
 * @return Top margin
 */
inline int Element::topMargin() {return marginTop_;}
/**
 * Gets the bottom margin
 * @return Bottom margin
 */
inline int Element::bottomMargin() {return marginBottom_;}
/**
 * Gets the left margin
 * @return Left margin
 */
inline int Element::leftMargin() {return marginLeft_;}
/**
 * Gets the right margin
 * @return Right margin
 */
inline int Element::rightMargin() {return marginRight_;}

/**
 * This method is called while rendering the item.
 *
 * Gets the cached position on x-axis of this element for the specified item.
 * @param item The item for which to get the position of the element
 * @return The position on the x-axis of this element, as cached for the specified item
 */
inline int Element::x(Item* item) const {return pos(item).x();}
/**
 * This method is called while rendering the item.
 *
 * Gets the cached position on y-axis of this element for the specified item.
 * @param item The item for which to get the position of the element
 * @return The position on the y-axis of this element, as cached for the specified item
 */
inline int Element::y(Item* item) const {return pos(item).y();}
/**
 * This method is called while rendering the item.
 *
 * Gets the cached width of this element for the specified item.
 * @param item The item for which to get the width of the element
 * @return The width of this element, as cached for the specified item
 */
inline int Element::width(Item* item) const {return size(item).width();}
/**
 * This method is called while rendering the item.
 *
 * Gets the cached height of this element for the specified item.
 * @param item The item for which to get the height of the element
 * @return The height of this element, as cached for the specified item
 */
inline int Element::height(Item* item) const {return size(item).height();}
/**
 * This method is called while rendering the item.
 *
 * Gets the cached position on x-axis where this element ends for the specified item. That is position on x-axis + width.
 * @param item The item for which to get the position of the element
 * @return The position on the x-axis of this element, as cached for the specified item
 */
inline int Element::xEnd(Item* item) const {return pos(item).x() + size(item).width();}
/**
 * This method is called while rendering the item.
 *
 * Gets the cached position on y-axis where this element ends for the specified item. That is position on y-axis + height.
 * @param item The item for which to get the position of the element
 * @return The position on the y-axis of this element, as cached for the specified item
 */
inline int Element::yEnd(Item* item) const {return pos(item).y() + size(item).height();}

/**
 * Adds a child to the list of children.
 * @param child The child element to add
 */
inline void Element::addChild(Element* child){children_.append(child); child->parent_ = this;}
/**
 * Removes a child to the list of children.
 * @param child The child element to remove
 */
inline void Element::removeChild(Element* child)
{
	if (child)
		for (int i=0; i<children_.size(); ++i)
			if (child == children_[i]) children_.remove(i);
}
/**
 * Returns the list of children.
 * @return child The list of child elements
 */
inline const QVector<Element*>& Element::children() const {return children_;}

}

#define FLUENT_ELEMENT_INTERFACE(ClassName)																									\
public:																																					\
	/**
	 * Sets all the margins of this element
	 * @param left Left margin
	 * @param top Top margin
	 * @param right Right margin
	 * @param bottom Bottom margin
	 * @return A pointer to this element
	 */																																					\
	ClassName* setMargins(int left, int top, int right, int bottom) {																	\
		return static_cast<ClassName*>(Element::setMargins(left, top, right, bottom));											\
	}																																						\
	/**
	 * Sets all the margins of this element
	 * @param margin Margin for all four sides
	 * @return A pointer to this element
	 */																																					\
	ClassName* setMargins(int margin) {return static_cast<ClassName*>(Element::setMargins(margin));}						\
	/**
	 * Sets the top margin of this element
	 * @param top Top margin
	 * @return A pointer to this element
	 */																																					\
	ClassName* setTopMargin(int top) {return static_cast<ClassName*>(Element::setTopMargin(top));}							\
	/**
	 * Sets the bottom margin of this element
	 * @param bottom Bottom margin
	 * @return A pointer to this element
	 */																																					\
	ClassName* setBottomMargin(int bottom) {return static_cast<ClassName*>(Element::setBottomMargin(bottom));}			\
	/**
	 * Sets the left margin of this element
	 * @param left Left margin
	 * @return A pointer to this element
	 */																																					\
	ClassName* setLeftMargin(int left) {return static_cast<ClassName*>(Element::setLeftMargin(left));}						\
	/**
	 * Sets the right margin of this element
	 * @param right Right margin
	 * @return A pointer to this element
	 */																																					\
	ClassName* setRightMargin(int right) {return static_cast<ClassName*>(Element::setRightMargin(right));}

#endif /* VisualizationBase_ELEMENT_H_ */
