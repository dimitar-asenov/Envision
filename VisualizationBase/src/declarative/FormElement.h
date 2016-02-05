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

namespace Visualization {

class Item;
class ItemRegion;
class DeclarativeItemBase;

/**
 * Class to cache information about an element, that varies form item to item.
 */
class VISUALIZATIONBASE_API ElementCache
{
	public:
		ElementCache() {};
		virtual ~ElementCache() {};
		/**
		 * Sets the size property of this element cache to the specified \a size.
		 */
		void setSize(const QSize& size);
		/**
		 * Sets the position property for this element cache to the specified \a position.
		 */
		void setPos(const QPoint& position);
		/**
		 * Returns the size property of this element cache.
		 */
		QSize size() const;
		/**
		 * Returns the position property of this element cache.
		 */
		QPoint pos() const;

	private:
		QSize size_{};
		QPoint pos_{}; // Position relative to the parent element
};

/**
 * Base class for all the form elements, that can be added as a form to a DeclarativeItem.
 *
 * It implements some basic behavior, such as a list of children, and margins.
 */
class VISUALIZATIONBASE_API FormElement
{
	public:
		FormElement() = default;
		FormElement(const FormElement& other);
		FormElement& operator=(const FormElement&) = delete;
		virtual ~FormElement();

		/**
		 * Copy this element and all its children into a new element tree.
		 */
		virtual FormElement* clone() const = 0;

		FormElement* cloneIfAlreadyUsed();

		/**
		 * Returns the list of shape elements inside this form element.
		 * Recursively collects the shape elements from the element's children.
		 * Method is called once after the static initialization of the visualization item type.
		 */
		virtual QList<FormElement*> shapeElements();

		// Methods executable on element definition
		/**
		 * Sets the \a left, \a top, \a right and \a bottom margins to the respective values.
		 * Returns a pointer to this FormElement.
		 */
		FormElement* setMargins(int left, int top, int right, int bottom);
		/**
		 * Sets all the margins of this form element to the value specified as \a margin.
		 * Returns a pointer to this FormElement.
		 */
		FormElement* setMargins(int margin);
		/**
		 * Sets the \a top margin of this form element.
		 * Returns a pointer to this FormElement.
		 */
		FormElement* setTopMargin(int top);
		/**
		 * Sets the \a bottom margin of this form element.
		 * Returns a pointer to this FormElement.
		 */
		FormElement* setBottomMargin(int bottom);
		/**
		 * Sets the \a left margin of this form element.
		 * Returns a pointer to this form Element.
		 */
		FormElement* setLeftMargin(int left);
		/**
		 * Sets the \a right margin of this form element.
		 * Returns a pointer to this FormElement.
		 */
		FormElement* setRightMargin(int right);

		/**
		 * Sets this element to be a formRoot
		 */
		FormElement* setFormRoot();

		// Methods executable when items need to be rendered
		/**
		 * Method called inside the \a item's determineChildren method to let the form element and it's children update
		 * their information about items and nodes belonging to this \a item.
		 *
		 * If any nodes or child items changed, this method allows the item wrappers and sequential layout to reflect
		 * those changes.
		 */
		virtual void synchronizeWithItem(Item* item);
		/**
		 * Method called inside the \a item's updateGeometry method to compute it's size and position it's child elements.
		 * The arguments \a availableWidth and \a availableHeight are similar to those of Item::updateGeometry. If they
		 * are zero, the form element calculates its minimum size, if it they are bigger, the element tries to
		 * stretch/grow to this size, but not further.
		 */
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) = 0;
		/**
		 * Method called inside the \a item's updateGeometry method to recursively position it's child items relative to
		 * the \a item's position. \a parentX and \a parentY are the offsets that this form element needs to have from the
		 * parent \a item. The element then puts its children at this offset plus the offset for its children relative to
		 * itself.
		 */
		virtual void setItemPositions(Item* item, int parentX=0, int parentY=0);
		/**
		 * Method called inside the \a item's sizeDependsOnParent method, and by some layout form elements. If the form
		 * element returns true it can be, that computeSize is called on it a second time with non-zero availableWidth and
		 * availableHeight.
		 */
		virtual bool sizeDependsOnParent(const Item* item) const = 0;
		/**
		 * Method called inside the \a item's elementOrChildHasFocus and by some layout form elements. It recursively
		 * determines if this form element or any of its children has the focus for the specified \a item.
		 */
		virtual bool elementOrChildHasFocus(Item* item) const;
		/**
		 * Method called inside the \a item's isEmpty method, and by some layout form elements. It recursively determines
		 * whether the form element is empty for the specified \a item.
		 */
		virtual bool isEmpty(const Item* item) const;
		/**
		 * Method called inside the \a item's regions method and returns cursor and item regions of all contained form
		 * elements for this \a item. \a parentX and \a parentY are the offsets that this element needs to have from the
		 * parent \a item (similar to the setItemPositions method).
		 */
		virtual QList<ItemRegion> regions(DeclarativeItemBase* item, int parentX=0, int parentY=0);
		/**
		 * Caches the \a position of this form element for the specified \a item.
		 */
		void setPos(Item* item, const QPoint& position);
		/**
		 * Returns the cached position of this form element for the specified \a item.
		 */
		QPoint pos(Item* item) const;
		/**
		 * Returns the cached size of this form element for the specified \a item.
		 */
		QSize size(Item* item) const;
		/**
		 * Returns the cached position on the x-axis of this form element for the specified \a item.
		 */
		int x(Item* item) const;
		/**
		 * Returns the cached position on the y-axis of this form element for the specified \a item.
		 */
		int y(Item* item) const;
		/**
		 * Returns the cached width of this form element for the specified \a item.
		 */
		int width(Item* item) const;
		/**
		 * Returns the cached height of this form element for the specified \a item.
		 */
		int height(Item* item) const;
		/**
		 * Returns the cached position on the x-axis where this form element ends for the specified \a item. That is the
		 * position on the x-axis + width.
		 */
		int xEnd(Item* item) const;
		/**
		 * Returns the cached position on the y-axis where this form element ends for the specified \a item. That is the
		 * position on the y-axis + height.
		 */
		int yEnd(Item* item) const;

		/**
		 * This method is recursively called on all form elements to destroy all the information they might have about a
		 * specific \a item.
		 *
		 * It is called in two cases:
		 *		- When the item is being destroyed. In that case all information and child items should be destroyed. The
		 * 	second argument is an empty list.
		 * 	- When the current form is changing. In that case only information pertaining to child items which are
		 * 	not definitely handled by the new form should be removed. The second argument is a list of all child item
		 * 	pointer handled by the new form.
		 */
		virtual void destroyChildItems(Item* item, QList<const Item* const DeclarativeItemBase::*> handledChildren);

		/**
		 * Returns a list of all pointer to child items of this form. This list should only be used for comparison
		 * as in destroyChildItems() and should not be used for writing.
		 */
		virtual QList<const Item* const DeclarativeItemBase::*> allHandledChildPointers();

		FormElement* parent() const;
		bool isFormRoot() const;

	protected:
		/**
		 * Caches the \a size of this form element for the specified \a item.
		 */
		void setSize(Item* item, const QSize& size);
		/**
		 * Returns the top margin.
		 */
		int topMargin();
		/**
		 * Returns the bottom margin.
		 */
		int bottomMargin();
		/**
		 * Returns the left margin.
		 */
		int leftMargin();
		/**
		 * Returns the right margin.
		 */
		int rightMargin();
		/**
		 * Adds a \a child to the list of children.
		 */
		void addChild(FormElement* child);
		/**
		 * Removes a \a child from the list of children.
		 */
		void removeChild(FormElement* child);
		/**
		 * Returns the list of children.
		 */
		const QVector<FormElement*>& children() const;

	private:

		// Do not forget to update the copy constructor if adding new members.
		bool isFormRoot_{};
		int marginTop_{};
		int marginBottom_{};
		int marginLeft_{};
		int marginRight_{};

		QVector<FormElement*> children_{};
		FormElement* parent_{};

		/**
		 * Returns the element property cache of this form element for a specific \a item.
		 */
		ElementCache& getCache(const Item* item) const;
		/**
		 * Clears the element property cache of this form element for a specific \a item.
		 */
		void clearCache(const Item* item);
		mutable QHash<const Item*, ElementCache*> elementCache_{};
};

inline void ElementCache::setSize(const QSize& size) {size_ = size;}
inline void ElementCache::setPos(const QPoint& position) {pos_ = position;}
inline QSize ElementCache::size() const {return size_;}
inline QPoint ElementCache::pos() const {return pos_;}


inline FormElement* FormElement::parent() const {return parent_;}

inline FormElement* FormElement::setMargins(int left, int top, int right, int bottom)
{
	marginLeft_ = left;
	marginTop_ = top;
	marginRight_ = right;
	marginBottom_ = bottom;
	return this;
}
inline FormElement* FormElement::setMargins(int margin)
{
	marginLeft_ = margin;
	marginTop_ = margin;
	marginRight_ = margin;
	marginBottom_ = margin;
	return this;
}
inline FormElement* FormElement::setTopMargin(int top) {marginTop_ = top; return this;}
inline FormElement* FormElement::setBottomMargin(int bottom) {marginBottom_ = bottom; return this;}
inline FormElement* FormElement::setLeftMargin(int left) {marginLeft_ = left; return this;}
inline FormElement* FormElement::setRightMargin(int right) {marginRight_ = right; return this;}

inline QSize FormElement::size(Item* item) const {return getCache(item).size();}
inline void FormElement::setSize(Item* item, const QSize& size) {getCache(item).setSize(size);}
inline QPoint FormElement::pos(Item* item) const {return getCache(item).pos();}
inline void FormElement::setPos(Item* item, const QPoint& position) {getCache(item).setPos(position);}

inline int FormElement::x(Item* item) const {return pos(item).x();}
inline int FormElement::y(Item* item) const {return pos(item).y();}
inline int FormElement::width(Item* item) const {return size(item).width();}
inline int FormElement::height(Item* item) const {return size(item).height();}
inline int FormElement::xEnd(Item* item) const {return pos(item).x() + size(item).width();}
inline int FormElement::yEnd(Item* item) const {return pos(item).y() + size(item).height();}

inline int FormElement::topMargin() {return marginTop_;}
inline int FormElement::bottomMargin() {return marginBottom_;}
inline int FormElement::leftMargin() {return marginLeft_;}
inline int FormElement::rightMargin() {return marginRight_;}
inline bool FormElement::isFormRoot() const {return isFormRoot_;}

inline const QVector<FormElement*>& FormElement::children() const {return children_;}

template<typename NewFormElement, typename BaseFormElement>
class SuperFormElement : public BaseFormElement
{
	public:
		using BaseFormElement::BaseFormElement;

		/**
		 * Sets the \a left, \a top, \a right and \a bottom margins to the respective values.
		 * Returns a pointer to this form element.
		 */
		NewFormElement* setMargins(int left, int top, int right, int bottom) {
			return static_cast<NewFormElement*>(BaseFormElement::setMargins(left, top, right, bottom));
		}
		/**
		 * Sets all the margins of this form element to the value specified as \a margin.
		 * Returns a pointer to this form element.
		 */
		NewFormElement* setMargins(int margin) {
			return static_cast<NewFormElement*>(BaseFormElement::setMargins(margin));
		}
		/**
		 * Sets the \a top margin of this form element.
		 * Returns a pointer to this form element.
		 */
		NewFormElement* setTopMargin(int top)
		{
			return static_cast<NewFormElement*>(BaseFormElement::setTopMargin(top));
		}
		/**
		 * Sets the \a bottom margin of this form element.
		 * Returns a pointer to this form element.
		 */
		NewFormElement* setBottomMargin(int bottom)
		{
			return static_cast<NewFormElement*>(BaseFormElement::setBottomMargin(bottom));
		}
		/**
		 * Sets the \a left margin of this form element.
		 * Returns a pointer to this form element.
		 */
		NewFormElement* setLeftMargin(int left)
		{
			return static_cast<NewFormElement*>(BaseFormElement::setLeftMargin(left));
		}
		/**
		 * Sets the \a right margin of this form element.
		 * Returns a pointer to this form element.
		 */
		NewFormElement* setRightMargin(int right)
		{
			return static_cast<NewFormElement*>(BaseFormElement::setRightMargin(right));
		}
};

}
