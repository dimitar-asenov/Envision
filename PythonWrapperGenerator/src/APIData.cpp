/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "APIData.h"

struct ClassDataNode
{
		ClassDataNode(QString name) : name_{name} {}
		ClassDataNode(ClassData data) : data_{data}, placeholder_{false}
		{
			name_ = data.qualifiedName_;
		}

		void setData(ClassData data)
		{
			Q_ASSERT(placeholder_);
			data_ = data;
			placeholder_ = false;
		}

		QList<ClassDataNode*> children_;
		ClassData data_;
		QString name_;
		bool placeholder_{true};
};

APIData& APIData::instance()
{
	static APIData instance;
	return instance;
}

void APIData::addIncludeFile(QString filePath)
{
	Q_ASSERT(!includePrefix_.isEmpty());
	auto parts = filePath.split(includePrefix_);
	if (parts.size() > 1)
		filePath = parts.last();
	if (!filePath.startsWith(QDir::separator()))
		filePath.prepend(QDir::separator());
	filePath.prepend(includePrefix_);
	includePaths_ << filePath;
}

void APIData::insertClassData(ClassData data, QStringList classHierarchy)
{
	// NOTE: this whole function will break for multiple inheritance, but we don't intend to have this in Envision.
	ClassDataNode* insertNode = nullptr;
	if (!classRoot_)
	{
		// No root so we just insert the new hierarchy
		classRoot_ = new ClassDataNode{classHierarchy[0]};
		insertNode = classRoot_;
		for (int i = 1; i < classHierarchy.size(); ++i)
		{
			ClassDataNode* nextNode = new ClassDataNode{classHierarchy[i]};
			insertNode->children_ << nextNode;
			insertNode = nextNode;
		}
	}
	else
	{
		auto currentNode = classRoot_;
		if (currentNode->name_ == classHierarchy[0])
		{
			// The root matches the current hierarchy just insert the missing nodes from the hierarchy.
			for (int i = 1; i < classHierarchy.size(); ++i)
			{
				QString childName = classHierarchy[i];
				ClassDataNode* nextNode = nullptr;
				for (auto child : currentNode->children_)
				{
					if (child->name_ == childName)
					{
						nextNode = child;
						break;
					}
				}
				if (!nextNode)
				{
					nextNode = new ClassDataNode{childName};
					currentNode->children_ << nextNode;
				}
				currentNode = nextNode;
			}
			insertNode = currentNode;
		}
		else
		{
			// The root does not match
			Q_ASSERT(false);
			// That should currently not happen
		}
	}
	insertNode->setData(data);
}

void APIData::insertTypeList(QString itemType)
{
	QString unqualified = itemType;
	if (unqualified.contains("::")) unqualified = unqualified.split("::").last();
	unqualified.append("s"); // Plural
	typedLists_.insert(QString("Model::TypedList<%1>").arg(itemType), unqualified);
}

QList<ClassData> APIData::classes() const
{
	QList<ClassData> result;
	std::queue<ClassDataNode*> q;
	q.push(classRoot_);
	while (!q.empty())
	{
		auto node = q.front();
		q.pop();
		if (!node) continue;
		if (!node->placeholder_) result << node->data_;
		for (auto child : node->children_)
			q.push(child);
	}
	return result;
}

QHash<QString, QString> APIData::typedLists() const
{
	return typedLists_;
}
