/***********************************************************************************************************************
 * CommandError.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMANDERROR_H_
#define COMMANDERROR_H_

#include "interactionbase_api.h"

#include "VisualizationBase/headers/items/Item.h"

namespace Interaction {

class INTERACTIONBASE_API CommandError
{
	public:
		CommandError(const QString& message = QString(), Visualization::Item* visualization = NULL);
		virtual ~CommandError();

		const QString& message();
		const QList<QString>& resolutionTips();
		Visualization::Item* visualization();

		void setMessage(const QString& message);
		void addResolutionTip(const QString& resolutionTip);
		void setVisualization(Visualization::Item* item);

	private:
		QString message_;
		QList<QString> resolutionTips_;
		Visualization::Item* visualization_;
};

inline const QString& CommandError::message() { return message_; }
inline const QList<QString>& CommandError::resolutionTips() { return resolutionTips_; }
inline Visualization::Item* CommandError::visualization() { return visualization_; }
inline void CommandError::setMessage(const QString& message) { message_ = message; }
inline void CommandError::addResolutionTip(const QString& resolutionTip) { resolutionTips_.append(resolutionTip); }
inline void CommandError::setVisualization(Visualization::Item* item) { visualization_ = item; }

}

#endif /* COMMANDERROR_H_ */
