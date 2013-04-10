/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#pragma once

#include "../oointeraction_api.h"
#include "../OOInteractionException.h"

namespace Model {
	class Node;
	class List;
}

namespace OOInteraction {

class OOINTERACTION_API StringComponents {
	public:
		StringComponents(Model::Node* node = nullptr);
		virtual ~StringComponents();
		virtual QStringList components();

		//Helper functions
		static QString stringForNode(Model::Node* node);
		static QStringList componentsForNode(Model::Node* node);

		template <class T> static void add(QStringList(*f)(T* node));

		static void initConversions();

	protected:
		enum UseOptional { USE, AUTO, DO_NOT_USE };
		struct Optional {
			Optional(QStringList list) : list(list){}
			Optional(const char* str) : list(str){}
			Optional(QString str) : list(str){}
			Optional(QString str, bool use) : list(str), use(use){}
			Optional(QChar ch) : list(ch){}
			Optional(Model::Node* node) : list{stringForNode(node)} {}
			Optional(Model::Node* node, UseOptional)
			: list{ use==USE || (use==AUTO && node) ? stringForNode(node) : QString() },
			  use { use==USE || (use==AUTO && node && !list.isEmpty() && !list.first().isEmpty())} {}
			Optional() : use(false){}
			QStringList list;
			bool use{true};
		};

		static Optional list(Model::List* listNode, const QString& prefix, const QString& separator,
				const QString& postfix, bool nothingIfEmpty, bool collapse);
		static Optional list(Model::List* listNode);

		// TODO: Check completeness statically.
		template <class E> static Optional choose(E value);
		template <class E, class ...Args> static Optional choose(E value, E option, Optional str, Args... args);

		template <class ...Args> static QStringList c(Args... args);

	private:
		Model::Node* node_;

		static QMap<int, QStringList(*)(Model::Node* node)>& componentFunctions();

		static void c_helper(QStringList&);
		template<class ...Args> static void c_helper(QStringList& result, Optional str, Args... args);
};

template <class T>
inline void StringComponents::add(QStringList(*f)(T* node))
{
	componentFunctions().insert(T::typeIdStatic(), reinterpret_cast<QStringList(*)(Model::Node* node)>(f));
}

template <class E>
StringComponents::Optional StringComponents::choose(E value)
{
	throw OOInteractionException("No matching choice variable " + QString::number(value)
		+ " when computing string components.");
}

template <class E, class ...Args>
StringComponents::Optional StringComponents::choose(E value, E option, Optional str, Args... args)
{
	if (value == option) return str;
	return choose(value, args...);
}

template <class ...Args>
QStringList StringComponents::c(Args... args)
{
	QStringList result;
	//if (!exp_) return result;

	c_helper(result, args...);
	return result;
}

inline void StringComponents::c_helper(QStringList&)
{}

template<class ...Args>
void StringComponents::c_helper(QStringList& result, Optional str, Args... args)
{
	if (str.use) result << str.list;
	c_helper(result, args...);
}

} /* namespace OOInteraction */
