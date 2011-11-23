/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * CommandHelp.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMANDHELP_H_
#define COMMANDHELP_H_

#include "interactionbase_api.h"

#include "VisualizationBase/headers/items/Item.h"

namespace Interaction {

class INTERACTIONBASE_API CommandHelp
{
	public:
		CommandHelp(Visualization::Item* visualization = nullptr);
		virtual ~CommandHelp();

		Visualization::Item* visualization();
		void setVisualization(Visualization::Item* item);

	private:
		Visualization::Item* visualization_;
};

inline Visualization::Item* CommandHelp::visualization() { return visualization_; }
inline void CommandHelp::setVisualization(Visualization::Item* item) { visualization_ = item; }

}

#endif /* COMMANDHELP_H_ */
