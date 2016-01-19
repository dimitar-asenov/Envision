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

#include "CAlloy.h"
#include "VisualizationBase/src/overlays/BoxOverlay.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"
#include "Comments/src/nodes/CommentNode.h"
#include "Comments/src/items/VCommentBrowser.h"
#include "OOModel/src/declarations/Class.h"

namespace Alloy {

CAlloy::CAlloy() : CreateNamedObjectWithAttributes("alloy",
		{{}})
{}

Interaction::CommandResult* CAlloy::executeNamed(Visualization::Item* source, Visualization::Item* /*target*/,
	const std::unique_ptr<Visualization::Cursor>&, const QString& /*name*/, const QStringList& /*attributes*/)
{
	QString tempAlloyPath = QDir::tempPath() + "/alloy";

	auto targetNode = source->node();
	while (!DCast<OOModel::Class>(targetNode))
	{
		targetNode = targetNode->parent();
	}

	AlloyExporter::exportTree(targetNode, tempAlloyPath);

	QString inputFile = tempAlloyPath + "/model.als";
	QString outputDirectory = tempAlloyPath + "/output/";

	QProcess aProcess;
	aProcess.setWorkingDirectory(QDir::currentPath());
	aProcess.start("java -jar AlloyIntegrationCLI.jar " + inputFile + " " + outputDirectory + " " +
						QString::number(MAX_IMAGES));
	aProcess.waitForFinished();

	QDir dir(outputDirectory);
	QString jsArray = "var pictureArray = new Array();\n";
	int i = 0;
	for (auto dirFile: dir.entryList())
	{
		if (dirFile.endsWith(".png"))
		{
			jsArray += "pictureArray[" + QString::number(i) + "] = new Image()\n";
			jsArray += "pictureArray[" + QString::number(i) + "].src = '" + dirFile + "'\n";
			i = i+1;
		}
	}

	QFile jsfile(outputDirectory + "pictureArray.js");
	jsfile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&jsfile);
	out << jsArray;
	jsfile.close();

	QFile::copy(QDir::currentPath() + "/alloy/AlloyModels.html", outputDirectory + "/AlloyModels.html");

	auto scene = source->scene();
	auto alloyModelGroup = scene->overlayGroup("AlloyModels");
	if (!alloyModelGroup) alloyModelGroup = scene->addOverlayGroup("AlloyModels");
	alloyModelGroup->clear();

	auto anURl = QUrl::fromLocalFile(tempAlloyPath + "/output/AlloyModels.html");
	auto aBrowserComment = new Comments::VCommentBrowser{nullptr, anURl};
	aBrowserComment->updateSize(QSize(720, 540));
	aBrowserComment->browser()->settings()->setObjectCacheCapacities(0, 0, 0); //disable the cache

	alloyModelGroup->addOverlay(makeOverlay( new Visualization::BoxOverlay(source,
		[aBrowserComment](Visualization::BoxOverlay* self){
		self->content() = aBrowserComment;
		aBrowserComment->setParentItem(self);
		return QString("AlloyModels");
	})));

	return new Interaction::CommandResult{};
}

}
