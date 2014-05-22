/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#pragma once

#include "../modelbase_api.h"
#include "PersistentStore.h"

namespace Model {

class MODELBASE_API PersistedNode
{
	public:
		PersistedNode();
		virtual ~PersistedNode();

		void setId(NodeIdType id);
		void setType(const QString& type);
		void setName(const QString& name);
		void setNewPersistenceUnit(bool newPersistenceUnit);

		NodeIdType id() const;
		const QString& type() const;
		const QString& name() const;
		bool isNewPersistenceUnit() const;

	private:
		NodeIdType id_;
		QString type_;
		QString name_;
		bool isNewPersistenceUnit_;
};

inline void PersistedNode::setId(NodeIdType id) { id_ = id; }
inline void PersistedNode::setType(const QString& type) { type_ = type; }
inline void PersistedNode::setName(const QString& name) { name_ = name; }
inline void PersistedNode::setNewPersistenceUnit(bool newPersistenceUnit) { isNewPersistenceUnit_ = newPersistenceUnit; }

inline NodeIdType PersistedNode::id() const { return id_; }
inline const QString& PersistedNode::type() const { return type_; }
inline const QString& PersistedNode::name() const { return name_; }
inline bool PersistedNode::isNewPersistenceUnit() const { return isNewPersistenceUnit_; }

}
