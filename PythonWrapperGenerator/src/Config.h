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

#pragma once
#include "precompiled.h"

class Config
{
	public:
		static const Config& instance();

		/**
		 * Returns the export file path
		 *
		 * ConfigKeys: "ExportPath"
		 */
		QString exportPath() const;

		/**
		 * Path to Envision to create a wrapper.
		 *
		 * ConfigKey: "EnvisionPath"
		 */
		QString envisionReadPath() const;

		/**
		 * Returns the maximum line length in the exported file.
		 *
		 * ConfigKey: "MaxLineLength"
		 */
		int maxLineLength() const;

		/**
		 * Returns a map of subdirs which should be looked at when wrapping.
		 * The key represents the subdir path name and the value the corresponding namespace name.
		 *
		 * Example key: "ModelBase", value: "Model"
		 *
		 * ConfigKey: "SubDirs"
		 */
		QHash<QString, QString> subdirsToNamespaceMap() const;

	private:
		Config();

		QJsonObject config_;
};
