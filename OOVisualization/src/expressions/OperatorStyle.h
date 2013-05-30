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

#include "VisualizationBase/src/items/StaticStyle.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API OperatorStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::StaticStyle preSymbol_;
		Visualization::StaticStyle inSymbol_;
		Visualization::StaticStyle in2Symbol_;
		Visualization::StaticStyle postSymbol_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::StaticStyle& preSymbol() const;
		const Visualization::StaticStyle& inSymbol() const;
		const Visualization::StaticStyle& in2Symbol() const;
		const Visualization::StaticStyle& postSymbol() const;
};

inline const Visualization::SequentialLayoutStyle& OperatorStyle::layout() const { return layout_; }
inline const Visualization::StaticStyle& OperatorStyle::preSymbol() const { return preSymbol_; }
inline const Visualization::StaticStyle& OperatorStyle::inSymbol() const { return inSymbol_; }
inline const Visualization::StaticStyle& OperatorStyle::in2Symbol() const { return in2Symbol_; }
inline const Visualization::StaticStyle& OperatorStyle::postSymbol() const { return postSymbol_; }

class OOVISUALIZATION_API OperatorSequenceStyle : public Visualization::ItemStyle
{
	private:
		QVector<OperatorStyle> seq_;

	public:
		void load(Visualization::StyleLoader& sl);

		const OperatorStyle& op(int i) const;
		const OperatorStyle& op(bool b) const;
};

inline const OperatorStyle& OperatorSequenceStyle::op(int i) const { return seq_.at(i); }
inline const OperatorStyle& OperatorSequenceStyle::op(bool b) const { return b ? seq_.at(1) : seq_.first(); }

}
