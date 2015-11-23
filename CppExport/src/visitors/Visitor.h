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

#include "../cppexport_api.h"
#include "../exporter/ExportError.h"

#include "OOModel/src/allOOModelNodes.h"
#include "Export/src/tree/CompositeFragment.h"

namespace CppExport {

struct VisitorData
{
		QList<ExportError> errors_;
		QStringList packageStack_;
};

class Visitor
{
	public:
		Visitor();
		Visitor(std::shared_ptr<VisitorData> data);

		QList<ExportError> errors() const;

	protected:
		std::shared_ptr<VisitorData> data();

		QStringList& packageStack();
		QString packagesSoFar() const;

		void required(Model::Node* parent, Model::Node* node, const QString& childName);
		void notAllowed(Model::Node* node);
		void notAllowed(Model::List* list);

		void error(const QString& errorMessage);
		void error(Model::Node* node, const QString& errorMessage);

		template<class ListElement, class VisitorClass>
		Export::SourceFragment* list(Model::TypedList<ListElement>* list, VisitorClass* v,
											  const QString& fragmentType = QString());
		template<class ListElement, class VisitorClass>
		Export::SourceFragment* list(Model::TypedList<ListElement>* list, VisitorClass&& v,
											  const QString& fragmentType = QString());

		template<class ListElement, class VisitorClass, typename Predicate>
		Export::SourceFragment* listWhere(Model::TypedList<ListElement>* list, VisitorClass* v,
											  Predicate predicate, const QString& fragmentType = QString(),
												bool* found = nullptr);
		template<class ListElement, class VisitorClass, typename Predicate>
		Export::SourceFragment* listWhere(Model::TypedList<ListElement>* list, VisitorClass&& v,
											  Predicate predicate, const QString& fragmentType = QString(),
												bool* found = nullptr);

		template <class NodeType> Export::SourceFragment* declaration(NodeType* node);
		template <class NodeType> Export::SourceFragment* statement(NodeType* node);
		template <class NodeType> Export::SourceFragment* expression(NodeType* node);
		template <class NodeType> Export::SourceFragment* element(NodeType* node);

	private:
		std::shared_ptr<VisitorData> data_;
};

inline std::shared_ptr<VisitorData> Visitor::data() { return data_; }

inline QStringList& Visitor::packageStack() { return data_->packageStack_; }
inline QList<ExportError> Visitor::errors() const { return data_->errors_; }
inline QString Visitor::packagesSoFar() const { return data_->packageStack_.join("."); }

inline void Visitor::error(const QString& errorMessage) { data_->errors_.append(ExportError(errorMessage)); }
inline void Visitor::error(Model::Node* node, const QString& errorMessage)
{ data_->errors_.append(ExportError(node, errorMessage)); }

template<class ListElement, class VisitorClass>
Export::SourceFragment* Visitor::list(Model::TypedList<ListElement>* list, VisitorClass* v, const QString& fragmentType)
{
	auto fragment = new Export::CompositeFragment(list, fragmentType);
	for (auto node : *list) *fragment << v->visit(node);
	return fragment;
}

template<class ListElement, class VisitorClass>
inline Export::SourceFragment* Visitor::list(Model::TypedList<ListElement>* list, VisitorClass&& v,
												  const QString& fragmentType)
{
	return Visitor::list(list, &v, fragmentType);
}

template<class ListElement, class VisitorClass, typename Predicate>
Export::SourceFragment* Visitor::listWhere(Model::TypedList<ListElement>* list, VisitorClass* v,
												  Predicate predicate, const QString& fragmentType,
														 bool* found)
{
	auto fragment = new Export::CompositeFragment(list, fragmentType);
	for (auto node : *list)
		if (predicate(node))
		{
			*fragment << v->visit(node);
			if (found) *found = true;
		}
	return fragment;
}

template<class ListElement, class VisitorClass, typename Predicate>
inline Export::SourceFragment* Visitor::listWhere(Model::TypedList<ListElement>* list, VisitorClass&& v,
															Predicate predicate, const QString& fragmentType,
																  bool* found)
{
	return Visitor::list(list, &v, predicate, fragmentType, found);
}

}
