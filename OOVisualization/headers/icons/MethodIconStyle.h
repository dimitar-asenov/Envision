/***********************************************************************************************************************
 * MethodIconStyle.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHODICONSTYLE_H_
#define METHODICONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/icons/IconStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API MethodIconStyle :public Visualization::IconStyle
{
	private:
		int width_;
		int numTeeth_;
		int innerRadius_;
		int middleRadius_;
		int outterRadius_;
		QPointF posFirst_;
		QPointF posSecond_;
		QBrush brush_;
		QPainterPath path_;

		void computePath();

	public:
		virtual void load();

		int width() const;
		int numTeeth() const;
		int innerRadius() const;
		int middleRadius() const;
		int outterRadius() const;
		const QPointF& posFirst() const;
		const QPointF& posSecond() const;
		const QBrush& brush() const;
		const QPainterPath& path() const;

		static MethodIconStyle* getDefault();
};

inline int MethodIconStyle::width() const { return width_; }
inline int MethodIconStyle::numTeeth() const { return numTeeth_; }
inline int MethodIconStyle::innerRadius() const { return innerRadius_; }
inline int MethodIconStyle::middleRadius() const { return middleRadius_; }
inline int MethodIconStyle::outterRadius() const { return outterRadius_; }
inline const QPointF& MethodIconStyle::posFirst() const { return posFirst_; }
inline const QPointF& MethodIconStyle::posSecond() const { return posSecond_; }
inline const QBrush& MethodIconStyle::brush() const { return brush_; }
inline const QPainterPath& MethodIconStyle::path() const { return path_; }


}

#endif /* METHODICONSTYLE_H_ */
