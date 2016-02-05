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
#include "FormElement.h"

namespace Visualization {

/**
 * Base class for a layout element.
 */
class VISUALIZATIONBASE_API LayoutFormElement : public FormElement
{
	public:
		LayoutFormElement() = default;
		LayoutFormElement(const LayoutFormElement&) = default;
		LayoutFormElement& operator=(const LayoutFormElement&) = delete;
		~LayoutFormElement() {};
		/**
		 * Sets a getter (\a hasCursorWhenEmptyGetter) for checking if the element should have cursors when empty
		 * dynamically while rendering.
		 */
		LayoutFormElement* setHasCursorWhenEmpty(std::function<bool(Item* item)> hasCursorWhenEmptyGetter);
		/**
		 * Sets a getter (\a notLocationEquivalentCursorsGetter) for checking if the element's cursors are not location
		 * equivalent dynamically while rendering.
		 */
		LayoutFormElement* setNotLocationEquivalentCursors(
				std::function<bool(Item* item)> notLocationEquivalentCursorsGetter);
		/**
		 * Sets a getter (\a noBoundaryCursorsGetter) for checking if the element should not have any boundary cursors
		 * dynamically while rendering.
		 */
		LayoutFormElement* setNoBoundaryCursors(std::function<bool(Item* item)> noBoundaryCursorsGetter);
		/**
		 * Sets a getter (\a noInnerCursorsGetter) for checking if the element should not have any inner cursors
		 * dynamically while rendering.
		 */
		LayoutFormElement* setNoInnerCursors(std::function<bool(Item* item)> noInnerCursorsGetter);

		bool hasCursorWhenEmpty(Item* item);
		bool notLocationEquivalentCursors(Item* item);
		bool noBoundaryCursors(Item* item);
		bool noInnerCursors(Item* item);

	private:

		// Do not forget to update the copy constructor if adding new members.
		std::function<bool(Item* item)> hasCursorWhenEmptyGetter_{};
		std::function<bool(Item* item)> notLocationEquivalentCursorsGetter_{};
		std::function<bool(Item* item)> noBoundaryCursorsGetter_{};
		std::function<bool(Item* item)> noInnerCursorsGetter_{};
};
inline LayoutFormElement* LayoutFormElement::setHasCursorWhenEmpty(
		std::function<bool(Item* item)> hasCursorWhenEmptyGetter)
{
	hasCursorWhenEmptyGetter_ = hasCursorWhenEmptyGetter;
	return this;
}
inline LayoutFormElement* LayoutFormElement::setNotLocationEquivalentCursors(
		std::function<bool(Item* item)> notLocationEquivalentCursorsGetter)
{
	notLocationEquivalentCursorsGetter_ = notLocationEquivalentCursorsGetter;
	return this;
}
inline LayoutFormElement*
	LayoutFormElement::setNoBoundaryCursors(std::function<bool(Item* item)> noBoundaryCursorsGetter)
{
	noBoundaryCursorsGetter_ = noBoundaryCursorsGetter;
	return this;
}
inline LayoutFormElement* LayoutFormElement::setNoInnerCursors(std::function<bool(Item* item)> noInnerCursorsGetter)
{
	noInnerCursorsGetter_ = noInnerCursorsGetter;
	return this;
}
inline bool LayoutFormElement::hasCursorWhenEmpty(Item* item)
{
	if (hasCursorWhenEmptyGetter_) return hasCursorWhenEmptyGetter_(item);
	return true;
}
inline bool LayoutFormElement::notLocationEquivalentCursors(Item* item)
{
	if (notLocationEquivalentCursorsGetter_) return notLocationEquivalentCursorsGetter_(item);
	return false;
}
inline bool LayoutFormElement::noBoundaryCursors(Item* item)
{
	if (noBoundaryCursorsGetter_) return noBoundaryCursorsGetter_(item);
	return false;
}
inline bool LayoutFormElement::noInnerCursors(Item* item)
{
	if (noInnerCursorsGetter_) return noInnerCursorsGetter_(item);
	return false;
}

template<typename NewLayoutElement, typename BaseLayoutElement>
class SuperLayoutElement : public SuperFormElement<NewLayoutElement, BaseLayoutElement>
{
	public:
		using SuperFormElement<NewLayoutElement, BaseLayoutElement>::SuperFormElement;
		/**
		 * Sets a getter (\a cursorWhenEmptyGetter) for checking if the element should have cursors when empty
		 * dynamically while rendering.
		 */
		NewLayoutElement* setHasCursorWhenEmpty(std::function<bool(Item* item)> hasCursorWhenEmptyGetter)
		{
			 return static_cast<NewLayoutElement*>(BaseLayoutElement::setHasCursorWhenEmpty(hasCursorWhenEmptyGetter));
		 }
		/**
		 * Sets a getter (\a notLocationEquivalentGetter) for checking if the element's cursors are not location
		 * equivalent dynamically while rendering.
		 */
		NewLayoutElement* setNotLocationEquivalentCursors(
				std::function<bool(Item* item)> notLocationEquivalentCursorsGetter)
		{
			 return static_cast<NewLayoutElement*>(
					 BaseLayoutElement::setNotLocationEquivalentCursors(notLocationEquivalentCursorsGetter));
		 }
		/**
		 * Sets a getter (\a noBoundaryCursorsGetter) for checking if the element should not have any boundary cursors
		 * dynamically while rendering.
		 */
		NewLayoutElement* setNoBoundaryCursors(std::function<bool(Item* item)> noBoundaryCursorsGetter)
		{
			 return static_cast<NewLayoutElement*>(BaseLayoutElement::setNoBoundaryCursors(noBoundaryCursorsGetter));
		 }
		/**
		 * Sets a getter (\a noInnerCursorsGetter) for checking if the element should not have any inner cursors
		 * dynamically while rendering.
		 */
		NewLayoutElement* setNoInnerCursors(std::function<bool(Item* item)> noInnerCursorsGetter)
		{
			 return static_cast<NewLayoutElement*>(BaseLayoutElement::setNoInnerCursors(noInnerCursorsGetter));
		 }
};

}
