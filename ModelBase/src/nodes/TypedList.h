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

#include "List.h"

namespace Model {

class Node;

template <typename T>
class TypedList: public Super<List>
{
	NODE_DECLARE_STANDARD_METHODS(TypedList)

	public:
		T* first() const;
		T* last() const;
		T* at(int i) const;

		virtual bool replaceChild(Node* child, Node* replacement) override;

		virtual Node* createDefaultElement() override;
		virtual int lowerTypeBoundForElements() const override;

		/**
		 * Iterator
		 */
		struct TypedIterator : public std::iterator<std::bidirectional_iterator_tag, T*> {
			TypedIterator() = default;
			TypedIterator(const TypedIterator& other) = default;
			TypedIterator(List::iterator it);
			TypedIterator& operator=(const TypedIterator&) = default;

			TypedIterator& operator++();
			TypedIterator operator++(int);
			TypedIterator& operator--();
			TypedIterator operator--(int);
			bool operator==(const TypedIterator& other) const;
			bool operator!=(const TypedIterator& other) const;
			T* & operator*() const;
			T*   operator->() const;

			List::iterator it_{};
		};

		struct ConstTypedIterator : public std::iterator<std::bidirectional_iterator_tag, const T*> {
			ConstTypedIterator() = default;
			ConstTypedIterator(const ConstTypedIterator& other) = default;
			ConstTypedIterator(List::const_iterator it);
			ConstTypedIterator& operator=(const ConstTypedIterator&) = default;
			ConstTypedIterator(TypedIterator& it);

			ConstTypedIterator& operator++();
			ConstTypedIterator operator++(int);
			ConstTypedIterator& operator--();
			ConstTypedIterator operator--(int);
			bool operator==(const ConstTypedIterator& other) const;
			bool operator!=(const ConstTypedIterator& other) const;
			T* const& operator*() const;
			T* operator->() const;

			List::const_iterator it_{};
		};

		using iterator = TypedIterator;
		using const_iterator = ConstTypedIterator;

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;
		iterator end();
		const_iterator end() const;
		const_iterator cend() const;
};

template <typename T>
inline typename TypedList<T>::const_iterator TypedList<T>::begin() const { return ConstTypedIterator{List::cbegin()}; }
template <typename T>
inline typename TypedList<T>::const_iterator TypedList<T>::cbegin() const { return ConstTypedIterator{List::cbegin()}; }
template <typename T>
inline typename TypedList<T>::const_iterator TypedList<T>::cend() const { return ConstTypedIterator{List::cend()}; }
template <typename T>
inline typename TypedList<T>::const_iterator TypedList<T>::end() const { return ConstTypedIterator{List::cend()}; }
template <typename T>
inline typename TypedList<T>::iterator TypedList<T>::begin() { return TypedIterator{List::begin()}; }
template <typename T>
inline typename TypedList<T>::iterator TypedList<T>::end() { return TypedIterator{List::end()}; }

template<typename T>
inline TypedList<T>::TypedIterator::TypedIterator(List::iterator it): it_{it}{}

template<typename T>
inline typename TypedList<T>::TypedIterator TypedList<T>::TypedIterator::operator++(int)
{
	TypedIterator old{*this};
	++it_;
	return old;
}

template<typename T>
inline typename TypedList<T>::TypedIterator TypedList<T>::TypedIterator::operator--(int)
{
	TypedIterator old{*this};
	--it_;
	return old;
}

template<typename T>
inline bool TypedList<T>::TypedIterator::operator==(const TypedIterator& other) const
{
	return it_ == other.it_;
}

template<typename T>
inline bool TypedList<T>::TypedIterator::operator!=(const TypedIterator& other) const
{
	return it_ != other.it_;
}

template<typename T>
inline T*& TypedList<T>::TypedIterator::operator*() const
{
	return reinterpret_cast<T*&>(*it_);
}

template<typename T>
inline T* TypedList<T>::TypedIterator::operator->() const
{
	return reinterpret_cast<T*&>(*it_);
}

template<typename T>
inline typename TypedList<T>::TypedIterator& TypedList<T>::TypedIterator::operator--()
{
	--it_;
	return *this;
}

template<typename T>
inline typename TypedList<T>::TypedIterator& TypedList<T>::TypedIterator::operator++()
{
	++it_;
	return *this;
}

template<typename T>
inline TypedList<T>::ConstTypedIterator::ConstTypedIterator(List::const_iterator it): it_{it}{}

template<typename T>
inline TypedList<T>::ConstTypedIterator::ConstTypedIterator(TypedIterator& it) : it_{it.it_}{}

template<typename T>
inline typename TypedList<T>::ConstTypedIterator TypedList<T>::ConstTypedIterator::operator++(int)
{
	ConstTypedIterator old{*this};
	++it_;
	return old;
}

template<typename T>
inline typename TypedList<T>::ConstTypedIterator TypedList<T>::ConstTypedIterator::operator--(int)
{
	ConstTypedIterator old{*this};
	--it_;
	return old;
}

template<typename T>
inline bool TypedList<T>::ConstTypedIterator::operator==(const ConstTypedIterator& other) const
{
	return it_ == other.it_;
}

template<typename T>
inline bool TypedList<T>::ConstTypedIterator::operator!=(const ConstTypedIterator& other) const
{
	return it_ != other.it_;
}

template<typename T>
inline T* const& TypedList<T>::ConstTypedIterator::operator*() const
{
	return reinterpret_cast<T* const &>(*it_);
}

template<typename T>
inline T* TypedList<T>::ConstTypedIterator::operator->() const
{
	return reinterpret_cast<T* const &>(*it_);
}

template<typename T>
inline typename TypedList<T>::ConstTypedIterator& TypedList<T>::ConstTypedIterator::operator--()
{
	--it_;
	return *this;
}

template<typename T>
inline typename TypedList<T>::ConstTypedIterator& TypedList<T>::ConstTypedIterator::operator++()
{
	++it_;
	return *this;
}

}
