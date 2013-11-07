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
#include "GenericNodeAllocator.h"

namespace FilePersistence {

static const int MAX_DOUBLE_PRECISION = 15;

static const int ATTRIBUTE_TRUE = 1;
static const int ATTRIBUTE_FALSE = 0;

static const QString PREFIX_STRING = QString("S_");
static const QString PREFIX_INTEGER = QString("I_");
static const QString PREFIX_DOUBLE = QString("D_");

GenericNode::GenericNode(){}
GenericNode::~GenericNode()
{
	if (lineStartInData_ == 0 && lineEndInData_ == 0)
		for(auto c : children_) SAFE_DELETE(c);
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

GenericNode* GenericNode::find(NodeIdMap::NodeIdType id)
{
	ensureDataRead();
	if (id_ == id) return this;
	for(auto c : children_)
		if (auto found = c->find(id)) return found;

	return nullptr;
}

inline int GenericNode::countTabs(char* data, int lineStart, int lineEnd)
{
	int numTabs = 0;

	for(int i = lineStart; i<=lineEnd; ++i)
	{
		if (data[i] == '\t') ++numTabs;
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

GenericNode* GenericNode::load(const QString& filename, bool lazy, GenericNodeAllocator* allocator)
{
	QFile file(filename);
	if ( !file.open(QIODevice::ReadOnly) )
		throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

	Q_ASSERT(file.size() < std::numeric_limits<int>::max());
	int totalFileSize = static_cast<int>(file.size());

	auto mapped = reinterpret_cast<char*>(file.map(0, totalFileSize));
	Q_ASSERT(mapped);

	// We need to make a copy since the memory will be unmapped after the file object gets out of scope.
	auto data = new char[totalFileSize];
	memcpy(data,mapped, totalFileSize);

	QList<GenericNode*> nodeStack;
	GenericNode* top = nullptr;
	int previousTabLevel = std::numeric_limits<int>::max();

	int start = 0;
	int lineEnd = -1; // This is must be initialized to -1 for the first call to nextLine.

	while ( nextNonEmptyLine(data, totalFileSize, start, lineEnd) )
	{
		auto tabLevel = countTabs(data, start, lineEnd);

		if (top == nullptr)
		{
			// Top can't be in the allNodes array since it must delete that array in its own destructor.
			top = allocator->newRoot(data, start, lineEnd);
			nodeStack.append(top);
			previousTabLevel = tabLevel;
		}
		else
		{
			Q_ASSERT(tabLevel <= previousTabLevel + 1);

			GenericNode* child = allocator->newChild(start, lineEnd);

			if (tabLevel == previousTabLevel + 1)
			{
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
				nodeStack.last()->addChild(child);
				nodeStack.append(child);
			}
		}

		// The top of the stack should now contain the element that we must add now
		if (!lazy) nodeStack.last()->ensureDataRead();
	}

	return top;
}

void GenericNode::parseData(GenericNode* node, char* data, int start, int lineEnd)
{
	auto tabLevel = countTabs(data, start, lineEnd);

	// Handle Headers
	start+=tabLevel;
	int headerPartEnd = -1;

	// Name
	auto moreHeaderParts = nextHeaderPart(data, start, headerPartEnd, lineEnd);
	Q_ASSERT(moreHeaderParts);
	node->setName( QString::fromLatin1(data + start, headerPartEnd-start+1) );

	// Type
	start = headerPartEnd+1;
	moreHeaderParts = nextHeaderPart(data, start, headerPartEnd, lineEnd);
	Q_ASSERT(moreHeaderParts);
	node->setType( QString::fromLatin1(data + start, headerPartEnd-start+1) );

	// Id (optional)
	start = headerPartEnd+1;
	moreHeaderParts = nextHeaderPart(data, start, headerPartEnd, lineEnd);
	if(moreHeaderParts)
	{
		bool isId = true;
		NodeIdMap::NodeIdType id = toId(data, start, headerPartEnd, isId);

		if ( isId ) node->setId( id );
		else throw FilePersistenceException("Unknown node header element "
				+ QString::fromAscii(data+start,headerPartEnd-start+1));
	}

	if (moreHeaderParts)
	{
		start = headerPartEnd+1;
		moreHeaderParts = nextHeaderPart(data, start, headerPartEnd, lineEnd);
	}

	Q_ASSERT(!moreHeaderParts);


	// Handle Value
	if (start > lineEnd) return;
	Q_ASSERT(data[start] == '.');

	// Ignore white spaces
	++start;
	while (start <= lineEnd && (data[start] == ' ' || data[start] == '\t')) ++start;
	if (start > lineEnd) return; // no value

	if (PREFIX_STRING == QString::fromLatin1(data + start, PREFIX_STRING.length()))
	{
		start += PREFIX_STRING.length();
		auto s = rawStringToQString(data, start, lineEnd);

		if (s.isEmpty() && start <= lineEnd)
		{
			// There were some characters, but the string is empty.
			// See if this was the UTF-8 BOM, which Qt ignores in strings, but it might be relevant for characters
			if(lineEnd-start == 2 && data[start] == (char)0xEF
					&& data[start+1] == (char)0xBB && data[start+2] == (char)0xBF)
			s = QChar{QChar::ByteOrderMark};
			else
				throw FilePersistenceException("Invalid string sequence when reading node id " + node->id());
		}

		node->setValue(STRING_VALUE, s);
	}
	else if (PREFIX_INTEGER == QString::fromLatin1(data + start, PREFIX_INTEGER.length()))
		node->setValue(INT_VALUE, QString::fromLatin1(data+start+PREFIX_INTEGER.length(),
				lineEnd - start - PREFIX_INTEGER.length() + 1 ));
	else if (PREFIX_DOUBLE == QString::fromLatin1(data + start, PREFIX_DOUBLE.length()))
		node->setValue(DOUBLE_VALUE, QString::fromLatin1(data+start+PREFIX_DOUBLE.length(),
				lineEnd - start - PREFIX_DOUBLE.length() + 1 ));
	else throw FilePersistenceException("Unknown value prefix" +
			QString::fromUtf8(data+start,lineEnd-start+1));
}

QString GenericNode::escape(const QString& line)
{
	QString res = line;
	res.replace('\\', "\\\\");
	res.replace('\r', "\\r");
	res.replace('\n', "\\n");

	return res;
}

QString GenericNode::rawStringToQString(char* data, int start, int endInclusive)
{
	// First get the escaped string
	QString utf8Escaped = QString::fromUtf8(data + start, endInclusive - start + 1);
	QString res;
	res.reserve(utf8Escaped.length());

	// The unescape it
	bool escaped = false;
	for(auto c : utf8Escaped)
	{
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

int GenericNode::toId(char* data, int start, int endInclusive, bool& ok)
{
	int id = 0;

	for(int i = start; i<=endInclusive; ++i)
	{
		auto c = data[i];
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

bool GenericNode::nextNonEmptyLine(char* data, int dataSize, int& lineStart, int& lineEnd)
{
	lineStart = lineEnd + 1;

	while (lineStart < dataSize)
	{
		auto c = data[lineStart];
		Q_ASSERT((c & (char)0x80) == 0); // Make sure that this character is an ASCII one and not some UTF-8 codepoint
		Q_ASSERT(c != 0);
		if (c =='\n' || c== '\r') ++lineStart;
		else break;
	}

	if (lineStart >= dataSize) return false;

	// LineEnd is the last character that belongs to the line. It does not point to the end of line character.
	lineEnd = lineStart + 1;

	bool onlyWhiteSpace = true;

	while (lineEnd < dataSize)
	{
		auto c = data[lineEnd];

		int utf8Bytes = 1;
		if ((c & 0x80) == 0) utf8Bytes = 1;
		else if ((c & 0xE0) == 0xC0) utf8Bytes = 2;
		else if ((c & 0xF0) == 0xE0) utf8Bytes = 3;
		else if ((c & 0xF8) == 0xF0) utf8Bytes = 4;
		else Q_ASSERT(false);

		if (utf8Bytes > 1)
		{
			lineEnd += utf8Bytes;
			onlyWhiteSpace = false;
		}
		else
		{
			if (c == '\n' || c == '\r') break;
			else
			{
				++lineEnd;
				if (c != ' ' && c != '\t') onlyWhiteSpace = false;
			}
		}
	}

	--lineEnd;
	if (lineEnd >= dataSize) lineEnd = dataSize - 1; // To compensate for possible unicode issues

	if (lineEnd >= lineStart)
	{
		if (onlyWhiteSpace) return nextNonEmptyLine(data, dataSize, lineStart, lineEnd);
		return true;
	}
	else return false;
}

int GenericNode::indexOf(const char c, char* data, int start, int endInclusive)
{
	for(int i = start; i<=endInclusive; ++i) if(data[i]==c) return i;
	return -1;
}

bool GenericNode::nextHeaderPart(char* data, int& start, int&endInclusive, int lineEnd)
{
	while (start <= lineEnd && (data[start] == ' ' || data[start] == '\t')) ++start;
	if (start > lineEnd || data[start] == '.') return false;

	endInclusive = start + 1;
	while (endInclusive <= lineEnd && data[endInclusive] != ' ' && data[endInclusive] != '\t'
			&& data[endInclusive] != '.') ++endInclusive;

	--endInclusive;
	return true;
}

void GenericNode::resetForLoading(char* data, int lineStart, int lineEndInclusive)
{
	Q_ASSERT(lineStart >= 0);
	Q_ASSERT(lineEndInclusive >= lineStart);

	data_ = data;
	lineStartInData_ = lineStart;
	lineEndInData_ = lineEndInclusive;

	name_.clear();
	type_.clear();
	value_.clear();
	valueType_ = NO_VALUE;
	id_ = -1;
	children_.clear(); // No need to delete these
}

} /* namespace FilePersistence */
