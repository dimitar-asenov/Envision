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
#include "../FilePersistenceException.h"
#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class GenericNodeAllocator;

class FILEPERSISTENCE_API GenericNode {
	friend class GenericNodeAllocator;

	public:
		GenericNode();
		~GenericNode();

		void setName(const QString& name);
		void setType(const QString& type);

		void setValue(const QString& value);
		void setValue(double value);
		void setValue(long value);

		void setId(Model::NodeIdType id);

		GenericNode* addChild(GenericNode* child);
		GenericNode* child(const QString& name);
		const QList<GenericNode*>& children() const;

		const QString& name() const;
		const QString& type() const;

		bool hasValue() const;
		const QString& valueAsString() const;
		long valueAsLong() const;
		double valueAsDouble() const;

		bool hasStringValue() const;
		bool hasIntValue() const;
		bool hasDoubleValue() const;

		GenericNode* find(Model::NodeIdType id);

		Model::NodeIdType id() const;

		void save(QTextStream& stream, int tabLevel = 0);
		static GenericNode* load(const QString& filename, bool lazy, GenericNodeAllocator* allocator);

	private:
		// //////////////////////////////////////////////////////////////////////////////////////////
		// !!!
		// Make sure to reset all the members in the resetForLoading() method
		// !!!

		QString name_;
		QString type_;
		QString value_;

		enum ValueType {NO_VALUE, STRING_VALUE, INT_VALUE, DOUBLE_VALUE};
		ValueType valueType_{};

		Model::NodeIdType id_{};
		QList<GenericNode*> children_;

		char* data_{};
		int lineStartInData_{};
		int lineEndInData_{};
		// //////////////////////////////////////////////////////////////////////////////////////////

		void resetForLoading(char* data, int lineStart, int lineEndInclusive);
		void setValue(ValueType type, const QString& value);
		void ensureDataRead() const;

		static void parseData(GenericNode* node, char* data, int start, int lineEnd);

		static int countTabs(char* data, int lineStart, int lineEnd);
		static QString rawStringToQString(char* data, int startAt, int endInclusive);
		static QString escape(const QString& line);

		static Model::NodeIdType toId(char* data, int start, int endInclusive, bool& ok);
		static uchar hexDigitToChar(char d, bool& ok);

		static bool nextNonEmptyLine(char* data, int dataSize, int& lineStart, int& lineEnd);
		static int indexOf(const char c, char* data, int start, int endInclusive);
		static bool nextHeaderPart(char* data, int& start, int&endInclusive, int lineEnd);
};

inline void GenericNode::setName(const QString& name) { name_ = name; }
inline void GenericNode::setType(const QString& type) { type_ = type; }
inline void GenericNode::setId(Model::NodeIdType id) { id_ = id; }

inline const QString& GenericNode::name() const { ensureDataRead(); return name_; }
inline const QString& GenericNode::type() const { ensureDataRead(); return type_; }
inline bool GenericNode::hasValue() const { ensureDataRead(); return valueType_ != NO_VALUE; }
inline Model::NodeIdType GenericNode::id() const { ensureDataRead(); return id_; }
inline const QList<GenericNode*>& GenericNode::children() const { ensureDataRead(); return children_; }

inline bool GenericNode::hasStringValue() const { ensureDataRead(); return valueType_ == STRING_VALUE; }
inline bool GenericNode::hasIntValue() const { ensureDataRead(); return valueType_ == INT_VALUE; }
inline bool GenericNode::hasDoubleValue() const { ensureDataRead(); return valueType_ == DOUBLE_VALUE; }

inline void GenericNode::ensureDataRead() const
{
	if (data_)
	{
		parseData(const_cast<GenericNode*>(this), data_, lineStartInData_, lineEndInData_);
		// Don't delete this, just mark it unused. The allocator will delete it.
		const_cast<GenericNode*>(this)->data_ = nullptr;
	}
}

} /* namespace FilePersistence */
