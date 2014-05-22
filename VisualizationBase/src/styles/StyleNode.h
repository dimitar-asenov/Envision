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

#include "../visualizationbase_api.h"

namespace Visualization {

class VISUALIZATIONBASE_API StyleNode
{
	public:
		/**
		 * Creates a node representing the requested style.
		 *
		 * \a folderWithinStyles is the folder that contains the style file \a rootStyleNameAndPath. The \a
		 * rootStyleNameAndPath might be just a simple file name or can contain an additional sub-style separated by a '/'
		 * e.g. "keywordStyleFile/requiresKeyword". The latter case indicates that, the style is not the root XML node,
		 * but the is contained by the node with a name indicated by the string after the '/'.
		 */
		StyleNode(const QString& rootStyleNameAndPath, const QString& folderWithinStyles);
		virtual ~StyleNode();

		static void setBaseFolder( const QString &baseFolder);

		QString getProperty(QStringList path);
		bool hasProperty(QStringList path);

	private:
		static QString baseFolder;

		StyleNode* parent;

		QDomDocument doc;
		QString folder;

		QDomElement elem;
		QList< StyleNode* > prototypes;
		QList< StyleNode* > children;

		StyleNode(StyleNode* parent, QDomElement elem);
		StyleNode(StyleNode* parent, const QString& prototypeName, const QString& currentFolder);
		void init();

		QDomDocument openStyleDoc(const QString& path);
		QString getFolder();
		QDomElement getElement(QStringList path);
};

inline bool StyleNode::hasProperty(QStringList path) { return !getElement(path).isNull(); }

}
