/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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

#include "InfoNode.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(Visualization::InfoNode)

namespace Visualization {

DEFINE_TYPE_ID_DERIVED(InfoNode, "InfoNode", )

QList<InfoNode*> InfoNode::allInfoNodes;
QHash<QString, InfoNode::InfoGetterStruct> InfoNode::allInfoGetters;

InfoNode::InfoNode(Model::Node *target)
	:Super(nullptr), target_(target)
{
	Q_ASSERT(target);
	for (auto key : allInfoGetters.keys())
		setEnabled(key, allInfoGetters[key].enabledByDefault_);
	allInfoNodes.append(this);
}

InfoNode::InfoNode(Model::Node *target, QList<QString> enabledInfos)
	:Super(nullptr), target_(target)
{
	Q_ASSERT(target);
	for (auto key : enabledInfos)
		setEnabled(key, true);
	allInfoNodes.append(this);
}

InfoNode::~InfoNode()
{
	allInfoNodes.removeAll(this);
	target_ = nullptr;
}

void InfoNode::save(Model::PersistentStore &) const
{
	Q_ASSERT(false);
}

void InfoNode::load(Model::PersistentStore &)
{
	Q_ASSERT(false);
}

void InfoNode::updateInfo(bool isAutoUpdate)
{
	QString infoHtml;
	for (auto name : enabledInfoGetters_)
	{
		auto getter = allInfoGetters[name];
		//If necessary, compute the value
		if (!isAutoUpdate || getter.updatesAutomatically_
				|| !cachedInfoStrings_.contains(name))
			cachedInfoStrings_[name] = getter.getter_(target_);
		//Use the computed value to extend the info
		if (!cachedInfoStrings_[name].isEmpty())
			infoHtml += cachedInfoStrings_[name] + "<small>" + "  (Layer " + name + ")</small>" + "<br>";
	}
	setInfoHtml(infoHtml);
}

void InfoNode::registerInfoGetter(const QString &name, const InfoGetter getter,
								  bool updatesAutomatically, bool enabledByDefault)
{
	if (!allInfoGetters.contains(name))
		allInfoGetters[name] = InfoGetterStruct{getter, updatesAutomatically, enabledByDefault};
	for (auto node : allInfoNodes)
		node->setEnabled(name, enabledByDefault);
}

void InfoNode::setEnabled(const QString name, bool isEnabled)
{
	if (isEnabled && !enabledInfoGetters_.contains(name) && allInfoGetters.contains(name))
		enabledInfoGetters_.append(name);
	else enabledInfoGetters_.removeAll(name);
}

//Class methods
void InfoNode::initType()
{
}

}
