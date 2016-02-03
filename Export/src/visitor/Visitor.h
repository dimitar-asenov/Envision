/***********************************************************************************************************************
**
** Copyright (c) 2011, 2016 ETH Zurich
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

#include "../export_api.h"
#include "VisitorData.h"
#include "../ExportError.h"
#include "../tree/CompositeFragment.h"

#include "ModelBase/src/nodes/TypedList.h"

namespace Export {

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
class Visitor
{
	public:
		Visitor();
		Visitor(std::shared_ptr<VisitorData> data);
		Visitor(PrintContext printContext);
		Visitor(PrintContext printContext, std::shared_ptr<VisitorData> data);
		~Visitor();

		QList<ExportError> errors() const;

	protected:
		std::shared_ptr<VisitorData> data();

		void required(Model::Node* parent, Model::Node* node, const QString& childName);
		void notAllowed(Model::Node* node);
		void notAllowed(Model::List* list);

		void error(const QString& errorMessage);
		void error(Model::Node* node, const QString& errorMessage);

		template<typename ListElement, typename VisitorClass, typename Predicate = bool (*)(ListElement*)>
		CompositeFragment* list(Model::TypedList<ListElement>* list, VisitorClass* v,
												  const QString& fragmentType = QString(), Predicate filter = nullptr);
		template<typename ListElement, typename VisitorClass, typename Predicate = bool (*)(ListElement*)>
		CompositeFragment* list(Model::TypedList<ListElement>* list, VisitorClass&& v,
												  const QString& fragmentType = QString(), Predicate filter = nullptr);

		template <typename NodeType> SourceFragment* declaration(NodeType* node);
		template <typename NodeType> SourceFragment* statement(NodeType* node);
		template <typename NodeType> SourceFragment* expression(NodeType* node);
		template <typename NodeType> SourceFragment* element(NodeType* node);

	private:
		std::shared_ptr<VisitorData> data_;
};


template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
inline std::shared_ptr<VisitorData>
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>::data() { return data_; }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
void Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::required(Model::Node* parent, Model::Node* node, const QString& childName)
{
	if (node) return;
	error(parent, "A required child (" + childName + ") is missing.");
}

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
void Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>::notAllowed(Model::Node* node)
{
	if (!node) return;

	if (auto list = DCast<Model::List>(node)) notAllowed(list);
	else error(node, "Node not allowed");
}

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
void Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>::notAllowed(Model::List* list)
{ if (list && !list->isEmpty()) error(list, "List must be empty"); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>::Visitor() : data_{new VisitorData}{}

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::Visitor(std::shared_ptr<VisitorData> data) : data_{data}
{ if (!data_.get()->printContextStack_.empty())
		data_.get()->printContextStack_.append(data_.get()->printContextStack_.last()); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::Visitor(PrintContext printContext) : Visitor{} { data_.get()->printContextStack_.append(printContext); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::Visitor(PrintContext printContext, std::shared_ptr<VisitorData> data) : Visitor{data}
{ data_.get()->printContextStack_.append(printContext); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::~Visitor() { if (!data_.get()->printContextStack_.empty()) data_.get()->printContextStack_.pop_back(); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
inline QList<ExportError> Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::errors() const { return data_->errors_; }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
inline void Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::error(const QString& errorMessage) { data_->errors_.append(ExportError(errorMessage)); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
inline void Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::error(Model::Node* node, const QString& errorMessage)
{ data_->errors_.append(ExportError(node, errorMessage)); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
template<typename ListElement, typename VisitorClass, typename Predicate>
CompositeFragment* Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::list(Model::TypedList<ListElement>* list, VisitorClass* v, const QString& fragmentType, Predicate filter)
{
	auto fragment = new CompositeFragment{list, fragmentType};
	for (auto node : *list)
		if (!filter || filter(node))
			*fragment << v->visit(node);
	return fragment;
}

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
template<typename ListElement, typename VisitorClass, typename Predicate>
inline CompositeFragment* Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
::list(Model::TypedList<ListElement>* list, VisitorClass&& v, const QString& fragmentType, Predicate filter)
{
	return Visitor::list(list, &v, fragmentType, filter);
}

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
template <typename NodeType> inline SourceFragment*
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>::declaration(NodeType* node)
{ return DeclarationVisitor(data_).visit(node); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
template <typename NodeType> inline SourceFragment*
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>::statement(NodeType* node)
{ return StatementVisitor(data_).visit(node); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
template <typename NodeType> inline SourceFragment*
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>::expression(NodeType* node)
{ return ExpressionVisitor(data_).visit(node); }

template<typename DeclarationVisitor, typename ExpressionVisitor, typename StatementVisitor, typename ElementVisitor>
template <typename NodeType> inline SourceFragment*
Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>::element(NodeType* node)
{ return ElementVisitor(data_).visit(node); }

}
