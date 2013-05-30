/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#pragma once

#include "../oointeraction_api.h"

namespace Model {
	class Node;
}

namespace Visualization {
	class Item;
	class VList;
}

namespace OOInteraction {

class OOINTERACTION_API StringOffsetProvider {
	public:

		// This is needed in order to enable default adapters. See the AdapterManager class for more info.
		using BaseAdapteeType = Visualization::Item;

		StringOffsetProvider(Visualization::Item* item);

		virtual QString string();
		virtual int offset(Qt::Key key) = 0;
		virtual void setOffset(int newOffset) = 0;
		virtual ~StringOffsetProvider();

		/**
		 * \brief Returns true when the visualization corresponding to this StringProvider is an indivisible entity.
		 *
		 * An indivisible entity's offset should be ignored and the cursor should be placed either before or after it.
		 * This is important for example for Static text symbols which are rendered as text but actually are represented
		 * in the expression editor as different text. The same is true of icons which have a text representation.
		 */
		virtual bool isIndivisible();

		Visualization::Item* item() const;

		// Helper methods
		static QStringList components(Model::Node* node);
		static QString stringFromComponenets(Model::Node* node);
		static QString stringFromComponenets(Visualization::Item* item);
		static QString stringFromStringOffsetProvider(Visualization::Item* item);
		static bool setOffsetInItem(int offset, Visualization::Item* item);
		static int itemOffset(Visualization::Item* item, int stringComponentLenght, Qt::Key key);
		static bool setOffsetInListItem(int& offset, Visualization::VList* list,
				const QString& prefix, const QString& separator, const QString& postfix);
		static int listItemOffset(Visualization::VList* list,
				const QString& prefix, const QString& separator, const QString& postfix, Qt::Key key);

		static StringOffsetProvider* defaultProvider(Visualization::Item* item);

	protected:
		// Helper methods
		virtual QStringList components();

	private:
		Visualization::Item* vis_;
};

inline Visualization::Item* StringOffsetProvider::item() const { return vis_; }

} /* namespace OOInteraction */
