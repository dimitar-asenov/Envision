/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "GitRepository.h"

#include <git2.h>

#include "../simple/Parser.h"

#include <iostream>

namespace FilePersistence {

// === Libgit2 call-back functions ===

struct GitDiffExtract
{
	IdToGenericNodeHash oldNodes_;
	IdToGenericNodeHash newNodes_;
};

struct GitTreeBlobs
{
	git_repository* repository_;
	QList<git_blob*> blobs_;
};

int gitDiffExtractFileCB(
				 const git_diff_delta *delta,
				 float /*progress*/,
				 void* /*carryAlongData*/)
{
	std::cout << "Git diff extract: " << delta->new_file.path << std::endl;
	return 0;
}

int gitDiffExtractLineCB(
				 const git_diff_delta* /*delta*/,
				 const git_diff_hunk* /*hunk*/,
				 const git_diff_line* line,
				 void* carryAlongData)
{
	GitDiffExtract* data = (GitDiffExtract*) carryAlongData;

	GenericNode* node = new GenericNode(line->content, line->content_len, false);;
	switch (line->origin)
	{
		case GIT_DIFF_LINE_ADDITION:
			// add node to newNodes
			data->newNodes_.insert(node->id(), node);
			break;
		case GIT_DIFF_LINE_DELETION:
			// add node to oldNodes
			data->oldNodes_.insert(node->id(), node);
			break;
		case GIT_DIFF_LINE_CONTEXT:
			// skip node (content did not change)
			break;
		default:
			Q_ASSERT(false);
	}
	return 0;
}

// Tree Walk Callback

int treeWalkBlobExtractionCB(const char *,
				const git_tree_entry *entry,
				void *carryAlongData)
{
	GitTreeBlobs* data = (GitTreeBlobs*) carryAlongData;

	git_object* obj = nullptr;
	int errorCode = git_tree_entry_to_object(&obj, data->repository_, entry);

	if (errorCode == 0)
	{

		if (git_object_type(obj) == GIT_OBJ_BLOB)
		{
			git_blob* blob = (git_blob*)obj;
			data->blobs_.append(blob);
		}
	}
	else
		Q_ASSERT(false);

	return 0;
}


GitRepository::GitRepository(QString path)
{
	// Bugfix: initialize git threads is usually done automatically
	// but there seem to be a bug related to libgit2 and Qtf
	git_threads_init();

	path_ = path;
	repository_ = nullptr;

	int error = git_repository_open(&(repository_), path_.toStdString().c_str());
	checkError(error);
}

GitRepository::~GitRepository()
{
	git_repository_free(repository_);

	// Bugfix: shutdown git threads is usually done automatically
	// but there seem to be a bug related to libgit2 and Qt
	git_threads_shutdown();
}

Diff GitRepository::diff(QString oldCommit, QString newCommit) const
{
	int errorCode = 0;

	DiffKind diffKind = kind(oldCommit, newCommit);
	Q_ASSERT(diffKind != DiffKind::Unspecified);


	git_diff* gitDiff = nullptr;

	git_diff_options diffOptions;
	errorCode = git_diff_init_options(&diffOptions, GIT_DIFF_OPTIONS_VERSION);
	diffOptions.context_lines = 0;

	git_tree* oldGitTree = nullptr;
	git_tree* newGitTree = nullptr;
	if (diffKind == DiffKind::IndexToWorkdir)
		errorCode = git_diff_index_to_workdir(&gitDiff, repository_, nullptr, &diffOptions);
	else
	{
		Q_ASSERT(oldCommit != nullptr);
		// parse oldCommit and get git_tree
		git_commit* oldGitCommit = parseCommit(oldCommit);
		errorCode = git_commit_tree(&oldGitTree, oldGitCommit);
		checkError(errorCode);

		if (diffKind == DiffKind::CommitToIndex)
			errorCode = git_diff_tree_to_index(&gitDiff, repository_, oldGitTree, nullptr, &diffOptions);
		else if (diffKind == DiffKind::CommitToWorkdir)
			errorCode = git_diff_tree_to_workdir_with_index(&gitDiff, repository_, oldGitTree, &diffOptions);
		else
		{
			Q_ASSERT(newCommit != nullptr);
			// parse newCommit and get git_tree
			git_commit* newGitCommit = parseCommit(newCommit);
			errorCode = git_commit_tree(&newGitTree, newGitCommit);
			checkError(errorCode);
			errorCode = git_diff_tree_to_tree(&gitDiff, repository_, oldGitTree, newGitTree, &diffOptions);
		}
	}
	checkError(errorCode);

	// Use callback on diff to extract node information -> oldNodes & newNodes
	GitDiffExtract carryAlongData;
	git_diff_foreach(gitDiff, gitDiffExtractFileCB, NULL, gitDiffExtractLineCB, &(carryAlongData));

	// get parent information of oldNodes & newNodes
	switch (diffKind)
	{
		case DiffKind::IndexToWorkdir:
			findParentsInGitIndex(carryAlongData.oldNodes_);
			findParentsInGitWorkdir(carryAlongData.newNodes_);
			break;

		case DiffKind::CommitToWorkdir:
			findParentsInGitTree(carryAlongData.oldNodes_, oldGitTree);
			findParentsInGitWorkdir(carryAlongData.newNodes_);
			break;

		case DiffKind::CommitToIndex:
			findParentsInGitTree(carryAlongData.oldNodes_, oldGitTree);
			findParentsInGitIndex(carryAlongData.newNodes_);
			break;

		case DiffKind::CommitToCommit:
			findParentsInGitTree(carryAlongData.oldNodes_, oldGitTree);
			findParentsInGitTree(carryAlongData.newNodes_, newGitTree);
			break;

		default:
			Q_ASSERT(false);
	}

	return Diff(carryAlongData.oldNodes_, carryAlongData.newNodes_);
}


// Private methods

void GitRepository::findParentsInGitTree(IdToGenericNodeHash nodes, git_tree* tree) const
{
	int errorCode = 0;

	// extract blobs here and store them
	GitTreeBlobs carryAlongData;
	carryAlongData.repository_ = repository_;

	errorCode = git_tree_walk(tree, GIT_TREEWALK_PRE, treeWalkBlobExtractionCB, &carryAlongData);
	checkError(errorCode);

	for (git_blob* blob : carryAlongData.blobs_)
	{
		const char* rawContent = (const char*)git_blob_rawcontent(blob);
		qint64 contentSize = git_blob_rawsize(blob);

		//extractParents(nodes, rawContent, contentSize);
		// FIXME use parser
		(void) nodes;
		(void) rawContent;
		(void) contentSize;
	}
}

void GitRepository::findParentsInGitIndex(IdToGenericNodeHash nodes) const
{
	// TODO
	(void) nodes;
}

void GitRepository::findParentsInGitWorkdir(IdToGenericNodeHash nodes) const
{
	(void) nodes;
	// TODO
}

void GitRepository::extractParents(IdToGenericNodeHash nodes, GenericNode* root) const
{
	QList<GenericNode*> nodeList = nodes.values();
	GenericNode* current = nullptr;
	for (GenericNode* node : nodeList)
	{
		current = root->find(node->id());
		if (current != nullptr)
		{
			// found node in tree
			if (current->parent() != nullptr)
				node->setParent(copyGenericNode(current->parent()));
			else
				node->setParent(nullptr);
		}
	}

}

GenericNode* GitRepository::copyGenericNode(const GenericNode* node)
{
	GenericNode* copy = new GenericNode();

	copy->setName(node->name());
	copy->setType(node->type());
	copy->setValue(node->valueType(), node->rawValue());

	copy->setId(node->id());
	copy->setParent(node->parent());

	// children list stays empty
	return copy;
}

git_commit* GitRepository::parseCommit(QString commit) const
{
	int errorCode = 0;
	git_object* obj = nullptr;

	errorCode = git_revparse_single(&obj, repository_, commit.toStdString().c_str());

	switch (errorCode)
	{
		// FIXME pass errors to the next level
		case GIT_ENOTFOUND:
			std::cout << "Error: " << commit.toStdString().c_str() << " not found!" << std::endl;
			Q_ASSERT(false);

		case GIT_EAMBIGUOUS:
			std::cout << "Error: " << commit.toStdString().c_str() << " is ambiguous!" << std::endl;
			Q_ASSERT(false);

		case GIT_EINVALIDSPEC:
			std::cout << "Error: " << commit.toStdString().c_str() << " is invalid!" << std::endl;
			Q_ASSERT(false);

		default:
			checkError(errorCode);
	}

	if (git_object_type(obj) == GIT_OBJ_TAG)
	{
		git_tag* tag = (git_tag*)obj;
		git_object* dereferencedTarget = nullptr;
		errorCode = git_tag_peel(&dereferencedTarget, tag);
		checkError(errorCode);
		// free unused objects
		git_tag_free(tag);

		obj = dereferencedTarget;
	}

	if (git_object_type(obj) == GIT_OBJ_COMMIT)
		return (git_commit*)obj;
	else
	{
		std::cout << "Error: " << commit.toStdString().c_str() << " is not a commit!" << std::endl;
		Q_ASSERT(false);
	}

}

GitRepository::DiffKind GitRepository::kind(QString oldCommit, QString newCommit)
{
	DiffKind diffKind{};

	if (oldCommit == nullptr && newCommit == nullptr)
		diffKind = DiffKind::IndexToWorkdir;
	else if (newCommit == nullptr)
		diffKind = DiffKind::CommitToWorkdir;
	else if (newCommit.compare("INDEX", Qt::CaseSensitive) == 0)
		diffKind = DiffKind::CommitToIndex;
	else
		diffKind = DiffKind::CommitToCommit;

	return diffKind;
}

void GitRepository::checkError(int errorCode)
{
	if (errorCode < 0)
	{
		const git_error* lastError = giterr_last();
		qDebug() << "Error " << errorCode << "/" << lastError->klass << ": " << lastError->message;
		exit(errorCode);
	}
}

} /* namespace FilePersistence */
