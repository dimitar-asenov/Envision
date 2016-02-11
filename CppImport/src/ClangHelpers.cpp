/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "ClangHelpers.h"

#include "OOModel/src/declarations/Project.h"

namespace CppImport {

QString ClangHelpers::spelling(clang::SourceRange sourceRange) const
{
	clang::SourceLocation b = sourceManager_->getSpellingLoc(sourceRange.getBegin());
	clang::SourceLocation e = clang::Lexer::getLocForEndOfToken(sourceManager_->getSpellingLoc(sourceRange.getEnd()), 0,
																					*sourceManager_, preprocessor_->getLangOpts());

	// this can help in preventing an uncaught exception due to questionable input
	//if (sourceManager_->getPresumedLoc(b).getFilename() != sourceManager_->getPresumedLoc(e).getFilename()) return "";

	bool invalid = true;
	auto beginPtr = sourceManager_->getCharacterData(b, &invalid);
	Q_ASSERT(!invalid);

	auto endPtr = sourceManager_->getCharacterData(e, &invalid);
	Q_ASSERT(!invalid);

	auto length = endPtr - beginPtr;
	return 0 < length ? QString::fromStdString(std::string(beginPtr, endPtr - beginPtr)) : "";
}

QString ClangHelpers::unexpandedSpelling(clang::SourceRange range) const
{
	auto result = spelling(getUnexpandedRange(range));
	while (result.startsWith("\\")) result = result.right(result.length() - 1);
	return result.trimmed();
}

clang::SourceRange ClangHelpers::getUnexpandedRange(clang::SourceRange sourceRange) const
{
	auto start = sourceRange.getBegin();
	auto end = sourceRange.getEnd();

	if (// start is a macro location and...
		 start.isMacroID() &&
		 // start was expanded from a macro argument or...
		 (sourceManager_->isMacroArgExpansion(start) ||
		 // start was expanded from a "virtual" macro i.e. it is not written anywhere (identifier concatentation etc.)
		 !sourceManager_->getFileEntryForID(sourceManager_->getFileID(sourceManager_->getSpellingLoc(start)))))
		start = sourceManager_->getImmediateExpansionRange(start).first;

	if (// end is a macro location and...
		 end.isMacroID() &&
		 // end was expanded from a macro argument or...
		 (sourceManager_->isMacroArgExpansion(end) ||
		 // end was expanded from a "virtual" macro i.e. it is not written anywhere (identifier concatentation etc.)
		 !sourceManager_->getFileEntryForID(sourceManager_->getFileID(sourceManager_->getSpellingLoc(end)))))
		end = sourceManager_->getImmediateExpansionRange(end).second;

	return clang::SourceRange{start, end};
}

ClangHelpers::ClangHelpers(OOModel::Project* rootProject, QString rootProjectPath)
	: rootProject_{rootProject}, rootProjectPath_{rootProjectPath} {}

clang::SourceLocation ClangHelpers::immediateMacroLocation(clang::SourceLocation location) const
{
	// this code is an adaptation of clang::lexer::immediateMacroName

	if (location.isMacroID())
		while (true)
		{
			auto fileID = sourceManager_->getFileID(location);
			auto sourceLocationEntry = &sourceManager_->getSLocEntry(fileID);
			if (!sourceLocationEntry->isExpansion()) break;

			auto expansion = sourceLocationEntry->getExpansion();
			location = expansion.getExpansionLocStart();
			if (!expansion.isMacroArgExpansion()) break;

			location = sourceManager_->getImmediateExpansionRange(location).first;
			auto spellingLocation = expansion.getSpellingLoc();
			if (spellingLocation.isFileID())	break;

			auto macroFileID = sourceManager_->getFileID(location);
			if (sourceManager_->isInFileID(spellingLocation, macroFileID))	break;

			location = spellingLocation;
		}

	return location;
}

void ClangHelpers::immediateSpellingHistory(clang::SourceLocation location,
														  QList<clang::SourceLocation>& result) const
{
	result.append(location);

	auto next = sourceManager_->getImmediateSpellingLoc(location);

	if (next != location)
		immediateSpellingHistory(next, result);
}

QList<QString> ClangHelpers::argumentNames(const clang::MacroDirective* definition) const
{
	QList<QString> result;

	for (auto i = definition->getMacroInfo()->arg_begin(); i != definition->getMacroInfo()->arg_end(); i++)
		result.append(QString::fromStdString((*i)->getName().str()));

	return result;
}

void ClangHelpers::deleteNode(Model::Node* node)
{
	QList<Model::Node*> workList{node};
	while (!workList.empty())
	{
		auto current = workList.takeLast();
		workList << current->children();
		envisionToClangMap_.remove(current);
	}

	SAFE_DELETE(node);
}

QStringList ClangHelpers::folderNamesFromPath(QString path)
{
	QStringList result;
	QDir dir{path};
	while (dir.cdUp())
	{
		if (dir.absolutePath() == rootProjectPath_) break;
		result.append(dir.dirName());
	}
	result.removeLast();
	return result;
}

void ClangHelpers::insertFieldInFolder(OOModel::Field* field, clang::SourceLocation location,
													 OOModel::Declaration* parentNonFolderDeclaration)
{
	auto folder = folderForLocation(location, parentNonFolderDeclaration);
	if (auto container = DCast<OOModel::Project>(folder)) container->fields()->append(field);
	else if (auto container = DCast<OOModel::Module>(folder)) container->fields()->append(field);
	else Q_ASSERT(false);
}

void ClangHelpers::insertClassInFolder(OOModel::Class* classs, clang::SourceLocation location,
													 OOModel::Declaration* parentNonFolderDeclaration)
{
	auto folder = folderForLocation(location, parentNonFolderDeclaration);
	if (auto container = DCast<OOModel::Project>(folder)) container->classes()->append(classs);
	else if (auto container = DCast<OOModel::Module>(folder)) container->classes()->append(classs);
	else Q_ASSERT(false);
}

void ClangHelpers::insertMethodInFolder(OOModel::Method* method, clang::SourceLocation location,
													 OOModel::Declaration* parentNonFolderDeclaration)
{
	auto folder = folderForLocation(location, parentNonFolderDeclaration);
	if (auto container = DCast<OOModel::Project>(folder)) container->methods()->append(method);
	else if (auto container = DCast<OOModel::Module>(folder)) container->methods()->append(method);
	else Q_ASSERT(false);
}

void ClangHelpers::insertSubDeclarationInFolder(OOModel::Declaration* subDeclaration, clang::SourceLocation location,
																OOModel::Declaration* parentNonFolderDeclaration)
{
	auto folder = folderForLocation(location, parentNonFolderDeclaration);
	if (auto container = DCast<OOModel::Project>(folder)) container->subDeclarations()->append(subDeclaration);
	else if (auto container = DCast<OOModel::Module>(folder)) container->subDeclarations()->append(subDeclaration);
	else Q_ASSERT(false);
}

OOModel::Declaration* ClangHelpers::folderForLocation(clang::SourceLocation location,
																		OOModel::Declaration* parentNonFolderDeclaration)
{
	Q_ASSERT(parentNonFolderDeclaration);
	Q_ASSERT(DCast<OOModel::Module>(parentNonFolderDeclaration) || DCast<OOModel::Project>(parentNonFolderDeclaration));

	auto currentFolder = parentNonFolderDeclaration;
	auto presumedLocation = sourceManager()->getPresumedLoc(location);
	auto folderNames = folderNamesFromPath(presumedLocation.getFilename());
	while (!folderNames.empty())
	{
		auto requestedFolderName = folderNames.takeLast();
		auto moduleFound = false;
		Model::TypedList<OOModel::Module>* modules{};
		if (auto currentProject = DCast<OOModel::Project>(currentFolder))
			modules = currentProject->modules();
		else if (auto currentModule = DCast<OOModel::Module>(currentFolder))
			modules = currentModule->modules();

		for (auto childFolder : *modules)
			if (childFolder->name() == requestedFolderName)
			{
				currentFolder = childFolder;
				moduleFound = true;
				break;
			}
		if (!moduleFound)
		{
			auto newFolder = new OOModel::Module{requestedFolderName, OOModel::Module::ModuleKind::Folder};
			if (auto currentProject = DCast<OOModel::Project>(currentFolder))
				currentProject->modules()->append(newFolder);
			else if (auto currentModule = DCast<OOModel::Module>(currentFolder))
				currentModule->modules()->append(newFolder);
			currentFolder = newFolder;
		}
	}

	Q_ASSERT(DCast<OOModel::Module>(currentFolder) || DCast<OOModel::Project>(currentFolder));
	return currentFolder;
}

OOModel::Project* ClangHelpers::projectForLocation(clang::SourceLocation location)
{
	auto presumedLocation = sourceManager()->getPresumedLoc(location);
	auto parentName = projectNameFromPath(presumedLocation.getFilename());
	if (!parentName.isEmpty())
	{
		auto project = projectByName(parentName);
		if (!project)
		{
			project = new OOModel::Project{parentName};
			rootProject_->projects()->append(project);
			projects_.insert(parentName, project);
		}
		return project;
	}
	return rootProject_;
}

QString ClangHelpers::projectNameFromPath(QString path)
{
	QRegularExpression regex{rootProjectPath_ + "/(\\w+)/"};
	auto m = regex.match(QDir{path}.absolutePath());
	if (m.hasMatch())
		return m.captured(1);
	return {};
}

}
