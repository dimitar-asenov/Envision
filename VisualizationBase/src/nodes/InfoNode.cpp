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

#include "../utils/InfoJavascriptFunctions.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
template class Model::TypedList<Visualization::InfoNode>;

namespace Visualization {

DEFINE_TYPE_ID_DERIVED(InfoNode, "InfoNode", )

QList<InfoNode*> InfoNode::allInfoNodes;
QHash<QString, InfoNode::InfoGetterStruct> InfoNode::allInfoGetters;

InfoNode::InfoNode(Model::Node *target)
	:Super{}, target_{target}
{
	Q_ASSERT(target);
	for (auto key : allInfoGetters.keys())
		setEnabled(key, allInfoGetters[key].enabledByDefault_);
	allInfoNodes.append(this);
}

InfoNode::InfoNode(Model::Node *target, QJsonArray enabledInfos)
	:Super{}, target_{target}
{
	Q_ASSERT(target);
	for (auto item : enabledInfos)
		setEnabled(item.toString(), true);
	allInfoNodes.append(this);
}

InfoNode::~InfoNode()
{
	allInfoNodes.removeAll(this);
	target_ = nullptr;
}

void InfoNode::updateInfo(bool isAutoUpdate)
{
	QString infoHtml = "<style> .content_bit { border: 1px solid; margin: 5px; padding: 5px; position:relative} "
					"button { border-radius: 4px; font-weight: bold; color: #0000CC; "
							 "border : 1px solid #000000; background: #FFFFFF; }"
					"button:hover { background: #0000CC; color: #FFFFFF; }"
					".close { position:absolute; top:0; right: 0; }"
					"tr:nth-child(even) { background: #CCC; }  </style>"
					"<div style=\"font-family:monospace\">";
	QString pointerAsString = QString::number(reinterpret_cast<long>(this));
	for (auto name : enabledInfoGetters_)
	{
		auto getter = allInfoGetters[name];
		//If necessary, compute the value
		if (!isAutoUpdate || getter.updatesAutomatically_
				|| !cachedInfoStrings_.contains(name))
			cachedInfoStrings_[name] = getter.getter_(target_);
		//Use the computed value to extend the info
		if (!cachedInfoStrings_[name].isEmpty())
			infoHtml += "<div class=\"content_bit\">" +  cachedInfoStrings_[name] + "<br><small>"
					"  (Layer " + name + ")</small>" + "<br>"
					"<button class=\"close\" onClick=\"operations.updateLayer('" +
							pointerAsString + "','" + name + "', " +
							QString::number((int) InfoJavascriptFunctions::HIDE_LAYER) + ")\">&#10006</button>"
					"<button onClick=\"operations.updateLayer('" +
							pointerAsString + "','" + name + "', " +
							QString::number((int) InfoJavascriptFunctions::MOVE_UP) + ")\">&#x25B2</button>"
					"<button onClick=\"operations.updateLayer('" +
							pointerAsString + "','" + name + "', " +
							QString::number((int) InfoJavascriptFunctions::MOVE_DOWN) + ")\">&#x25BC</button></div>";
	}
	infoHtml = "<html>" + infoHtml + "</div></html>";
	setInfoHtml(infoHtml);
}

void InfoNode::setInfoHtml(QString content)
{
	if (content == infoHtml_) return;
	infoHtml_ = content;
	incrementRevision();
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

void InfoNode::move(const QString& name, bool moveUp)
{
	auto index = enabledInfoGetters_.indexOf(name);
	if (index > 0 && moveUp) enabledInfoGetters_.move(index, index - 1);
	else if (index >= 0 && index < enabledInfoGetters_.size() - 1 && !moveUp)
		enabledInfoGetters_.move(index, index + 1);
}

QJsonValue InfoNode::toJson() const
{
	QJsonArray result;
	for (auto item : enabledInfoGetters_)
		result.append(item);
	return result;
}

//Class methods
void InfoNode::initType()
{
}

}
