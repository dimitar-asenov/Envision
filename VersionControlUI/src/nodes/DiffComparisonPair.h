/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "../versioncontrolui_api.h"

#include "VisualizationBase/src/nodes/UINode.h"

#include "ModelBase/src/nodes/nodeMacros.h"
#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/nodes/Text.h"

namespace VersionControlUI {
	class DiffComparisonPair;
}

extern template class VERSIONCONTROLUI_API Model::TypedList<VersionControlUI::DiffComparisonPair>;

namespace VersionControlUI {

struct ObjectPathCrumbData
{
	QString name;
	QString type;
	QString path;
};

class VERSIONCONTROLUI_API DiffComparisonPair : public Super<Visualization::UINode>
{
	NODE_DECLARE_STANDARD_METHODS(DiffComparisonPair)

	public:
		DiffComparisonPair(Model::Node* oldVersionNode, Model::Node* newVersionNode);

		virtual QJsonValue toJson() const override;

		Model::Node* oldVersionNode();
		Model::Node* newVersionNode();

		Model::Text* newVersionObjectPath();
		Model::Text* oldVersionObjectPath();

		Model::Text* singleObjectPath();

		bool twoObjectPathsDefined();
		QString comparisonName();

		QList<ObjectPathCrumbData*> objectPathCrumbsDataOldNode();
		QList<ObjectPathCrumbData*> objectPathCrumbsDataNewNode();


	private:
		bool twoObjectPathsDefined_{};

		Model::Node* oldVersionNode_{};
		Model::Node* newVersionNode_{};

		Model::Text* newVersionObjectPath_{};
		Model::Text* oldVersionObjectPath_{};

		Model::Text* singleObjectPath_{};

		QString comparisonName_;

		QString computeComponentName();

		QList<ObjectPathCrumbData*> computeObjectPathCrumbData(Model::Node* node, QString& objectPath);

		QString computeObjectPath(Model::Node* node);

		void computeObjectPathCrumbs(Model::Node* oldNode, QString oldNodeObjectPath,
											  Model::Node* newNode, QString newNodeObjectPath);

		void computeObjectPath();

		void setComparisonName(Model::Node* node, QString nodeObjectPath, QString componentName);

		QList<ObjectPathCrumbData*> objectPathCrumbsDataOldNode_;
		QList<ObjectPathCrumbData*> objectPathCrumbsDataNewNode_;

};

inline Model::Node* DiffComparisonPair::newVersionNode() {return newVersionNode_;}
inline Model::Node* DiffComparisonPair::oldVersionNode() {return oldVersionNode_;}

inline Model::Text* DiffComparisonPair::newVersionObjectPath() {return newVersionObjectPath_;}
inline Model::Text* DiffComparisonPair::oldVersionObjectPath() {return oldVersionObjectPath_;}

inline Model::Text* DiffComparisonPair::singleObjectPath() {return singleObjectPath_;}

inline bool DiffComparisonPair::twoObjectPathsDefined() {return twoObjectPathsDefined_;}

inline QString DiffComparisonPair::comparisonName() { return comparisonName_; }

inline QList<ObjectPathCrumbData*> DiffComparisonPair::objectPathCrumbsDataOldNode()
{ return objectPathCrumbsDataOldNode_; }

inline QList<ObjectPathCrumbData*> DiffComparisonPair::objectPathCrumbsDataNewNode()
{ return objectPathCrumbsDataNewNode_; }

}
