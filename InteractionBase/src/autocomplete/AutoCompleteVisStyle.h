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

/*
 * AutoCompleteVisStyle.h
 *
 *  Created on: Jul 24, 2012
 *      Author: Dimitar Asenov
 */

#ifndef InteractionBase_AUTOCOMPLETEVISSTYLE_H_
#define InteractionBase_AUTOCOMPLETEVISSTYLE_H_

#include "interactionbase_api.h"
#include "VisualizationBase/src/layouts/SequentialLayoutStyle.h"
#include "VisualizationBase/src/items/StaticStyle.h"

namespace Interaction {

class INTERACTIONBASE_API AutoCompleteVisStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::StaticStyle noProposals_;
		int heightLimit_;
		int distanceToCursor_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::StaticStyle&  noProposals() const;
		int heightLimit() const;
		int distanceToCursor() const;
};

inline const Visualization::SequentialLayoutStyle& AutoCompleteVisStyle::layout() const {return layout_; }
inline const Visualization::StaticStyle& AutoCompleteVisStyle::noProposals() const {return noProposals_; }
inline int AutoCompleteVisStyle::heightLimit() const {return heightLimit_; }
inline int AutoCompleteVisStyle::distanceToCursor() const {return distanceToCursor_; }

} /* namespace Interaction */
#endif /* InteractionBase_AUTOCOMPLETEVISSTYLE_H_ */
