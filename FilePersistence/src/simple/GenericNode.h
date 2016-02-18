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
#include "GenericTree.h"
#include "GenericPersistentUnit.h"

namespace FilePersistence {

class FILEPERSISTENCE_API GenericNode {

	public:
		enum ValueType {NO_VALUE, STRING_VALUE, INT_VALUE, DOUBLE_VALUE};

		/**
		 * Sets id, parentId, label, value and type of this node equal to \a other.
		 * If the parent changes, this node must first be detached from its parent.
		 */
		void copy(const GenericNode* other);

		bool equalTo(const GenericNode* other);

		void setLabel(const QString& label);
		void setType(const QString& type);

		/**
		 * This method must only be called if no value has been set yet.
		 */
		void setValue(ValueType type, const QString& value);
		void setValue(const QString& value);
		void setValue(double value);
		void setValue(long value);
		void setChildren(QList<GenericNode*> children);

		void resetValue(ValueType type, const QString& value);

		void setId(Model::NodeIdType id);
		void setParentId(Model::NodeIdType parentId);

		void setParent(GenericNode* parent);
		GenericNode* attachChild(GenericNode* child);
		GenericNode* child(const QString& label);
		const QList<GenericNode*>& children() const;
		bool areAllChildrenLoaded() const;
		GenericNode* parent() const;

		const QString& label() const;
		const QString& type() const;

		bool hasValue() const;
		const QString& valueAsString() const;
		long valueAsLong() const;
		double valueAsDouble() const;

		ValueType valueType() const;
		const QString& rawValue() const;

		bool hasStringValue() const;
		bool hasIntValue() const;
		bool hasDoubleValue() const;

		/**
		 * Removes the node from the tree. If \a recursive is false, this node must not have any children.
		 * If \a recursive is true, all children of this node are also removed.
		 */
		void remove(bool recursive = false);

		/**
		 * Links this node to the parent according to the parentId if the parent is loaded.
		 */
		void attachToParent();
		void detachFromParent();

		Model::NodeIdType id() const;
		Model::NodeIdType parentId() const;

		GenericPersistentUnit* persistentUnit() const;

		static const QString PERSISTENT_UNIT_TYPE;

		void linkNode(bool recursiveLink = false);

	private:
		friend class GenericTree;
		friend class GenericPersistentUnit;
		friend class PiecewiseLoader;

		/**
		 * !!!
		 * When adding new members, make sure to reset them in the various reset() methods
		 * !!!
		 */
		QString label_;
		QString type_;
		QString value_;
		ValueType valueType_{};

		Model::NodeIdType id_{};
		Model::NodeIdType parentId_{};
		GenericNode* parent_{};
		QList<GenericNode*> children_;
		bool areAllChildrenLoaded_ = false;

		/**
		 * The text line from which this node should be created.
		 */
		const char* dataLine_{};
		int dataLineLength_{};

		GenericPersistentUnit* persistentUnit_{};

		GenericNode();

		/**
		 * Loads all the properties of this node using the provided \a dataLine. Any old content of this node will be
		 * removed. If this node did not belong to an allocator, it's children will be deleted.
		 *
		 * \a dataLine should start with the indenting tabs and should end with the last content character of the node.
		 * The line should not include the final line break characters.
		 *
		 * Set \a lazy to true when loading with an allocator. In this case \a dataLine will be saved and only parsed
		 * on demand. If \a lazy is false, then the node will be immediately initialized with the provided data.
		 *
		 * Balz writes: This method should only be called from a persistent unit and that persistent unit should pass itself
		 * as the \a persistenUnit argument. Before it is called, the values of the members of \a this are undefined.
		 */
		void reset(GenericPersistentUnit* persistentUnit, const char* dataLine, int dataLineLength, bool lazy);
		void reset(GenericPersistentUnit* persistentUnit, const GenericNode* nodeToCopy);
		void reset(GenericPersistentUnit* persistentUnit);

		void ensureDataRead() const;

		GenericTree* tree() const;
		bool sameTree(const GenericNode* other);
};

inline void GenericNode::setLabel(const QString& label) { label_ = label; }
inline void GenericNode::setType(const QString& type) { type_ = type; }
inline void GenericNode::setId(Model::NodeIdType id) { id_ = id; }
inline void GenericNode::setParentId(Model::NodeIdType parentId)
{ Q_ASSERT(!parent_ || !tree()->piecewiseLoader()); parentId_ = parentId; }
inline void GenericNode::setChildren(QList<GenericNode*> children)
{ children_ = children; areAllChildrenLoaded_ = true; }

/**
 * Returns true if the return value of \a this.children() is valid.
 */
inline bool GenericNode::areAllChildrenLoaded() const { return areAllChildrenLoaded_; }

inline const QString& GenericNode::label() const { ensureDataRead(); return label_; }
inline const QString& GenericNode::type() const { ensureDataRead(); return type_; }
inline bool GenericNode::hasValue() const { ensureDataRead(); return valueType_ != NO_VALUE; }
inline GenericNode::ValueType GenericNode::valueType() const { ensureDataRead(); return valueType_; }
inline const QString& GenericNode::rawValue() const { ensureDataRead(); return value_; }
inline Model::NodeIdType GenericNode::id() const { ensureDataRead(); return id_; }
inline Model::NodeIdType GenericNode::parentId() const { ensureDataRead(); return parentId_; }

inline bool GenericNode::hasStringValue() const { ensureDataRead(); return valueType_ == STRING_VALUE; }
inline bool GenericNode::hasIntValue() const { ensureDataRead(); return valueType_ == INT_VALUE; }
inline bool GenericNode::hasDoubleValue() const { ensureDataRead(); return valueType_ == DOUBLE_VALUE; }

inline GenericPersistentUnit* GenericNode::persistentUnit() const {return persistentUnit_;}

inline bool GenericNode::equalTo(const GenericNode* other)
{
	ensureDataRead();
	other->ensureDataRead();
	return (id_ == other->id_ &&
			  label_ == other->label_ &&
			  type_ == other->type_ &&
			  valueType_ == other->valueType_ &&
			  value_ == other->value_ &&
			  parentId_ == other->parentId_);
}

inline GenericTree* GenericNode::tree() const
{ return persistentUnit_->tree();}

inline bool GenericNode::sameTree(const GenericNode* other)
{return persistentUnit_->tree() == other->persistentUnit_->tree();}

}
