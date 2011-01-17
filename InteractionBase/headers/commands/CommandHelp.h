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
		CommandHelp(Visualization::Item* visualization = NULL);
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
