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

#include "../layouts/PanelBorderLayoutStyle.h"
#include "../layouts/SequentialLayout.h"

namespace Visualization {

class VISUALIZATIONBASE_API VCompositeStyle : public ItemStyle
{
	private:
		PanelBorderLayoutStyle borderStyle_;
		SequentialLayoutStyle headerStyle_;
		SequentialLayoutStyle smallHeaderStyle_;
		SequentialLayoutStyle attributesStyle_;
		bool expanded_;

	public:
		VCompositeStyle();
		void load(StyleLoader& sl);

		const PanelBorderLayoutStyle& borderStyle() const;
		const SequentialLayoutStyle&  headerStyle() const;
		const SequentialLayoutStyle&  smallHeaderStyle() const;
		const SequentialLayoutStyle&  attributesStyle() const;
		bool expanded() const;
};

inline const PanelBorderLayoutStyle& VCompositeStyle::borderStyle() const {return borderStyle_; }
inline const SequentialLayoutStyle& VCompositeStyle::headerStyle() const {return headerStyle_; }
inline const SequentialLayoutStyle& VCompositeStyle::smallHeaderStyle() const {return smallHeaderStyle_; }
inline const SequentialLayoutStyle& VCompositeStyle::attributesStyle() const {return attributesStyle_; }
inline bool VCompositeStyle::expanded() const { return expanded_; }

}