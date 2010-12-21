/***********************************************************************************************************************
 * PanelLayout.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PANELLAYOUT_H_
#define PANELLAYOUT_H_

#include "visualizationbase_api.h"

#include "Layout.h"
#include "PanelLayoutStyle.h"
#include "SequentialLayout.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelLayout: public Layout
{
	public:
		typedef PanelLayoutStyle StyleType;

	private:
		PanelLayoutStyle* style;

		Item* first_;
		Item* middle_;
		Item* last_;

		int minimalLength;

		void setItem(Item* item, Item*& position);

	protected:
		virtual const PanelLayoutStyle* getStyle() const;

	public:
		PanelLayout(Item* parent, PanelLayoutStyle* style = PanelLayoutStyle::getDefault());
		static const QString& className();
		void setStyle(PanelLayoutStyle* style = PanelLayoutStyle::getDefault());

		void setFirst(Item* item);
		void setMiddle(Item* item);
		void setLast(Item* item);

		template <class T> T* first();
		template <class T> T* middle();
		template <class T> T* last();

		void setMinimalLength(int minimalLength);
		virtual void updateState();
};

inline const QString& PanelLayout::className() {static QString name("PanelLayout"); return name;}

inline void PanelLayout::setFirst(Item* item) { setItem(item, first_); }
inline void PanelLayout::setMiddle(Item* item) { setItem(item, middle_); }
inline void PanelLayout::setLast(Item* item) { setItem(item, last_); }

template <class T> inline T* PanelLayout::first() { return static_cast<T*> (first_); };
template <class T> inline T* PanelLayout::middle() { return static_cast<T*> (middle_); };
template <class T> inline T* PanelLayout::last() { return static_cast<T*> (last_); };

inline void PanelLayout::setMinimalLength(int length) { minimalLength = length; setUpdateNeeded(); };

}

#endif /* PANELLAYOUT_H_ */
