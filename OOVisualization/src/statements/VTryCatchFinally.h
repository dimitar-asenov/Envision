/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "../oovisualization_api.h"
#include "VTryCatchFinallyStyle.h"
#include "VStatementItem.h"

#include "OOModel/src/statements/TryCatchFinallyStatement.h"
#include "VisualizationBase/src/items/LayoutProvider.h"

namespace Visualization {
	class Static;
	class SequentialLayout;
	class PanelBorderLayout;
	class VList;
}

namespace OOVisualization {

class VStatementItemList;

class OOVISUALIZATION_API VTryCatchFinally
	: public VStatementItem<VTryCatchFinally,
	  Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::TryCatchFinallyStatement>
{
	ITEM_COMMON(VTryCatchFinally)

	public:
		VTryCatchFinally(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VTryCatchFinally();

		Visualization::Static* tryIcon() const;
		Visualization::SequentialLayout* contents() const;
		VStatementItemList* tryBody() const;

		Visualization::VList* catchClauses() const;

		Visualization::PanelBorderLayout* finallyOutline() const;
		Visualization::Static* finallyIcon() const;
		VStatementItemList* finallyBody() const;

	protected:
		void determineChildren();

	private:
		typedef VStatementItem<VTryCatchFinally,
			Visualization::LayoutProvider<Visualization::PanelBorderLayout>,
			OOModel::TryCatchFinallyStatement> BaseItemType;

		Visualization::Static* tryIcon_{};
		Visualization::SequentialLayout* contents_{};
		VStatementItemList* tryBody_{};

		Visualization::VList* catchClauses_{};

		Visualization::PanelBorderLayout* finallyOutline_{};
		Visualization::Static* finallyIcon_{};
		VStatementItemList* finallyBody_{};
};

inline Visualization::Static* VTryCatchFinally::tryIcon() const { return tryIcon_; }
inline Visualization::SequentialLayout* VTryCatchFinally::contents() const {return contents_; }
inline VStatementItemList* VTryCatchFinally::tryBody() const {return tryBody_; }

inline Visualization::VList* VTryCatchFinally::catchClauses() const {return catchClauses_; }

inline Visualization::PanelBorderLayout* VTryCatchFinally::finallyOutline() const {return finallyOutline_; }
inline Visualization::Static* VTryCatchFinally::finallyIcon() const {return finallyIcon_; }
inline VStatementItemList* VTryCatchFinally::finallyBody() const {return finallyBody_; }

} /* namespace OOVisualization */
