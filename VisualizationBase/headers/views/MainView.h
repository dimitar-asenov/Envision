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
