/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "../interactionbase_api.h"

namespace Interaction {

class AutoCompleteVis;
class AutoCompleteEntry;

class INTERACTIONBASE_API AutoComplete {
	public:

		/**
		 * Schedules an event that will show the auto completion menu on the next scene update.
		 *
		 * The menu will be shown with the provided \a entries. If \a explicitSelection is set no default selection
		 * will be made and the user will be required to explicitly select a menu item.
		 */
		static void show(const QList<AutoCompleteEntry*> entries, bool explicitSelection = false);

		/**
		 * Schedules an event that will hide the auto completion menu on the next scene update.
		 *
		 * Use this method when you do not want to immediately hide the visualization but wait until the next update. If
		 * show() is called before the next update, the visualization is never hidden. This way flickering can be avoided.
		 */
		static void hide();

		static bool isVisible();

		static AutoCompleteVis* visualization();

	private:
		static AutoCompleteVis* vis_;
		static bool hideRequested_;

		/**
		 * Updates the existing AutoComplete visualization or adds a new one to the top level items of a scene.
		 */
		static void showNow(const QList<AutoCompleteEntry*>& entries, bool explicitSelection);

		/**
		 * Hides the auto completion menu immediately.
		 *
		 * This method removes the menu from the scene and deletes the visualization.
		 */
		static void hideNow();
};

}
