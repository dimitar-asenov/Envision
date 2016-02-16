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

#pragma once

#include "../filepersistence_api.h"
#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class GenericNode;
class GenericPersistentUnit;

class FILEPERSISTENCE_API Parser {
	public:

		const static QString PREFIX_STRING;
		const static QString PREFIX_INTEGER;
		const static QString PREFIX_DOUBLE;

		static void parseLine(GenericNode* node, const char* line, int lineLength);

		/**
		 * Saves the generic node and its children recursively to a text stream.
		 * If any persistentUnitTypes are provided, they are saved as new peristent unit reference only
		 * and are returned in the output list.
		 */
		static QList<GenericNode*> save(QTextStream& stream, GenericNode* node,
												  const QStringList& persistentUnitTypes = {}, int tabLevel = 0);

		static GenericNode* load(const QString& filename, bool lazy, GenericPersistentUnit& persistentUnit);
		static GenericNode* load(const char* data, int dataLength, bool lazy, GenericPersistentUnit& persistentUnit);


		/**
		 * This comparator is used to sort children lists by label.
		 */
		static bool labelComparator(GenericNode* const node1, GenericNode* const node2);

	private:
		static int countTabs(const char* data, int lineStart, int lineEnd);
		static QString rawStringToQString(const char* data, int start, int endInclusive);
		static QString escape(const QString& line);

		static Model::NodeIdType toId(const char* data, int start, int endInclusive, bool& ok);
		static uchar hexDigitToChar(char d, bool& ok);

		static bool nextNonEmptyLine(const char* data, int dataSize, int& lineStart, int& lineEnd);
		static int indexOf(const char c, const char* data, int start, int endInclusive);
		static bool nextHeaderPart(const char* data, int& start, int&endInclusive, int lineEnd);

		/**
		 * If true, sort children by label when writing encoding.
		 * This is to ensure consistency between all methods used to produce Envision encodings.
		 */
		static constexpr bool SORT_BY_LABEL = true;
};

}
