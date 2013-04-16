/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "../visualizationbase_api.h"
#include "VisualizationGroup.h"

namespace Visualization {

class Item;

class VISUALIZATIONBASE_API ModelRenderer
{
	public:
		ModelRenderer();
		virtual ~ModelRenderer();

		/**
		 * Returns an Item which has a parent \a parent and is a rendering of \a node.
		 *
		 * If \a purpose is specified (\a purpose >= 0) it is used as the target purpose when determining an appropriate
		 * visualization. Otherwise the purpose of \a parent is used. If \a parent is nullptr the default purpose is used.
		 */
		virtual Item* render(Item* parent, Model::Node* node, int purpose = -1);

		/**
		 * Returns whether a specific visualization has been registered for nodes of type \a nodeTypeId  and the provided
		 * \a purpose.
		 */
		bool hasVisualization(int nodeTypeId, int purpose = 0);
		void registerVisualization(int nodeTypeId, int purpose, VisualizationGroup::ItemConstructor visualization);
		void registerVisualization(int nodeTypeId, VisualizationGroup::ItemConstructor visualization);
		void registerGroup(int nodeTypeId, int purpose, VisualizationGroup* group);
		void registerGroup(int nodeTypeId, VisualizationGroup* group);

		int registerVisualizationPurpose(const QString& name);
		int numRegisteredPurposes() const;
		QString purposeName(int purpose);

	private:
		QVector<QVector<VisualizationGroup*>> groups_;
		QVector<QString > purposes_;
};

inline void ModelRenderer::registerVisualization(int nodeTypeId, VisualizationGroup::ItemConstructor visualization)
{
	registerVisualization(nodeTypeId, 0, visualization);
}

inline void ModelRenderer::registerGroup(int nodeTypeId, VisualizationGroup* group)
{
	registerGroup(nodeTypeId, 0, group);
}

template<class VIS, class NODE>
Item* createVisualization(Item* parent, Model::Node* node)
{
	return new VIS(parent, static_cast<NODE*> (node));
}

inline int ModelRenderer::numRegisteredPurposes() const { return purposes_.size(); }
inline QString ModelRenderer::purposeName(int purpose) { return purposes_[purpose]; }


}
