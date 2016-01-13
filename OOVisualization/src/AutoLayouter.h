/***********************************************************************************************************************
**
** Copyright (c) 2011, 2016 ETH Zurich
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

#include "oovisualization_api.h"

#include "OOModel/src/allOOModelNodes.h"
#include "VisualizationBase/src/node_extensions/Position.h"

namespace OOVisualization {

constexpr float TEXT_SIZE_IN_PIXELS = 15;
constexpr float HEIGHT_TO_WIDTH_RATIO = 3.0f / 2.0f;
constexpr float TEXT_HEIGHT = TEXT_SIZE_IN_PIXELS;
constexpr float TEXT_WIDTH = TEXT_HEIGHT/HEIGHT_TO_WIDTH_RATIO;

constexpr float MIN_WIDTH = TEXT_WIDTH;
constexpr float MIN_HEIGHT = TEXT_HEIGHT;

class OOVISUALIZATION_API AutoLayouter
{
	public:
		class Size
		{
			public:
				float width = MIN_WIDTH;
				float height = MIN_HEIGHT;

				Size(float width, float height)
				{
					this->width = width;
					this->height = height;
				}

				void addHorizontally(Size size)
				{
					width += size.width;
					if (size.height > height) height = size.height;
				}

				void addVertically(Size size)
				{
					height += size.height;
					if (size.width > width) width = size.width;
				}

				void addHorizontally(Model::Node* node)
				{
					if (!node) return;
					addHorizontally(estimatedSize(node));
				}

				void addVertically(Model::Node* node)
				{
					if (!node) return;
					addVertically(estimatedSize(node));
				}
		};


		enum Mode {Estimated, Measured};
		static void arrange(Model::Node* tree, Mode mode);

	private:
		enum Direction {Horizontally, Vertically};
		enum Major {ColumnMajor, RowMajor};


		static Size arrange(Direction d, int spaceBetween, QList<Model::Node*> nodes);
		static Size arrangeWithBody(Model::Node* nodeWithBody, Model::Node* body);
		static Size arrangeIf(OOModel::IfStatement* ifs);
		static Size arrangeMethod(OOModel::Method* me);
		static Size arrangeClass(OOModel::Class* cl);
		static Size arrangeModule(OOModel::Module* mo);
		static Size arrangeProject(OOModel::Project* pr);

		static Size estimateSize(Model::Node* node);
		static Size estimatedSize(Model::Node* node);

		static Size arrangeNodesInGrid(QList<Model::Node*> nodes, Major major);

		static QHash<Model::Node*, AutoLayouter::Size>& estimatedSizeMap();
};

}
