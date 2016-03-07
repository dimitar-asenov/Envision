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

#include "SystemClipboard.h"

#include "FilePersistenceException.h"
#include "XMLModel.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Node.h"
#include "Core/src/EnvisionException.h"

using namespace Model;

namespace FilePersistence {

static const QString CLIPBOARD_TAG = "clipboard";
const QString SystemClipboard::NULL_STRING = "____NULL____";

SystemClipboard::SystemClipboard() :
	xml{nullptr}, numNodes_{0}
{
	readClipboard();
}

SystemClipboard::~SystemClipboard()
{
	SAFE_DELETE(xml);
}

SystemClipboard* SystemClipboard::clone() const
{
	return new SystemClipboard{};
}

void SystemClipboard::saveTree(TreeManager* manager, const QString &name)
{
	if (manager->root())
	{
		SAFE_DELETE(xml);
		xml = new XMLModel{};

		xml->beginSaveChildNode(CLIPBOARD_TAG);
		saveNode(manager->root(), name);
		xml->endSaveChildNode();

		QApplication::clipboard()->setText(xml->documentText());

		SAFE_DELETE(xml);
		numNodes_ = 0;
	}
}

void SystemClipboard::saveStringValue(const QString &value)
{
	xml->saveStringValue(value);
}

void SystemClipboard::saveIntValue(int value)
{
	xml->saveIntValue(value);
}

void SystemClipboard::saveDoubleValue(double value)
{
	xml->saveDoubleValue(value);
}

void SystemClipboard::saveReferenceValue(const QString &name, const Node* target)
{
	QString nameString = name.isNull() ? (target && target->definesSymbol() ? target->symbolName() : NULL_STRING) : name;
	xml->saveStringValue(nameString);
}

void SystemClipboard::saveNode(const Node *node, const QString &name)
{
	// Do not require a fully loaded node.
	xml->beginSaveChildNode(node->typeName());
	xml->setName(name);
	node->save(*this);
	xml->endSaveChildNode();
}

Node* SystemClipboard::loadTree(TreeManager*, const QString &, bool)
{
	throw FilePersistenceException{"The clipboard does not support the loadTree() method."};
}

QList<LoadedNode> SystemClipboard::loadAllSubNodes(Node*, const QSet<QString>&)
{
	QList<LoadedNode> result;

	if ( xml->hasChildren() )
	{
		xml->goToFirstChild();
		while ( true )
		{
			LoadedNode ln = loadNode(nullptr);
			result.append(ln);
			if ( xml->hasNext() ) xml->loadNext();
			else break;
		}
		xml->goToParent();
	}

	return result;
}

Node* SystemClipboard::loadSubNode(Node* parent, const QString& name, bool)
{
	if (!xml->hasChild(name)) return nullptr;

	xml->beginLoadChildNode(name);
	LoadedNode ln = loadNode(parent);
	xml->endLoadChildNode();

	return ln.node;
}

LoadedNode SystemClipboard::loadNode(Node* parent)
{
	LoadedNode node;
	node.name = xml->getName();
	node.node = Node::createNewNode(xml->getType(), parent, *this, false);

	return node;
}

QString SystemClipboard::currentNodeType() const
{
	return xml->getType();
}

int SystemClipboard::loadIntValue()
{
	return xml->loadIntValue();
}

QString SystemClipboard::loadStringValue()
{
	return xml->loadStringValue();
}

double SystemClipboard::loadDoubleValue()
{
	return xml->loadDoubleValue();
}

QString SystemClipboard::loadReferenceValue(Reference*)
{
	QString name = xml->loadStringValue();
	if (name == NULL_STRING) name = QString{};

	return name;
}

void SystemClipboard::putNode(const Node* node)
{
	QList<const Node*> nodes;
	nodes.append(node);
	putNodes(nodes);
}

void SystemClipboard::putNodes(const QList<const Node*>& nodes)
{
	if (nodes.size() > 0)
	{
		SAFE_DELETE(xml);
		xml = new XMLModel{};
		xml->beginSaveChildNode(CLIPBOARD_TAG);

		for (int i = 0; i<nodes.size(); ++i)
		{
			xml->beginSaveChildNode(nodes[i]->typeName());
			xml->setName(QString::number(i));

			nodes[i]->save(*this);

			xml->endSaveChildNode();
		}

		xml->endSaveChildNode();
		QApplication::clipboard()->setText(xml->documentText());

		SAFE_DELETE(xml);
		numNodes_ = 0;
	}
}

bool SystemClipboard::readClipboard()
{
	numNodes_ = 0;
	SAFE_DELETE(xml);

	bool clipboardContainsEnvisionData = false;
	if (!QApplication::clipboard()->text().isEmpty())
	{
		bool oldAssertOnThrow = Core::EnvisionException::assertOnThrow();
		Core::EnvisionException::assertOnThrow() = false;
		try
		{
			xml = new XMLModel{};
			xml->setDocumentText(QApplication::clipboard()->text());
			xml->goToRoot();
			numNodes_ = xml->getChildrenNames().size();
			xml->goToFirstChild();
			clipboardContainsEnvisionData = true;
		}
		catch (FilePersistenceException&)
		{
			SAFE_DELETE(xml);
			clipboardContainsEnvisionData = false;
		}
		Core::EnvisionException::assertOnThrow() = oldAssertOnThrow;
	}

	return clipboardContainsEnvisionData && numNodes_ > 0;
}

int SystemClipboard::numNodes() const
{
	return numNodes_;
}

bool SystemClipboard::hasNext() const
{
	return xml->hasNext();
}

void SystemClipboard::next()
{
	if (xml->hasNext()) xml->loadNext();
	else throw FilePersistenceException{"Could not find next clipboard element."};
}

Node* SystemClipboard::create(TreeManager*, Node* parent)
{
	Node* node = Node::createNewNode(xml->getType(), parent, *this, false);
	return node;
}

bool SystemClipboard::isLoadingPartially() const
{
	return false;
}

}
