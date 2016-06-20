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

#include "Config.h"

const Config& Config::instance()
{
	static Config conf;
	return conf;
}

QString Config::exportPath() const
{
	auto path = config_["ExportPath"].toString();
	QDir dir(path);
	path = dir.absolutePath();
	if (!path.endsWith(QDir::separator())) path.append(QDir::separator());
	return path;
}

QString Config::envisionReadPath() const
{
	QDir dir(config_["EnvisionPath"].toString());
	auto path = dir.absolutePath();
	if (!path.endsWith(QDir::separator())) path.append(QDir::separator());
	return path;
}

int Config::maxLineLength() const
{
	return config_["MaxLineLength"].toInt();
}

QHash<QString, QString> Config::subdirsToNamespaceMap() const
{
	QHash<QString, QString> result;
	auto obj = config_["SubDirs"].toObject();
	for (auto it = obj.begin(); it != obj.end(); ++it)
		result.insert(it.key(), it.value().toString());
	return result;
}

Config::Config()
{
	QFile configFile(QCoreApplication::applicationDirPath() + QDir::separator() + "config.json");
	bool open = configFile.open(QIODevice::ReadOnly);
	Q_ASSERT(open);
	QJsonParseError err;
	auto doc = QJsonDocument::fromJson(configFile.readAll(), &err);
	Q_ASSERT(err.error == QJsonParseError::NoError);
	config_ = doc.object();
}
