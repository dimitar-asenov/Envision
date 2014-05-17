#ifndef COMMENTDIAGRAMTOOLBAR_H
#define COMMENTDIAGRAMTOOLBAR_H

#include <QToolBar>
#include <QToolButton>
#include <QLabel>
#include "qtcolorpicker.h"

#include "nodes/CommentDiagram.h"

#include "VisualizationBase/src/items/Item.h"

namespace Comments {

class CommentDiagramToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit CommentDiagramToolbar(QWidget *parent = 0);
	 CommentDiagram* diagram_;
	 Visualization::Item* currentShape_;

    void setCurrentShape(Visualization::Item *currentShape);

private:
	 QToolButton* bRectangle_;
	 QToolButton* bEllipse_;
	 QToolButton* bDiamond_;

	 QtColorPicker* colorPickerBackground_;
	 QtColorPicker* colorPickerBorder_;
	 QtColorPicker* colorPickerText_;

	 QMap<QString, Qt::GlobalColor> mapQtColors_;
	 QMap<QString, QColor> mapTextColors_;

signals:


public slots:
    void createRectangle();
    void createEllipse();
    void createDiamond();
    void applyBackgroundColor();
    void applyBorderColor();
    void applyTextColor();
};

} /* namespace Comments */

#endif // COMMENTDIAGRAMTOOLBAR_H
