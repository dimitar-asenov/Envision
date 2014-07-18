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
#include "../simple/GenericNodeAllocator.h"

#include <iostream>

namespace FilePersistence {


CommitContent::~CommitContent()
{
	for (const char* data : content_.values())
		delete data;
}

// === Libgit2 call-back functions ===

struct GitDiffExtract
{
	IdToGenericNodeHash oldNodes_;
	IdToGenericNodeHash newNodes_;
};

struct GitTreeBlobs
{
	git_repository* repository_;
	QList<QString> fileNames_;
	QList<git_blob*> blobs_;

	~GitTreeBlobs()
	{
		for (git_blob* blob : blobs_)
			git_blob_free(blob);
	}
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
	size_t lineLength = line->content_len;

	while (line->content[lineLength] == '\t')
		lineLength--;

	GenericNode* node = new GenericNode(line->content, lineLength, false);;
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

			QString file(git_tree_entry_name(entry));
			data->fileNames_.append(file);
		}
	}
	else
		Q_ASSERT(false);

	return 0;
}

const QString GitRepository::WORKDIR = "GitRepositoryWorkdir";
const QString GitRepository::INDEX = "GitRepositoryIndex";

GitRepository::GitRepository(QString path)
{
	// Bugfix: initialize git threads is usually done automatically
	// but there seem to be a bug related to libgit2 and Qtf
	git_threads_init();

	path_ = path;
	repository_ = nullptr;

	if (path_.endsWith(".git"))
		path_.chop(4);

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

	git_commit* oldGitCommit = nullptr;
	git_commit* newGitCommit = nullptr;
	git_tree* oldGitTree = nullptr;
	git_tree* newGitTree = nullptr;
	if (diffKind == DiffKind::IndexToWorkdir)
		errorCode = git_diff_index_to_workdir(&gitDiff, repository_, nullptr, &diffOptions);
	else
	{
		Q_ASSERT(oldCommit != nullptr);
		// parse oldCommit and get git_tree
		oldGitCommit = parseCommit(oldCommit);
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
			newGitCommit = parseCommit(newCommit);
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

	// clean up
	git_commit_free(oldGitCommit);
	git_commit_free(newGitCommit);
	git_tree_free(oldGitTree);
	git_tree_free(newGitTree);
	git_diff_free(gitDiff);

	return Diff(carryAlongData.oldNodes_, carryAlongData.newNodes_);
}

CommitGraph GitRepository::commitGraph(QString start, QString end) const
{
	git_commit* gitStartCommit = parseCommit(start);
	const git_oid* startOID = git_commit_id(gitStartCommit);
	char* sha = git_oid_allocfmt(startOID);
	QString startSHA(sha);
	delete sha;

	git_commit* gitEndCommit = parseCommit(end);
	const git_oid* endOID = git_commit_id(gitEndCommit);
	sha = git_oid_allocfmt(endOID);
	QString endSHA(sha);
	delete sha;

	CommitGraph graph(startSHA, endSHA);
	traverseCommitGraph(&graph, gitEndCommit, startOID);

	git_commit_free(gitStartCommit);
	git_commit_free(gitEndCommit);
	return graph;
}

CommitProperties GitRepository::getCommitProperties(QString commit)
{
	CommitProperties properties;

	if (commit.compare(WORKDIR) != 0 && commit.compare(INDEX) != 0)
	{
		git_commit* gitCommit = parseCommit(commit);

		const git_oid* oid = git_commit_id(gitCommit);
		char* sha = git_oid_allocfmt(oid);
		properties.sha_ = QString(sha);
		delete sha;

		const char* msg = git_commit_message(gitCommit);
		properties.message_ = QString(msg);

		git_time_t time = git_commit_time(gitCommit);
		properties.dateTime_.setTime_t(time);

		const git_signature* committer = git_commit_committer(gitCommit);
		properties.committerName_ = QString(committer->name);
		properties.committerEMail_ = QString(committer->email);

		const git_signature* author = git_commit_author(gitCommit);
		properties.authorName_ = QString(author->name);
		properties.authorEMail_ = QString(author->email);

		unsigned int parentCount = git_commit_parentcount(gitCommit);
		for (unsigned int i = 0; i < parentCount; i++)
		{
			const git_oid* parentID = git_commit_parent_id(gitCommit, i);
			char* sha = git_oid_allocfmt(parentID);
			properties.parents_.append(QString(sha));
			delete sha;
		}

		git_commit_free(gitCommit);
	}

	return properties;
}

CommitContent GitRepository::getCommitContent(QString commit) const
{
	CommitContent commitContent;
	int errorCode = 0;

	// parse commit
	git_commit* gitCommit = parseCommit(commit);
	git_tree* tree = nullptr;
	errorCode = git_commit_tree(&tree, gitCommit);
	checkError(errorCode);

	// extract blobs here and store them
	GitTreeBlobs carryAlongData;
	carryAlongData.repository_ = repository_;

	errorCode = git_tree_walk(tree, GIT_TREEWALK_PRE, treeWalkBlobExtractionCB, &carryAlongData);
	checkError(errorCode);

	for (int i = 0; i < carryAlongData.blobs_.size(); i++)
	{
		git_blob* blob = carryAlongData.blobs_.at(i);
		QString fileName = carryAlongData.fileNames_.at(i);

		const char* rawContent = (const char*)git_blob_rawcontent(blob);
		qint64 contentSize = git_blob_rawsize(blob);

		char* content = new char[contentSize];
		memcpy(content, rawContent, contentSize);

		commitContent.files_.append(fileName);
		commitContent.content_.insert(fileName, content);
		commitContent.size_.insert(fileName, contentSize);
	}

	return commitContent;
}

QString GitRepository::getSHA(QString commit) const
{
	git_commit* gitCommit = parseCommit(commit);

	const git_oid* oid = git_commit_id(gitCommit);
	char* sha = git_oid_allocfmt(oid);
	QString commitSHA(sha);
	delete sha;
	git_commit_free(gitCommit);

	return commitSHA;
}

void GitRepository::checkout(QString commit, bool force)
{
	if (commit.compare(WORKDIR) != 0)
	{
		git_checkout_options options;
		git_checkout_init_options(&options, GIT_CHECKOUT_OPTIONS_VERSION);
		if (force)
			options.checkout_strategy = GIT_CHECKOUT_FORCE;
		else
			options.checkout_strategy = GIT_CHECKOUT_SAFE;

		int errorCode = 0;
		if (commit.compare(INDEX) == 0)
			errorCode = git_checkout_index(repository_, nullptr, &options);
		else
		{
			git_commit* gitCommit = parseCommit(commit);
			errorCode = git_checkout_tree(repository_, (git_object*)gitCommit, &options);

			git_commit_free(gitCommit);
		}
		checkError(errorCode);
	}
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

		GenericNodeAllocator* allocator = new GenericNodeAllocator();
		GenericNode* root = Parser::load(rawContent, contentSize, false, allocator);
		extractParents(nodes, root);
		allocator->endThisLoad();
		delete allocator;
	}
}

void GitRepository::findParentsInGitIndex(IdToGenericNodeHash nodes) const
{
	// TODO
	(void) nodes;
}

void GitRepository::findParentsInGitWorkdir(IdToGenericNodeHash nodes) const
{
	QDir workdir = QDir(path_);
	QFileInfoList list = workdir.entryInfoList(QDir::Files | QDir::NoDot | QDir::NoDotDot);
	for (QFileInfo fileInfo : list)
	{
		GenericNodeAllocator* allocator = new GenericNodeAllocator();
		GenericNode* root = Parser::load(fileInfo.filePath(), false, allocator);
		extractParents(nodes, root);
		allocator->endThisLoad();
		delete allocator;
	}
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
	if (node->valueType() != GenericNode::ValueType::NO_VALUE)
		copy->setValue(node->valueType(), node->rawValue());

	copy->setId(node->id());
	copy->setParent(node->parent());

	// children list stays empty
	return copy;
}

void GitRepository::traverseCommitGraph(CommitGraph* graph, git_commit* current, const git_oid* target) const
{
	const git_oid* oid = git_commit_id(current);
	char* sha = git_oid_allocfmt(oid);
	QString currentSHA(sha);
	delete sha;

	if (git_oid_cmp(oid, target) != 0)
	{
		unsigned int numParents = git_commit_parentcount(current);
		for (unsigned int i = 0; i < numParents; i++)
		{
			git_commit* parent = nullptr;
			int errorCode = git_commit_parent(&parent, current, i);
			checkError(errorCode);

			const git_oid* parentOID = git_commit_id(parent);
			int parentIsTarget = git_oid_cmp(parentOID, target);
			int isConnected = git_graph_descendant_of(repository_, parentOID, target);
			if (isConnected == 1 || parentIsTarget == 0) {
				sha = git_oid_allocfmt(parentOID);
				QString parentSHA(sha);
				delete sha;

				graph->add(parentSHA, currentSHA);

				traverseCommitGraph(graph, parent, target);
			}
			else if (isConnected != 0)
				checkError(isConnected);

			git_commit_free(parent);
		}
	}
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

		// clean up
		git_tag_free(tag);
	}

	if (git_object_type(obj) == GIT_OBJ_COMMIT)
		return (git_commit*)obj;
	else
	{
		std::cout << "Error: " << commit.toStdString().c_str() << " is not a commit!" << std::endl;
		Q_ASSERT(false);
	}

	// clean up
	git_object_free(obj);
}

GitRepository::DiffKind GitRepository::kind(QString oldCommit, QString newCommit)
{
	DiffKind diffKind{};

	if (oldCommit.compare(INDEX) == 0 && newCommit.compare(WORKDIR) == 0)
		diffKind = DiffKind::IndexToWorkdir;
	else if (newCommit.compare(WORKDIR) == 0)
		diffKind = DiffKind::CommitToWorkdir;
	else if (newCommit.compare(INDEX) == 0)
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
