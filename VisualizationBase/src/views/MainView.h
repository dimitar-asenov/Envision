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
 * MainView.h
 *
 *  Created on: Jan 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MAINVIEW_H_
#define MAINVIEW_H_

#include "../visualizationbase_api.h"

#include "View.h"
#include "MiniMap.h"

namespace Visualization {

class Scene;

class VISUALIZATIONBASE_API MainView: public View
{
	public:
		MainView(Scene *scene);
		virtual ~MainView();

		void setMiniMapSize(int width, int height);

		static const int MINIMAP_DEFAULT_WIDTH = 200;
		static const int MINIMAP_DEFAULT_HEIGHT = 200;

		qreal scaleFactor() const;

	protected:
		virtual bool event(QEvent *event);
		virtual void resizeEvent( QResizeEvent *event );
		virtual void wheelEvent(QWheelEvent *event);
		virtual void scrollContentsBy(int dx, int dy);
		virtual void keyPressEvent (QKeyEvent *event);


	private:
		MiniMap* miniMap;

		static const int SCALING_FACTOR = 2;
		int scaleLevel;
};

}

#endif /* MAINVIEW_H_ */
