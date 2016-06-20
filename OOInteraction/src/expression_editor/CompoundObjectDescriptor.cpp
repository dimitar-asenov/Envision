/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "CompoundObjectDescriptor.h"
#include "OOModel/src/expressions/IntegerLiteral.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "ModelBase/src/model/TreeManager.h"

namespace OOInteraction {

CompoundObjectDescriptor::CompoundObjectDescriptor(const QString& name, const QString& signature,
		int precedence, Associativity associativity)
		: OOOperatorDescriptor{name, signature, precedence, associativity}
{}

OOModel::Expression* CompoundObjectDescriptor::create(const QList<OOModel::Expression*>& operands)
{
	Q_ASSERT(operands.size() == 1);
	auto ilit = DCast<OOModel::IntegerLiteral> (operands.first());
	Q_ASSERT(ilit);
	auto e = storedExpressions().value(ilit->valueAsInt());
	SAFE_DELETE(ilit);
	Q_ASSERT(e);

	Q_ASSERT(e->parent());
	auto replaceSuccessfull = e->parent()->replaceChild(e, new OOModel::EmptyExpression{});
	Q_ASSERT(replaceSuccessfull);

	return e;
}

QMap<int, OOModel::Expression*>& CompoundObjectDescriptor::storedExpressions()
{
	static QMap<int, OOModel::Expression*> map;
	return map;
}

const QString& CompoundObjectDescriptor::compoundSignature()
{
	// If the signature is changed the processDeleteOrBackspaceKey() method must be adjusted accordingly
	static const QString symbols{NUM_SIGNATURE_SYMBOLS, '@'};
	static const QString sig{symbols + " expr " +symbols};
	return sig;
}

int CompoundObjectDescriptor::nextId()
{
	static int id = 0;
	return ++id;
}

const QString CompoundObjectDescriptor::storeExpression(OOModel::Expression* object)
{
	int id;

	if (storedExpressions().values().contains(object))
		id = storedExpressions().key(object);
	else
	{
		id = nextId();
		storedExpressions().insert(id, object);
	}

	auto str = compoundSignature();
	str.replace(" expr ", QString::number(id));

	return str;
}

void CompoundObjectDescriptor::cleanAllStoredExpressions()
{
	// We do not need to delete the original expressions. This is handled by the Undo system.
	storedExpressions().clear();
}

bool CompoundObjectDescriptor::processDeleteOrBackspaceKey(Qt::Key key, QString& expression, int& index)
{
	// A little optimization to speed up the most common case
	if (key == Qt::Key_Delete)
	{
		if ( expression.at(index) != '@') return false;
		if (isInQuotes(index, expression)) return false;
	}
	if (key == Qt::Key_Backspace)
	{
		if (expression.at(index-1) != '@') return false;
		if (isInQuotes(index, expression)) return false;
	}

	// Try to match the expression to the compound signature
	QString number;
	int delimitersFound = 0;
	int i = key == Qt::Key_Delete ? index : index-1;
	int step = key == Qt::Key_Delete ? 1 : -1;

	while (i>=0 && i < expression.length())
	{
		auto c = expression.at(i);
		if ( c == '@') ++delimitersFound;
		else if (c.isDigit() && delimitersFound == NUM_SIGNATURE_SYMBOLS)
		{
			if (key == Qt::Key_Delete) number.append(c);
			else number.prepend(c);
		}
		else return false;

		if (delimitersFound > NUM_SIGNATURE_SYMBOLS && number.isEmpty()) return false;
		if (delimitersFound == 2*NUM_SIGNATURE_SYMBOLS) break;

		i+=step;
	}

	if (delimitersFound != 2*NUM_SIGNATURE_SYMBOLS) return false;

	// If we reach this point then the user is indeed trying to delete a compound expression. Make sure the number it
	// refers to exists.
	bool conversionOK = false;
	int n = number.toInt(&conversionOK, 10);
	if (!conversionOK) return false;

	if (!storedExpressions().contains(n)) return false;

	// Everything checks out, delete the compound expression
	auto compundLenght = delimitersFound + number.length();
	if (key == Qt::Key_Backspace) index-=compundLenght;
	expression.remove(index, compundLenght);
	storedExpressions().remove(n);

	return true;
}

bool CompoundObjectDescriptor::isInQuotes(int index, const QString& string, const QChar& quote)
{
	bool inQuote = false;
	bool escaped = false;

	for (int i = 0; i<index; ++i)
	{
		if (escaped)
		{
			// TODO: This assumes that the escape sequence after an escape slash \ is only one character long. Improve
			escaped = false;
			continue;
		}

		if (string.at(i) == quote) inQuote = !inQuote;
		else if (inQuote && string.at(i) == '\\') escaped = true;
	}

	return inQuote;
}

}
