/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#pragma once

#include "core_api.h"

namespace Core {

class EnvisionManager;

/**
 * This EnvisionPlugin interface is the one that each plug-in must implement.
 *
 * The Envision core will look for this interface (via the QT plug-in framework) in each plug-in.
 */
class CORE_API EnvisionPlugin
{
	public:

		/**
		 * Initializes the plug-in.
		 *
		 * This function is called by the core system as soon as the plug-in is loaded. All initializatoin should happen
		 * within this function.
		 *
		 * @param manager
		 * 				The EnvisionManager object that can be used to query various aspects of the Envision system.
		 *
		 * @returns
		 * 				True if the initialization was successful, false otherwise.
		 */
		virtual bool initialize(EnvisionManager& manager) = 0;

		/**
		 * Frees up all resources allocated by this plug-in and prepares it to be unloaded.
		 *
		 * This function is called when Envision is about to exit or when a plug-in has manually been unloaded. At the
		 * time when this function is called there will be no plug-ins dependent on this one.
		 */
		virtual void unload() = 0;

		/**
		 * Starts the indicated self-test for this plug-in.
		 *
		 * If the Envision system was started with a command line argument that indicates a particular plug-in should be
		 * tested, this method will be invoked for that plug-in. Optionally a particular test can be requested.
		 *
		 * This method will only be invoked once all plug-ins are loaded.
		 *
		 * To request that a plug-in be tested use the --test option when starting envision.
		 *
		 * E.G. to run all tests of the File persistence module:
		 * Envision --test filepersistence
		 *
		 * to only run the 'load' test:
		 * Envision --test filepersistence:load
		 *
		 * @param testid
		 * 				The string id of the test to run. If this string is empty all tests should be run.
		 */
		virtual void selfTest(QString testid) = 0;

		virtual ~EnvisionPlugin() = default;
};

}

Q_DECLARE_INTERFACE(Core::EnvisionPlugin, "EnvisionPlugin/1.0")
