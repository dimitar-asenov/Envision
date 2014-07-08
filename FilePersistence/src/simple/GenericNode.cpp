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

#include "GenericNode.h"
#include "GenericNodeAllocator.h"
#include "Parser.h"

namespace FilePersistence {

static const int MAX_DOUBLE_PRECISION = 15;

GenericNode::GenericNode(){}
GenericNode::~GenericNode()
{
	if (dataLineLength_ == 0)
		for (auto c : children_) SAFE_DELETE(c);
}

const QString& GenericNode::valueAsString() const
{
	ensureDataRead();
	Q_ASSERT(valueType_ == STRING_VALUE);
	return value_;
}

long GenericNode::valueAsLong() const
{
	ensureDataRead();
	Q_ASSERT(valueType_ == INT_VALUE);
	bool ok = true;

	int res = value_.toInt(&ok);
	if ( !ok ) throw FilePersistenceException("Could not read integer value " + value_);

	return res;
}

double GenericNode::valueAsDouble() const
{
	ensureDataRead();
	Q_ASSERT(valueType_ == DOUBLE_VALUE);
	bool ok = true;

	double res = value_.toDouble(&ok);
	if ( !ok ) throw FilePersistenceException("Could read real value " + value_);

	return res;
}

GenericNode* GenericNode::child(const QString& name)
{
	ensureDataRead();
	for (auto c : children_)
		if (c->name() == name) return c;

	return nullptr;
}

void GenericNode::setValue(const QString& value)
{
	Q_ASSERT(children_.isEmpty());
	Q_ASSERT(valueType_ == NO_VALUE);

	valueType_ = STRING_VALUE;
	value_ = value;
}

void GenericNode::setValue(long value)
{
	Q_ASSERT(children_.isEmpty());
	Q_ASSERT(valueType_ == NO_VALUE);

	valueType_ = INT_VALUE;
	value_ = QString::number(value);
}

void GenericNode::setValue(double value)
{
	Q_ASSERT(children_.isEmpty());
	Q_ASSERT(valueType_ == NO_VALUE);

	valueType_ = DOUBLE_VALUE;
	value_ = QString::number(value, 'f', MAX_DOUBLE_PRECISION);
}

void GenericNode::setValue(ValueType type, const QString& value)
{
	Q_ASSERT(children_.isEmpty());
	Q_ASSERT(valueType_ == NO_VALUE);
	Q_ASSERT(type != NO_VALUE);

	valueType_ = type;
	value_ = value;
}

GenericNode* GenericNode::addChild(GenericNode* child)
{
	Q_ASSERT(value_.isEmpty());
	Q_ASSERT(valueType_ == NO_VALUE);

	Q_ASSERT(child);

	children_.append(child);
	return child;
}

GenericNode* GenericNode::find(Model::NodeIdType id)
{
	ensureDataRead();
	if (id_ == id) return this;
	for (auto c : children_)
		if (auto found = c->find(id)) return found;

	return nullptr;
}

void GenericNode::resetForLoading(char* dataLine, int dataLineLength)
{
	Q_ASSERT(dataLineLength > 0);

	dataLine_ = dataLine;
	dataLineLength_ = dataLineLength;

	name_.clear();
	type_.clear();
	value_.clear();
	valueType_ = NO_VALUE;
	id_ = {};
	children_.clear(); // No need to delete these
}

void GenericNode::ensureDataRead() const
{
	if (dataLine_)
	{
		Parser::parseLine(const_cast<GenericNode*>(this), dataLine_, dataLineLength_);
		// Don't delete this, just mark it unused. The allocator will delete it.
		const_cast<GenericNode*>(this)->dataLine_ = nullptr;
	}
}

} /* namespace FilePersistence */
