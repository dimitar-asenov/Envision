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
 * GridBasedOffsetProvider.h
 *
 *  Created on: Jun 11, 2012
 *      Author: Dimitar Asenov
 */

#ifndef OOInteraction_GRIDBASEDOFFSETPROVIDER_H_
#define OOInteraction_GRIDBASEDOFFSETPROVIDER_H_

#include "../oointeraction_api.h"
#include "StringOffsetProvider.h"

namespace OOInteraction {

class Cell;

class OOINTERACTION_API GridBasedOffsetProvider : public StringOffsetProvider {
	public:
		GridBasedOffsetProvider(Visualization::Item* vis);
		~GridBasedOffsetProvider();

		virtual int offset(Qt::Key key);
		virtual void setOffset(int newOffset);

		void add(Cell* cell);

	private:
		QList<Cell*> cells_;
		QSize size_;

		enum Direction {Left, Up, Right, Down};
		Cell* findCell(const QRect& start, Direction dir) const;

		bool isOnTop(Cell* cell) const;
		bool isOnLeft(Cell* cell) const;
		bool isOnBottom(Cell* cell) const;
		bool isOnRight(Cell* cell) const;
};

} /* namespace OOInteraction */
#endif /* OOInteraction_GRIDBASEDOFFSETPROVIDER_H_ */
