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

#ifndef OOVisualization_VTRYCATCHFINALLYSTYLE_H_
#define OOVisualization_VTRYCATCHFINALLYSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/src/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/src/layouts/SequentialLayoutStyle.h"
#include "VisualizationBase/src/items/VListStyle.h"
#include "VisualizationBase/src/items/StaticStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VTryCatchFinallyStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle layout_;
		Visualization::StaticStyle tryIcon_;
		Visualization::SequentialLayoutStyle contents_;
		Visualization::VListStyle tryBody_;

		Visualization::VListStyle catchClauses_;

		Visualization::PanelBorderLayoutStyle finallyOutline_;
		Visualization::StaticStyle finallyIcon_;
		Visualization::VListStyle finallyBody_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::PanelBorderLayoutStyle& layout() const;
		const Visualization::StaticStyle& tryIcon() const;
		const Visualization::SequentialLayoutStyle& contents() const;
		const Visualization::VListStyle& tryBody() const;

		const Visualization::VListStyle& catchClauses() const;

		const Visualization::PanelBorderLayoutStyle& finallyOutline() const;
		const Visualization::StaticStyle& finallyIcon() const;
		const Visualization::VListStyle& finallyBody() const;
};

inline const Visualization::PanelBorderLayoutStyle& VTryCatchFinallyStyle::layout() const { return layout_; }
inline const Visualization::StaticStyle& VTryCatchFinallyStyle::tryIcon() const { return tryIcon_; }
inline const Visualization::SequentialLayoutStyle& VTryCatchFinallyStyle::contents() const { return contents_; }
inline const Visualization::VListStyle& VTryCatchFinallyStyle::tryBody() const { return tryBody_; }

inline const Visualization::VListStyle& VTryCatchFinallyStyle::catchClauses() const { return catchClauses_; }

inline const Visualization::PanelBorderLayoutStyle& VTryCatchFinallyStyle::finallyOutline() const
	{ return finallyOutline_; }
inline const Visualization::StaticStyle& VTryCatchFinallyStyle::finallyIcon() const { return finallyIcon_; }
inline const Visualization::VListStyle& VTryCatchFinallyStyle::finallyBody() const { return finallyBody_; }

} /* namespace OOVisualization */
#endif /* OOVisualization_VTRYCATCHFINALLYSTYLE_H_ */
