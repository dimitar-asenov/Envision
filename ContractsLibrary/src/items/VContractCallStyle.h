/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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
 * VContractCallStyle.h
 *
 *  Created on: May 23, 2012
 *      Author: Dimitar Asenov
 */

#ifndef ContractsLibrary_VCONTRACTCALLSTYLE_H_
#define ContractsLibrary_VCONTRACTCALLSTYLE_H_

#include "../contractslibrary_api.h"

#include "VisualizationBase/src/items/StaticStyle.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/items/VListStyle.h"

namespace ContractsLibrary {

class CONTRACTSLIBRARY_API VContractCallStyle : public Visualization::ItemStyle
{
	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::StaticStyle& keyword() const;
		const Visualization::VListStyle& arguments() const;

	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::StaticStyle keyword_;
		Visualization::VListStyle arguments_;
};

inline const Visualization::SequentialLayoutStyle& VContractCallStyle::layout() const { return layout_; }
inline const Visualization::StaticStyle& VContractCallStyle::keyword() const { return keyword_; }
inline const Visualization::VListStyle& VContractCallStyle::arguments() const { return arguments_; }

} /* namespace ContractsLibrary */
#endif /* ContractsLibrary_VCONTRACTCALLSTYLE_H_ */
