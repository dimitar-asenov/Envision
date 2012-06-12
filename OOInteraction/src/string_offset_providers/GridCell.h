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
 * GridCell.h
 *
 *  Created on: Jun 11, 2012
 *      Author: Dimitar Asenov
 */

#ifndef OOInteraction_GRIDCELL_H_
#define OOInteraction_GRIDCELL_H_

#include "../oointeraction_api.h"
#include "StringOffsetProvider.h"

namespace OOInteraction {

class OOINTERACTION_API GridCell
{
	public:
		GridCell(int x, Visualization::Item* item, int stringComponentsStart, int stringComponentsEnd = -1);
		GridCell(int x, int y, Visualization::Item* item, int stringComponentsStart, int stringComponentsEnd = -1);
		GridCell(int x, int y, int width, int height, Visualization::Item* item, int stringComponentsStart,
				int stringComponentsEnd = -1);

		const QRect& region() const;
		int x() const;
		int y() const;
		Visualization::Item* item() const;
		int stringComponentsStart() const;
		int stringComponentsEnd() const;

	private:
		QRect region_;
		Visualization::Item* item_;
		int stringComponentsStart_;
		int stringComponentsEnd_;
};

inline const QRect& GridCell::region() const { return region_; }
inline int GridCell::x() const { return region_.x(); }
inline int GridCell::y() const { return region_.y(); }
inline Visualization::Item* GridCell::item() const { return item_; }
inline int GridCell::stringComponentsStart() const { return stringComponentsStart_; }
inline int GridCell::stringComponentsEnd() const { return stringComponentsEnd_; }

} /* namespace OOInteraction */
#endif /* OOInteraction_GRIDCELL_H_ */
