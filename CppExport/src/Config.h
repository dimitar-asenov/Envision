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

#pragma once

#include "cppexport_api.h"

namespace CppExport {

class CPPEXPORT_API Config
{
	public:
		static const Config& instance();

		/**
		 * Returns a map of dependency unit name to dependency unit name.
		 * The key represents the dependency unit that should be merged into the dependency unit represented by the value.
		 *
		 * Example:
		 * key: "Model/NodeToDebugStringAdapter", value: "Model/Node"
		 * merges NodeToDebugStringAdapter's dependency unit into Node's dependency unit.
		 *
		 * ConfigKey: "DependencyUnitMergeMap"
		 */
		QHash<QString, QString> dependencyUnitMergeMap() const;

		/**
		 * Returns a set containing the names of all code composites which contain a separate template implemenation.
		 */
		QSet<QString> separateTemplateImplementationSet() const;

	private:
		Config();
		QJsonObject config_;

		QHash<QString, QString> dependencyUnitMergeMap_;
		QSet<QString> separateTemplateImplementationSet_;

		QHash<QString, QString> createMap(QJsonObject config, const QString& key) const;
		QSet<QString> createSet(QJsonObject config, const QString& key) const;
};

inline QHash<QString, QString> Config::dependencyUnitMergeMap() const { return dependencyUnitMergeMap_; }
inline QSet<QString> Config::separateTemplateImplementationSet() const { return separateTemplateImplementationSet_; }

}
