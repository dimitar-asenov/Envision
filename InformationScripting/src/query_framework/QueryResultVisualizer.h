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

#include "../informationscripting_api.h"

#include "../queries/LinearQuery.h"
#include "ArgumentParser.h"

namespace InformationScripting {

class HighlightOverlay;
class QueryExecutor;

class INFORMATIONSCRIPTING_API QueryResultVisualizer : public LinearQuery
{
	public:
		Optional<TupleSet> executeLinear(TupleSet input) override;

		static void registerDefaultQueries();

		Optional<int> visualize(const TupleSet& ts);

		static void cleanScene();

	private:
		friend class QueryRegistry;
		ArgumentParser arguments_;
		QueryExecutor* executor_;

		static const QString HIGHLIGHT_OVERLAY_GROUP;
		static const QString ARROW_OVERLAY_GROUP;
		static const QString MESSAGE_OVERLAY_GROUP;

		static const QStringList INFO_ARGUMENT_NAMES;
		static const QStringList SORT_ARGUMENT_NAMES;

		static constexpr int DEFAULT_ALPHA_{60};

		QueryResultVisualizer(Model::Node* target, QStringList args, QueryExecutor* executor = nullptr);

		static void showASTRelation(const TupleSet& ts, const QString& relationName);
		static QHash<Model::Node*, QString> extractColors(const TupleSet& ts);
		Optional<QHash<Model::Node*, QString>> extractInfo(const TupleSet& ts);
		Optional<QHash<Model::Node*, QString>> convertTuplesToString(const QHash<Model::Node*, QList<Tuple>>& infos);
		using TaggedValue = std::pair<QString, QString>;
		Optional<std::vector<TaggedValue>> infoArgumentValues();

		void visualizeMessages(const TupleSet& ts);

		static void setColor(HighlightOverlay* overlay, QColor color);
};

}
