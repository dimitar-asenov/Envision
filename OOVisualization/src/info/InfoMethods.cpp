/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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
#include "InfoMethods.h"

#include "OOModel/src/allOOModelNodes.h"

namespace OOVisualization {

QString InfoMethods::numberOfCallees(Model::Node *node)
{
	if (auto method = DCast<OOModel::Method>(node))
	{
		QString html = expandButton("toggleCalleeTable", "calleeTable");
		html = "Number of called methods: " + QString::number(method->callees().size()) + "<br>" + html;
		html += "<table id=\"calleeTable\" style=\"display:none\">";
		html += "<tr><td style=\"border-right:solid 1px\"><b>Class</b></td><td><b>Method</b></td></tr>";
		for (auto callee : method->callees())
		{
			auto methodClass = callee->firstAncestorOfType<OOModel::Class>();
			html = html + "<tr><td style=\"border-right:solid 1px\">" + (methodClass ? methodClass->name() : "n/a") + "</td>";
			html = html + "<td>" + callee->name() + "</td></tr>";
		}
		html += "</table>";
		return html;
	}
	else return QString();
}

QString InfoMethods::numberOfUsages(Model::Node *node)
{

	if (auto method = DCast<OOModel::Method>(node))
	{
		QString html = expandButton("toggleCallerTable", "callerTable");
		auto callers = method->callers();
		html = "Number of callers: " + QString::number(callers.size()) + "<br>" + html;
		html += "<table id=\"callerTable\" style=\"display:none\">";
		html += "<tr><td style=\"border-right:solid 1px\"><b>Class</b></td><td><b>Method</b></td></tr>";
		for (auto caller : callers)
		{
			auto methodClass = caller->firstAncestorOfType<OOModel::Class>();
			html = html + "<tr><td style=\"border-right:solid 1px\">" + (methodClass ? methodClass->name() : "n/a") + "</td>";
			html = html + "<td>" + caller->name() + "</td></tr>";
		}
		html += "</table>";
		return html;
	}
	else if (auto someClass = DCast<OOModel::Class>(node))
	{
		QSet<Model::Node*> result;
		auto top = someClass->root();
		//Find all the places where this class is referenced
		QList<Model::Node*> toCheck{top};
		while (!toCheck.isEmpty())
		{
			auto check = toCheck.takeLast();
			if (auto expr = DCast<OOModel::ReferenceExpression>(check))
				if (expr->target() == someClass)
					result << expr->topMostExpressionParent();
			toCheck.append(check->children());
		}
		return "Number of usages: " + QString::number(result.size());
	}
	else return QString();
}

QString InfoMethods::fullName(Model::Node *node)
{
	if (auto method = DCast<OOModel::Method>(node))
		return "<b>" + method->fullyQualifiedName() + "</b>";
	else if (auto clazz = DCast<OOModel::Class>(node))
		return "<b>" + clazz->name() + "</b>";
	else return QString();
}

QString InfoMethods::expandButton(QString functionName, QString expandableId)
{
	QString html = "<script>";
	html += "function " + functionName + "()";
	html += "{";
	html += "var table = document.getElementById(\"" + expandableId + "\");";
	html += "table.style.display = (table.style.display == \"table\") ? \"none\" : \"table\";";
	html += "}";
	html += "</script>";
	html += "<button onclick=\"" + functionName + "()\">Details</button>";
	return html;
}

}
