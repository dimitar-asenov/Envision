/***********************************************************************************************************************
 * VVariableDeclarationStyle.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VVARIABLEDECLARATIONSTYLE_H_
#define VVARIABLEDECLARATIONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VVariableDeclarationStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle container_;
		Visualization::TextStyle name_;
		Visualization::TextStyle assignmentStyle_;
		QString assignmentSymbol_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& container() const;
		const Visualization::TextStyle& name() const;
		const Visualization::TextStyle& assignmentStyle() const;
		const QString& assignmentSymbol() const;

		static VVariableDeclarationStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VVariableDeclarationStyle::container() const { return container_; }
inline const Visualization::TextStyle& VVariableDeclarationStyle::name() const { return name_; }
inline const Visualization::TextStyle& VVariableDeclarationStyle::assignmentStyle() const { return assignmentStyle_; }
inline const QString& VVariableDeclarationStyle::assignmentSymbol() const { return assignmentSymbol_; }

}

#endif /* VVARIABLEDECLARATIONSTYLE_H_ */
