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

#pragma once

#include "filepersistence_api.h"
#include "FilePersistenceException.h"
#include "NodeIdMap.h"

namespace FilePersistence {

class FILEPERSISTENCE_API GenericNode {
	public:
		GenericNode();
		~GenericNode();

		void setName(const QString& name);
		void setType(const QString& type);

		void setName(const QStringRef& name);
		void setType(const QStringRef& type);

		void setValue(const QString& value);
		void setValue(double value);
		void setValue(long value);

		void setId(NodeIdMap::NodeIdType id);

		GenericNode* addChild(GenericNode* child);
		bool hasChildren() const;
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

		GenericNode* find(NodeIdMap::NodeIdType id);

		NodeIdMap::NodeIdType id() const;

		void save(QTextStream& stream, int tabLevel = 0);
		static GenericNode* load(const QString& filename);

	private:
		QString name_;
		QString type_;
		QString value_;

		enum ValueType {NO_VALUE, STRING_VALUE, INT_VALUE, DOUBLE_VALUE};
		ValueType valueType_{};

		NodeIdMap::NodeIdType id_{-1};
		QList<GenericNode*> children_;

		void setValue(ValueType type, const QString& value);
		void setValue(ValueType type, const QStringRef& value);

		static int countTabs(const QString& line);
		static QString unEscape(const QString& line, int startAt);
		static QString escape(const QString& line);

		static int findNextNonWhiteSpace(const QString& line, int startPos, int before);
		static int findNextWhiteSpace(const QString& line, int startPos, int before);
		static int refToId(const QStringRef& ref, bool& ok);
};

inline void GenericNode::setName(const QString& name) { name_ = name; }
inline void GenericNode::setType(const QString& type) { type_ = type; }
inline void GenericNode::setName(const QStringRef& name) { Q_ASSERT(name_.isEmpty()); name_.append(name); }
inline void GenericNode::setType(const QStringRef& type) { Q_ASSERT(type_.isEmpty()); type_.append(type); }
inline void GenericNode::setId(NodeIdMap::NodeIdType id) { id_ = id; }

inline const QString& GenericNode::name() const { return name_; }
inline const QString& GenericNode::type() const { return type_; }
inline bool GenericNode::hasValue() const { return valueType_ != NO_VALUE; }
inline NodeIdMap::NodeIdType GenericNode::id() const { return id_; }
inline bool GenericNode::hasChildren() const { return !children_.isEmpty(); }
inline const QList<GenericNode*>& GenericNode::children() const { return children_; }

inline bool GenericNode::hasStringValue() const { return valueType_ == STRING_VALUE; }
inline bool GenericNode::hasIntValue() const { return valueType_ == INT_VALUE; }
inline bool GenericNode::hasDoubleValue() const { return valueType_ == DOUBLE_VALUE; }

inline void GenericNode::setValue(ValueType type, const QString& value)
{
	Q_ASSERT(children_.isEmpty());
	Q_ASSERT(valueType_ == NO_VALUE);
	Q_ASSERT(type != NO_VALUE);

	valueType_ = type;
	value_ = value;
}

inline void GenericNode::setValue(ValueType type, const QStringRef& value)
{
	Q_ASSERT(children_.isEmpty());
	Q_ASSERT(valueType_ == NO_VALUE);
	Q_ASSERT(type != NO_VALUE);

	valueType_ = type;
	Q_ASSERT(value_.isEmpty());
	value_.append(value);
}

} /* namespace FilePersistence */
