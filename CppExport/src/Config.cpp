/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "Config.h"

namespace CppExport {

const Config& Config::instance()
{
	static Config conf;
	return conf;
}

QHash<QString, QString> Config::createMap(QJsonObject config, const QString& key) const
{
	QHash<QString, QString> result;
	auto obj = config[key].toObject();
	for (auto it = obj.begin(); it != obj.end(); ++it)
		result.insert(it.key(), it.value().toString());
	return result;
}

QSet<QString> Config::createSet(QJsonObject config, const QString& key) const
{
	QSet<QString> result;
	auto obj = config[key].toArray();
	for (auto it = obj.begin(); it != obj.end(); ++it)
		result.insert(it->toString());
	return result;
}

Config::Config()
{
	QFile configFile{"cpp-export-settings/config.json"};
	bool open = configFile.open(QIODevice::ReadOnly);
	Q_ASSERT(open);
	QJsonParseError err;
	auto doc = QJsonDocument::fromJson(configFile.readAll(), &err);
	if (err.error != QJsonParseError::NoError)
		qDebug() << err.errorString();
	Q_ASSERT(err.error == QJsonParseError::NoError);

	dependencyUnitMergeMap_ = createMap(doc.object(), "DependencyUnitMergeMap");
	separateTemplateImplementationSet_ = createSet(doc.object(), "SeparateTemplateImplementationSet");
}

}
