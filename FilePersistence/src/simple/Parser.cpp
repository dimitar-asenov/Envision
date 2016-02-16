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
#include "Parser.h"
#include "GenericNode.h"
#include "GenericPersistentUnit.h"

namespace FilePersistence {

const QString PREFIX_STRING{"S_"};
const QString PREFIX_INTEGER{"I_"};
const QString PREFIX_DOUBLE{"D_"};
/**
 * If true, sort children by label when writing encoding.
 * This is to ensure consistency between all methods used to produce Envision encodings.
 */
const bool SORT_BY_LABEL = true;

QString Parser::escape(const QString& line)
{
	QString res = line;
	res.replace('\\', "\\\\");
	res.replace('\r', "\\r");
	res.replace('\n', "\\n");

	return res;
}

QString Parser::rawStringToQString(const char* data, int start, int endInclusive)
{
	// First get the escaped string
	QString utf8Escaped = QString::fromUtf8(data + start, endInclusive - start + 1);
	QString res;
	res.reserve(utf8Escaped.length());

	// The unescape it
	bool escaped = false;
	for (auto c : utf8Escaped)
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
		else throw FilePersistenceException{"Unrecognized escape character " + QString{c}};
	}

	return res;
}

Model::NodeIdType Parser::toId(const char* data, int start, int endInclusive, bool& ok)
{
	// length of "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" is 38.
	const char* idStart = data + start;
	if (endInclusive - start != 37 || data[start] != '{' || data[endInclusive] != '}'
		 || idStart[9] != '-' || idStart[14] != '-' || idStart[19] != '-' || idStart[24] != '-')
	{
		ok = false;
		return {};
	}

	ok = true;
	uint low = hexDigitToChar(idStart[1], ok) << 28
		| hexDigitToChar(idStart[2], ok) << 24
		| hexDigitToChar(idStart[3], ok) << 20
		| hexDigitToChar(idStart[4], ok) << 16
		| hexDigitToChar(idStart[5], ok) << 12
		| hexDigitToChar(idStart[6], ok) << 8
		| hexDigitToChar(idStart[7], ok) << 4
		| hexDigitToChar(idStart[8], ok);
	ushort w1 = hexDigitToChar(idStart[10], ok) << 12
		| hexDigitToChar(idStart[11], ok) << 8
		| hexDigitToChar(idStart[12], ok) << 4
		| hexDigitToChar(idStart[13], ok);
	ushort w2 = hexDigitToChar(idStart[15], ok) << 12
		| hexDigitToChar(idStart[16], ok) << 8
		| hexDigitToChar(idStart[17], ok) << 4
		| hexDigitToChar(idStart[18], ok);
	uchar b1 = hexDigitToChar(idStart[20], ok) << 4 | hexDigitToChar(idStart[21], ok);
	uchar b2 = hexDigitToChar(idStart[22], ok) << 4 | hexDigitToChar(idStart[23], ok);
	uchar b3 = hexDigitToChar(idStart[25], ok) << 4 | hexDigitToChar(idStart[26], ok);
	uchar b4 = hexDigitToChar(idStart[27], ok) << 4 | hexDigitToChar(idStart[28], ok);
	uchar b5 = hexDigitToChar(idStart[29], ok) << 4 | hexDigitToChar(idStart[30], ok);
	uchar b6 = hexDigitToChar(idStart[31], ok) << 4 | hexDigitToChar(idStart[32], ok);
	uchar b7 = hexDigitToChar(idStart[33], ok) << 4 | hexDigitToChar(idStart[34], ok);
	uchar b8 = hexDigitToChar(idStart[35], ok) << 4 | hexDigitToChar(idStart[36], ok);

	if (!ok) return {};
	return QUuid{low, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8};
}

uchar Parser::hexDigitToChar(char d, bool& ok)
{
	if (d >= '0' && d<= '9') return d - '0';

	// lowercase
	constexpr char bitMask = 'A' ^ 'a';
	d |= bitMask;
	if (d >= 'a' && d<= 'f') return d - 'a' + 10;

	ok = false;
	return 0;
}

bool Parser::nextNonEmptyLine(const char* data, int dataSize, int& lineStart, int& lineEnd)
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

int Parser::indexOf(const char c, const char* data, int start, int endInclusive)
{
	for (int i = start; i<=endInclusive; ++i) if (data[i]==c) return i;
	return -1;
}

bool Parser::nextHeaderPart(const char* data, int& start, int&endInclusive, int lineEnd)
{
	while (start <= lineEnd && (data[start] == ' ' || data[start] == '\t')) ++start;
	if (start > lineEnd || data[start] == '.') return false;

	endInclusive = start + 1;
	while (endInclusive <= lineEnd && data[endInclusive] != ' ' && data[endInclusive] != '\t'
			&& data[endInclusive] != '.') ++endInclusive;

	--endInclusive;
	return true;
}

bool Parser::labelComparator(GenericNode * const node1, GenericNode * const node2)
{
	bool ok = true;
	node1->label().toInt(&ok);
	if (ok)
	{
		node2->label().toInt(&ok);
		if (ok)
			return node1->label().toInt() < node2->label().toInt();
	}
	return node1->label() < node2->label();
}

QList<GenericNode*> Parser::save(QTextStream& stream, GenericNode* node,
											const QStringList& persistentUnitTypes, int tabLevel)
{
	QList<GenericNode*> res;

	bool isPU = tabLevel > 0 && persistentUnitTypes.contains(node->type());

	for (int i = 0; i<tabLevel; ++i) stream << '\t';
	stream << node->label() << ' ' << (isPU ? GenericNode::PERSISTENT_UNIT_TYPE : node->type());
	if (!node->id().isNull())
	{
		stream << ' ' << node->id().toString() << ' ' << node->parentId().toString();
	}

	if (isPU)
	{
		stream << '\n';
		res << node;
	}
	else
	{
		if (node->hasValue())
		{
			if (node->valueType() == GenericNode::STRING_VALUE) stream << ". "<< PREFIX_STRING << escape(node->rawValue());
			else if (node->valueType() == GenericNode::INT_VALUE) stream << ". " << PREFIX_INTEGER << node->rawValue();
			else if (node->valueType() == GenericNode::DOUBLE_VALUE) stream << ". " << PREFIX_DOUBLE << node->rawValue();
			else throw FilePersistenceException{"Unknown value type " + QString::number(node->valueType())};
		}
		stream << '\n';

		QList<GenericNode*> children{node->children()};

		if (SORT_BY_LABEL)
			std::sort(children.begin(), children.end(), labelComparator);

		for (auto child : children)
			res << save(stream, child, persistentUnitTypes, tabLevel+1);
	}

	return res;
}

GenericNode* Parser::load(const QString& filename, bool lazy, GenericPersistentUnit& persistentUnit)
{
	QFile file{filename};
	if ( !file.open(QIODevice::ReadOnly) )
		throw FilePersistenceException{"Could not open file " + file.fileName() + ". " + file.errorString()};

	Q_ASSERT(file.size() < std::numeric_limits<int>::max());
	int totalFileSize = static_cast<int>(file.size());

	auto mapped = reinterpret_cast<char*>(file.map(0, totalFileSize));
	Q_ASSERT(mapped);

	return load(mapped, totalFileSize, lazy, persistentUnit);
}

GenericNode* Parser::load(const char* data, int dataLength, bool lazy, GenericPersistentUnit& persistentUnit)
{
	// We need to make a copy since the memory will be unmapped after the file object gets out of scope.
	auto dataCopy = persistentUnit.setData(data, dataLength);

	QList<GenericNode*> nodeStack;
	GenericNode* top = nullptr;

	int start = 0;
	int lineEnd = -1; // This is must be initialized to -1 for the first call to nextLine.

	// TODO: Do not rely on indentation, instead use the parentID (will this work in the clipboard?)
	while ( nextNonEmptyLine(dataCopy, dataLength, start, lineEnd) )
	{
		auto tabLevel = countTabs(dataCopy, start, lineEnd);

		if (top == nullptr)
		{
			Q_ASSERT(tabLevel == 0);

			// Top can't be in the allNodes array since it must delete that array in its own destructor.
			top = persistentUnit.newNode(start, lineEnd);
			nodeStack.append(top);
		}
		else
		{
			Q_ASSERT(tabLevel > 0);
			Q_ASSERT(nodeStack.size() >= tabLevel);

			GenericNode* child = persistentUnit.newNode(start, lineEnd);

			while (nodeStack.size() > tabLevel) nodeStack.removeLast();

			nodeStack.last()->attachChild(child);
			child->setParent(nodeStack.last());
			nodeStack.append(child);
		}

		// The top of the stack should now contain the element that we must add now
		if (!lazy) nodeStack.last()->label(); // This will ensure that all data is read. We don't actually need the name.
	}

	return top;
}

void Parser::parseLine(GenericNode* node, const char* line, int lineLength)
{
	int start = 0;
	int lineEnd = lineLength - 1; // Last character which belongs to the line, excluding new line characters.

	auto tabLevel = countTabs(line, start, lineEnd);

	// Handle Headers
	start+=tabLevel;
	int headerPartEnd = -1;

	// Name
	auto moreHeaderParts = nextHeaderPart(line, start, headerPartEnd, lineEnd);
	Q_ASSERT(moreHeaderParts);
	node->setLabel( QString::fromLatin1(line + start, headerPartEnd-start+1) );

	// Type
	start = headerPartEnd+1;
	moreHeaderParts = nextHeaderPart(line, start, headerPartEnd, lineEnd);
	Q_ASSERT(moreHeaderParts);
	node->setType( QString::fromLatin1(line + start, headerPartEnd-start+1) );

	// Id and parent (optional)
	start = headerPartEnd+1;
	moreHeaderParts = nextHeaderPart(line, start, headerPartEnd, lineEnd);
	if (moreHeaderParts)
	{
		bool isId = true;
		Model::NodeIdType id = toId(line, start, headerPartEnd, isId);

		if ( isId ) node->setId( id );
		else throw FilePersistenceException{"Unknown node header element (should be id) "
				+ QString::fromLatin1(line+start, headerPartEnd-start+1)};

		// parent ID
		start = headerPartEnd+1;
		moreHeaderParts = nextHeaderPart(line, start, headerPartEnd, lineEnd);
		id = toId(line, start, headerPartEnd, isId);
		if ( isId ) node->setParentId( id );
		else throw FilePersistenceException{"Unknown node header element (should be parent id)"
				+ QString::fromLatin1(line+start, headerPartEnd-start+1)};
	}

	if (moreHeaderParts)
	{
		start = headerPartEnd+1;
		moreHeaderParts = nextHeaderPart(line, start, headerPartEnd, lineEnd);
	}

	Q_ASSERT(!moreHeaderParts);


	// Handle Value
	if (start > lineEnd) return;
	Q_ASSERT(line[start] == '.');

	// Ignore white spaces
	++start;
	while (start <= lineEnd && (line[start] == ' ' || line[start] == '\t')) ++start;
	if (start > lineEnd) return; // no value

	if (PREFIX_STRING == QString::fromLatin1(line + start, PREFIX_STRING.length()))
	{
		start += PREFIX_STRING.length();
		auto s = rawStringToQString(line, start, lineEnd);

		if (s.isEmpty() && start <= lineEnd)
		{
			// There were some characters, but the string is empty.
			// See if this was the UTF-8 BOM, which Qt ignores in strings, but it might be relevant for characters
			if (lineEnd-start == 2 && line[start] == (char)0xEF
					&& line[start+1] == (char)0xBB && line[start+2] == (char)0xBF)
			s = QChar{QChar::ByteOrderMark};
			else
				throw FilePersistenceException{"Invalid string sequence when reading node id " + node->id().toString()};
		}

		node->setValue(GenericNode::STRING_VALUE, s);
	}
	else if (PREFIX_INTEGER == QString::fromLatin1(line + start, PREFIX_INTEGER.length()))
		node->setValue(GenericNode::INT_VALUE, QString::fromLatin1(line+start+PREFIX_INTEGER.length(),
				lineEnd - start - PREFIX_INTEGER.length() + 1 ));
	else if (PREFIX_DOUBLE == QString::fromLatin1(line + start, PREFIX_DOUBLE.length()))
		node->setValue(GenericNode::DOUBLE_VALUE, QString::fromLatin1(line+start+PREFIX_DOUBLE.length(),
				lineEnd - start - PREFIX_DOUBLE.length() + 1 ));
	else throw FilePersistenceException{"Unknown value prefix" +
			QString::fromUtf8(line+start, lineEnd-start+1)};
}

}
