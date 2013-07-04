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

#include "cppimport_api.h"

#include "visitors/ClangAstVisitor.h"
#include "CppImportLogger.h"

namespace CppImport {

class CPPIMPORT_API ClangFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
	public:
		/**
		 * This instantiates the logger and the astvisitor to make sure to keep
		 * the same instance over multiple translation units.
		 * The \a project is the root node where everything will be added to.
		 */
		ClangFrontendActionFactory(OOModel::Project* project);

		virtual ~ClangFrontendActionFactory() override;

		/**
		 * This creates a clang FrontendAction. The function is called for every translationunit
		 */
		virtual clang::FrontendAction* create() override;

		/**
		 * Calls the outputStatistics() method of the logger
		 * This is to display some statistics in the end of the import.
		 */
		void outputStatistics();

	private:
		ClangAstVisitor* visitor_{};
		CppImportLogger* logger_{};
};

}
