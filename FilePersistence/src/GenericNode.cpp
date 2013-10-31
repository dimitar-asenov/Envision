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

#include "GenericNode.h"

namespace FilePersistence {

static const int MAX_DOUBLE_PRECISION = 15;

static const int ATTRIBUTE_TRUE = 1;
static const int ATTRIBUTE_FALSE = 0;

static const QString PREFIX_STRING = QString("S_");
static const QString PREFIX_INTEGER = QString("I_");
static const QString PREFIX_DOUBLE = QString("D_");

GenericNode::GenericNode()
{
	// TODO Auto-generated constructor stub

}

GenericNode::~GenericNode()
{
	for(auto c : children_) SAFE_DELETE(c);
}

const QString& GenericNode::valueAsString() const
{
	Q_ASSERT(valueType_ == STRING_VALUE);
	return value_;
}

long GenericNode::valueAsLong() const
{
	Q_ASSERT(valueType_ == INT_VALUE);
	bool ok = true;

	int res = value_.toInt(&ok);
	if ( !ok ) throw FilePersistenceException("Could not read integer value " + value_);

	return res;
}

double GenericNode::valueAsDouble() const
{
	Q_ASSERT(valueType_ == DOUBLE_VALUE);
	bool ok = true;

	double res = value_.toDouble(&ok);
	if ( !ok ) throw FilePersistenceException("Could read real value " + value_);

	return res;
}

GenericNode* GenericNode::child(const QString& name)
{
	for(auto c : children_)
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

GenericNode* GenericNode::addChild(GenericNode* child)
{
	Q_ASSERT(value_.isEmpty());
	Q_ASSERT(valueType_ == NO_VALUE);

	Q_ASSERT(child);

	children_.append(child);
	return child;
}

GenericNode* GenericNode::find(NodeIdMap::NodeIdType id)
{
	if (id_ == id) return this;
	for(auto c : children_)
		if (auto found = c->find(id)) return found;

	return nullptr;
}

inline int GenericNode::countTabs(const QString& line)
{
	int numTabs = 0;

	for(QChar c : line)
	{
		if (c == '\t') ++numTabs;
		else break;
	}

	return numTabs;
}

void GenericNode::save(QTextStream& stream, int tabLevel)
{
	for(int i = 0; i<tabLevel; ++i) stream << '\t';
	stream << name_ << ' ' << type_;
	if (id_ >= 0) stream << ' ' << id_;
	if (hasValue())
	{
		if (valueType_ == STRING_VALUE) stream << ". " << PREFIX_STRING << escape(value_);
		else if (valueType_ == INT_VALUE) stream << ". " << PREFIX_INTEGER << value_;
		else if (valueType_ == DOUBLE_VALUE) stream << ". " << PREFIX_DOUBLE << value_;
		else throw FilePersistenceException("Unknown value type" + valueType_);
	}
	stream << '\n';

	for(auto c : children_)
		c->save(stream, tabLevel+1);
}

GenericNode* GenericNode::load(const QString& filename)
{
	QFile file(filename);
	if ( !file.open(QIODevice::ReadOnly) )
		throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

	QTextStream stream(&file);
	stream.setCodec("UTF-8");

	Q_ASSERT(!stream.atEnd());

	QList<GenericNode*> nodeStack;
	GenericNode* top = nullptr;
	int previousTabLevel = std::numeric_limits<int>::max();

	while ( !stream.atEnd() )
	{
		QString line = stream.readLine();
		if (line.isEmpty()) continue;

		auto tabLevel = countTabs(line);

		if (top == nullptr)
		{
			top = new GenericNode();
			nodeStack.append(top);
			previousTabLevel = tabLevel;
		}
		else
		{
			Q_ASSERT(tabLevel <= previousTabLevel + 1);
			if (tabLevel == previousTabLevel + 1)
			{
				auto child = new GenericNode();
				nodeStack.last()->addChild(child);
				nodeStack.append(child);
				previousTabLevel = tabLevel;
			}
			else
			{
				// Some nodes are finished, pop them
				nodeStack.removeLast(); // We always remove at least one element (only one in case prev level = tab level)
				while (previousTabLevel > tabLevel)
				{
					--previousTabLevel;
					nodeStack.removeLast();
				}

				auto child = new GenericNode();
				nodeStack.last()->addChild(child);
				nodeStack.append(child);
			}
		}

		// The top of the stack should now contain the element that we must add now

		int dotIndex = line.indexOf('.', tabLevel); // There may not be a dot, in case there is no value.
		if (dotIndex < 0) dotIndex = line.length();

		int startIndex = findNextNonWhiteSpace(line, tabLevel, dotIndex);
		Q_ASSERT(startIndex >= 0);
		int endIndex = findNextWhiteSpace(line, startIndex + 1, dotIndex);
		Q_ASSERT(endIndex > 0);
		nodeStack.last()->setName( line.midRef(startIndex, endIndex-startIndex) );

		startIndex = findNextNonWhiteSpace(line, endIndex + 1, dotIndex);
		Q_ASSERT(startIndex > 0);
		endIndex = findNextWhiteSpace(line, startIndex + 1, dotIndex);
		nodeStack.last()->setType( line.midRef(startIndex,
				endIndex >= 0 ? endIndex-startIndex :
						(dotIndex < 0 ? -1 : dotIndex-startIndex)) );

		startIndex = findNextNonWhiteSpace(line, endIndex + 1, dotIndex);
		while (startIndex > 0)
		{
			endIndex = findNextWhiteSpace(line, startIndex + 1, dotIndex);
			auto headerPart = line.midRef(startIndex,
					endIndex >= 0 ? endIndex-startIndex :
							(dotIndex < 0 ? -1 : dotIndex-startIndex));

			// Try to process this as an Id
			bool isId = true;
			NodeIdMap::NodeIdType id = refToId(headerPart, isId);

			if ( isId ) nodeStack.last()->setId( id );
			else
				throw FilePersistenceException("Unknown node header element " + line);

			if (endIndex < 0) break;
			startIndex = findNextNonWhiteSpace(line, endIndex + 1, dotIndex);
		}

		if (dotIndex < 0 || line.length() == dotIndex) continue; // No value

		startIndex = findNextNonWhiteSpace(line, dotIndex+1, line.length());
		if (startIndex == -1) continue;
		QStringRef rest = line.midRef(startIndex);

		if (rest.startsWith(PREFIX_STRING))
			nodeStack.last()->setValue(STRING_VALUE, unEscape(line, startIndex + PREFIX_STRING.size()));
		else if (rest.startsWith(PREFIX_INTEGER))
			nodeStack.last()->setValue(INT_VALUE, line.midRef( startIndex + PREFIX_INTEGER.size()));
		else if (rest.startsWith(PREFIX_DOUBLE))
			nodeStack.last()->setValue(DOUBLE_VALUE, line.midRef( startIndex + PREFIX_DOUBLE.size()));
		else throw FilePersistenceException("Unknown value prefix" + line);
	}

	return top;
}

int GenericNode::findNextNonWhiteSpace(const QString& line, int startPos, int before)
{
	// We assume that all, characters in line up to 'before' are ASCII
	for(int i = startPos; i < line.length(); ++i)
		if (i>= before) return -1;
		else
		{
			auto c = line[i].cell();
			if (c != ' ' && c != '\t') return i;
		}

	return -1;
}

int GenericNode::findNextWhiteSpace(const QString& line, int startPos, int before)
{
	// We assume that all, characters in line up to 'before' are ASCII
	for(int i = startPos; i < line.length(); ++i)
		if (i >= before) return -1;
		else
		{
			auto c = line[i].cell();
			if (c == ' ' || c == '\t') return i;
		}

	return -1;
}

QString GenericNode::escape(const QString& line)
{
	QString res = line;
	res.replace('\\', "\\\\");
	res.replace('\r', "\\r");
	res.replace('\n', "\\n");

	return res;
}

QString GenericNode::unEscape(const QString& line, int startAt)
{
	QString res;
	res.reserve(line.length()-startAt);

	bool escaped = false;
	for(int i = startAt; i<line.length(); ++i)
	{
		const QChar& c = line[i];
		if (!escaped && c == '\\')
		{
			escaped = true;
			continue;
		}

		if (!escaped)
		{
			res += c;
			continue;
		}

		if (c == '\\')
		{
			res += '\\';
			escaped = false;
		}
		else if (c== 'n')
		{
			res += '\n';
			escaped = false;
		}
		else if (c== 'r')
		{
			res += '\r';
			escaped = false;
		}
		else throw FilePersistenceException("Unrecognized escape character " + QString(c));
	}

	return res;
}

int GenericNode::refToId(const QStringRef& ref, bool& ok)
{
	int id = 0;

	int end = ref.position() + ref.size();
	auto s = ref.string();
	for(int i = ref.position(); i<end; ++i)
	{
		auto c = (*s)[i].cell();
		if (c >= '0' && c<= '9') id = id*10 + (c - '0');
		else
		{
			ok = false;
			return 0;
		}
	}

	ok = true;
	return id;
}

} /* namespace FilePersistence */
