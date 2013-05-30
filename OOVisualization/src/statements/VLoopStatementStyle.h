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

#include "../oovisualization_api.h"

#include "VisualizationBase/src/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/src/layouts/SequentialLayoutStyle.h"
#include "VisualizationBase/src/items/VListStyle.h"
#include "VisualizationBase/src/items/StaticStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VLoopStatementStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle header_;
		Visualization::StaticStyle icon_;
		Visualization::SequentialLayoutStyle initStep_;
		Visualization::SequentialLayoutStyle condition_;
		Visualization::SequentialLayoutStyle updateStep_;
		Visualization::VListStyle body_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& header() const;
		const Visualization::StaticStyle& icon() const;

		const Visualization::SequentialLayoutStyle& initStep() const;
		const Visualization::SequentialLayoutStyle& condition() const;
		const Visualization::SequentialLayoutStyle& updateStep() const;
		const Visualization::VListStyle& body() const;
};

inline const Visualization::SequentialLayoutStyle& VLoopStatementStyle::header() const { return header_; }
inline const Visualization::StaticStyle& VLoopStatementStyle::icon() const { return icon_; }

inline const Visualization::SequentialLayoutStyle& VLoopStatementStyle::initStep() const { return initStep_; }
inline const Visualization::SequentialLayoutStyle& VLoopStatementStyle::condition() const { return condition_; }
inline const Visualization::SequentialLayoutStyle& VLoopStatementStyle::updateStep() const { return updateStep_; }
inline const Visualization::VListStyle& VLoopStatementStyle::body() const { return body_; }

}
