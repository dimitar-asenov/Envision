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
 * Cursor.h
 *
 *  Created on: Jan 26, 2012
 *      Author: Dimitar Asenov
 */

#ifndef VisualizationBase_CURSOR_H_
#define VisualizationBase_CURSOR_H_

#include "../visualizationbase_api.h"

namespace Visualization {

class CursorData;
class Item;

class VISUALIZATIONBASE_API Cursor {
	public:
		Cursor(Item* owner, Item* visualization = nullptr);
		virtual ~Cursor();

		virtual Item* owner();

		const QPoint& position();
		const QRect& region();
		Item* visualization();

		void setPosition(const QPoint& pos);
		void setRegion(const QRect& region);

		virtual bool isSame(Cursor* c);

	protected:
		void setVisualization(Item* visualization);

	private:
		QPoint position_;
		QRect region_;
		Item* owner_;
		Item* visualization_;
};

inline void Cursor::setPosition(const QPoint& pos) { position_ = pos; }
inline void Cursor::setRegion(const QRect& region) { region_ = region; }
inline const QPoint& Cursor::position() { return position_; }
inline const QRect& Cursor::region() { return region_; }
inline Item* Cursor::visualization() { return visualization_; }

} /* namespace Visualization */
#endif /* VisualizationBase_CURSOR_H_ */
