/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "GitPiecewiseLoader.h"
#include "../FilePersistenceException.h"
#include "../simple/GenericNode.h"
#include "../simple/GenericPersistentUnit.h"
#include "../simple/GenericTree.h"
#include "../simple/Parser.h"

namespace FilePersistence {

/**
 * libgit callback
 */
static int gitDiffExtractFileCallBack(
				 const git_diff_delta *delta,
				 float,
				 void* carryAlongData)
{
	GitDiffExtract* data = (GitDiffExtract*) carryAlongData;

	QString relativePathA{delta->old_file.path};
	if (!data->treeA_->persistentUnit(relativePathA))
		data->treeA_->newPersistentUnit(relativePathA);

	QString relativePathB{delta->new_file.path};
	data->treeB_->newPersistentUnit(relativePathB);

	return 0;
}

/**
 * libgit callback
 */
static void createNodeAndAppend(const git_diff_line* line, const char* filePath, GenericTree* tree,
													 QList<GenericNode*>& list)
{
	// lineLength is number of chars on line EXCLUDING '\n'
	size_t lineLength = line->content_len - 1;

	QString relativePath{filePath};
	GenericPersistentUnit* unit = tree->persistentUnit(relativePath);
	Q_ASSERT(unit != nullptr);
	auto pair = unit->newOrExistingNode(line->content, lineLength);
	auto node = pair.second;

	// pair.second is null if this line is a persistence node indicator
	if (pair.second)
	{
		if (pair.first)
			node->linkNode();
		list.append(node);
	}
}

/**
 * libgit callback
 */
static int gitDiffExtractLineCallBack(const git_diff_delta* delta, const git_diff_hunk*,
															 const git_diff_line* line, void* carryAlongData)
{
	// TODO: Make sure we only process files which actually belong to the Envision
	// model!
	GitDiffExtract* data = (GitDiffExtract*) carryAlongData;

	if ((!data->reverseAB_ && line->origin == GIT_DIFF_LINE_ADDITION) ||
		 (data->reverseAB_ && line->origin == GIT_DIFF_LINE_DELETION))
	{
		// appear on side B
		createNodeAndAppend(line, delta->new_file.path, data->treeB_, data->nodesB_);
	}
	else if ((!data->reverseAB_ && line->origin == GIT_DIFF_LINE_DELETION) ||
				(data->reverseAB_ && line->origin == GIT_DIFF_LINE_ADDITION))
	{
		// appear on side A
		createNodeAndAppend(line, delta->old_file.path, data->treeA_, data->nodesA_);
	}
	else
		Q_ASSERT(false);

	return 0;
}

/**
 * libgit callback: Tree Walk Callback
 */
static int treeWalkCommitExtractCallBack(const char* root,
									 const git_tree_entry* entry,
									 void* walkData)
{
	GitCommitExtract* data = (GitCommitExtract*) walkData;

	git_object* obj = nullptr;
	int errorCode = git_tree_entry_to_object(&obj, data->repository_, entry);

	if (errorCode == 0)
	{
		if (git_object_type(obj) == GIT_OBJ_BLOB)
		{
			QString rootPath{root};
			QString fileName{git_tree_entry_name(entry)};
			QString relativePath = rootPath.append(fileName);

			git_blob* blob = (git_blob*)obj;

			qint64 contentSize = git_blob_rawsize(blob);

			std::unique_ptr<char[], CommitFileContentDeleter>
					content{(char*)git_blob_rawcontent(blob), [blob](char*){ git_blob_free(blob); }};
			data->commit_->addFile(relativePath, contentSize, std::move(content));
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
	git_libgit2_init();

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
	git_libgit2_shutdown();
}

bool GitRepository::repositoryExists(QString path)
{
	git_libgit2_init();
	git_repository* repo{};

	int error = git_repository_open(&repo, path.toStdString().c_str());
	git_repository_free(repo);
	git_libgit2_shutdown();
	return error == GIT_OK;
}

std::shared_ptr<Merge> GitRepository::merge(QString revision, bool fastForward)
{
	if (merge_.expired())
	{
		auto merge = std::shared_ptr<Merge>{new Merge{revision, fastForward, this}};
		merge_ = merge;
		return merge;
	}
	else
		Q_ASSERT(false);
}

Diff GitRepository::diff(QString revisionA, QString revisionB,
								 std::shared_ptr<GenericTree> treeA,
								 std::shared_ptr<GenericTree> treeB) const
{
	int errorCode = 0;

	QPair<SourceKind, SourceKind> diffKind = kind(revisionA, revisionB);

	git_diff* gitDiff = nullptr;

	git_diff_options diffOptions;
	errorCode = git_diff_init_options(&diffOptions, GIT_DIFF_OPTIONS_VERSION);
	checkError(errorCode);
	diffOptions.context_lines = 0;

	git_commit* gitCommitA = nullptr;
	git_commit* gitCommitB = nullptr;
	git_tree* gitTreeA = nullptr;
	git_tree* gitTreeB = nullptr;

	bool reverseAB = false;

	Q_ASSERT(diffKind.first != SourceKind::Unspecified);
	Q_ASSERT(diffKind.second != SourceKind::Unspecified);

	if ((diffKind.first == SourceKind::Workdir && diffKind.second == SourceKind::Workdir) ||
		 (diffKind.first == SourceKind::Index && diffKind.second == SourceKind::Index))
		return Diff{};

	if ((diffKind.first == SourceKind::Index && diffKind.second == SourceKind::Workdir) ||
		 (diffKind.first == SourceKind::Workdir && diffKind.second == SourceKind::Index))
	{
		errorCode = git_diff_index_to_workdir(&gitDiff, repository_, nullptr, &diffOptions);
		if (diffKind.first == SourceKind::Workdir)
			reverseAB = true;
	}
	else if (diffKind.first == SourceKind::Commit)
	{
		gitTreeA = getCommitTree(revisionA);

		if (diffKind.second == SourceKind::Workdir)
			errorCode = git_diff_tree_to_workdir_with_index(&gitDiff, repository_, gitTreeA, &diffOptions);
		else if (diffKind.second == SourceKind::Index)
			errorCode = git_diff_tree_to_index(&gitDiff, repository_, gitTreeA, nullptr, &diffOptions);
		else
		{
			Q_ASSERT(diffKind.second == SourceKind::Commit);
			gitTreeB = getCommitTree(revisionB);
			errorCode = git_diff_tree_to_tree(&gitDiff, repository_, gitTreeA, gitTreeB, &diffOptions);
		}
	}
	else
	{
		Q_ASSERT(diffKind.second == SourceKind::Commit);
		gitTreeB = getCommitTree(revisionB);
		reverseAB = true;

		if (diffKind.first == SourceKind::Workdir)
			errorCode = git_diff_tree_to_workdir_with_index(&gitDiff, repository_, gitTreeB, &diffOptions);
		else
		{
			Q_ASSERT(diffKind.first == SourceKind::Index);
			errorCode = git_diff_tree_to_index(&gitDiff, repository_, gitTreeB, nullptr, &diffOptions);
		}
	}
	checkError(errorCode);

	if (!treeA)
	{
		QString sha1A = getSHA1(revisionA);
		treeA = std::shared_ptr<GenericTree>{new GenericTree{projectName()}};
		new GitPiecewiseLoader{treeA, this, sha1A};
	}
	if (!treeB)
	{
		QString sha1B = getSHA1(revisionB);
		treeB = std::shared_ptr<GenericTree>{new GenericTree{projectName()}};
		new GitPiecewiseLoader{treeB, this, sha1B};
	}

	// Use callback on diff to extract node information
	GitDiffExtract carryAlongData;
	carryAlongData.treeA_ = treeA.get();
	carryAlongData.treeB_ = treeB.get();
	carryAlongData.reverseAB_ = reverseAB;

	git_diff_foreach(gitDiff, gitDiffExtractFileCallBack, nullptr, nullptr, gitDiffExtractLineCallBack, &(carryAlongData));

	// clean up
	git_commit_free(gitCommitA);
	git_commit_free(gitCommitB);
	git_tree_free(gitTreeA);
	git_tree_free(gitTreeB);
	git_diff_free(gitDiff);

	return Diff{carryAlongData.nodesA_, treeA,
					carryAlongData.nodesB_, treeB,
					this};
}

git_tree* GitRepository::getCommitTree(QString revision) const
{
	git_tree* gitTree = nullptr;
	auto gitCommit = parseCommit(revision);
	auto errorCode = git_commit_tree(&gitTree, gitCommit);
	checkError(errorCode);
	git_commit_free(gitCommit);
	return gitTree;
}

CommitGraph GitRepository::commitGraph(QString startRevision, QString endRevision) const
{
	QString startSHA1 = getSHA1(startRevision);
	QString endSHA1 = getSHA1(endRevision);

	git_commit* gitStartCommit = parseCommit(startRevision);
	const git_oid* startOID = git_commit_id(gitStartCommit);

	git_commit* gitEndCommit = parseCommit(endRevision);

	CommitGraph graph{startSHA1, endSHA1};
	TraverseCommitGraph(&graph, gitEndCommit, startOID);

	git_commit_free(gitStartCommit);
	git_commit_free(gitEndCommit);
	return graph;
}

const Commit* GitRepository::getCommit(QString revision) const
{
	Q_ASSERT(revision != INDEX);
	if (revision == WORKDIR)
	{
		auto commit = new Commit();
		QDirIterator DirIter(workdirPath(), QDirIterator::Subdirectories);
		while (DirIter.hasNext())
		{
			DirIter.next();
			if (DirIter.fileInfo().isFile())
			{
				QFile file(DirIter.filePath());
				if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
				QByteArray text = file.readAll();
				char *content = new char[text.size() + 1];
				strncpy(content, text.data(), text.size()+1);
				std::unique_ptr<char[]> content_ptr{content};
				commit->addFile(DirIter.filePath(), file.size(), std::move(content_ptr));
			}
		}
		return commit;
	}
	else
	{
		int errorCode = 0;

		git_commit* gitCommit = parseCommit(revision);
		git_tree* tree = nullptr;
		errorCode = git_commit_tree(&tree, gitCommit);
		checkError(errorCode);

		GitCommitExtract treeWalkData;
		treeWalkData.repository_ = repository_;
		treeWalkData.commit_ = new Commit{};

		CommitMetaData info = getCommitInformation(revision);
		treeWalkData.commit_->setMetaData(info);

		errorCode = git_tree_walk(tree, GIT_TREEWALK_PRE, treeWalkCommitExtractCallBack, &treeWalkData);
		checkError(errorCode);

		git_commit_free(gitCommit);
		git_tree_free(tree);

		return treeWalkData.commit_;
	}
}

const CommitFile* GitRepository::getCommitFile(QString revision, QString relativePath) const
{
	if (revision == WORKDIR)
		return getCommitFileFromWorkdir(relativePath);
	else if (revision == INDEX)
		return getCommitFileFromIndex(relativePath);
	else
		return getCommitFileFromTree(revision, relativePath);
}

CommitMetaData GitRepository::getCommitInformation(QString revision) const
{
	CommitMetaData info;

	if (revision != WORKDIR && revision != INDEX)
	{
		git_commit* gitCommit = parseCommit(revision);

		// SHA1
		const git_oid* oid = git_commit_id(gitCommit);
		info.sha1_ = oidToQString(oid);

		// Message
		const char* msg = git_commit_message(gitCommit);
		info.message_ = QString{msg};

		// Date and Time
		git_time_t time = git_commit_time(gitCommit);
		info.dateTime_.setTime_t(time);

		// Committer
		Signature committer;
		const git_signature* gitCommitter = git_commit_committer(gitCommit);
		committer.name_ = QString{gitCommitter->name};
		committer.eMail_ = QString{gitCommitter->email};
		info.committer_ = committer;

		// Author
		Signature author;
		const git_signature* gitAuthor = git_commit_author(gitCommit);
		author.name_ = QString{gitAuthor->name};
		author.eMail_ = QString{gitAuthor->email};
		info.author_ = author;

		git_commit_free(gitCommit);
	}

	return info;
}

QString GitRepository::getSHA1(QString revision) const
{
	if (revision != WORKDIR && revision != INDEX)
	{
		git_commit* gitCommit = parseCommit(revision);
		const git_oid* oid = git_commit_id(gitCommit);

		QString commitSHA1 = oidToQString(oid);

		git_commit_free(gitCommit);

		return commitSHA1;
	}
	else
		return revision;
}

void GitRepository::checkout(QString revision, bool force)
{
	if (revision != WORKDIR)
	{
		git_checkout_options options;
		git_checkout_init_options(&options, GIT_CHECKOUT_OPTIONS_VERSION);

		options.checkout_strategy = force ? GIT_CHECKOUT_FORCE : GIT_CHECKOUT_SAFE;

		int errorCode = 0;
		if (revision == INDEX)
			errorCode = git_checkout_index(repository_, nullptr, &options);
		else
		{
			git_commit* gitCommit = parseCommit(revision);
			errorCode = git_checkout_tree(repository_, (git_object*)gitCommit, &options);

			git_commit_free(gitCommit);
		}
		checkError(errorCode);
	}
}

QString GitRepository::currentBranchName() const
{
	HEADState state = getHEADState();
	if (state != HEADState::ATTACHED)
		return QString{};

	int errorCode = 0;

	git_reference* head = nullptr;
	errorCode = git_repository_head(&head, repository_);
	checkError(errorCode);

	const char* branch = nullptr;
	errorCode = git_branch_name(&branch, head);
	checkError(errorCode);

	QString branchName{branch};

	git_reference_free(head);

	return branchName;
}

QStringList GitRepository::localBranches() const
{
	int errorCode = 0;

	git_branch_iterator* iter = nullptr;
	git_branch_t type;

	errorCode = git_branch_iterator_new(&iter, repository_, GIT_BRANCH_LOCAL);
	checkError(errorCode);

	QStringList branches;
	git_reference* ref = nullptr;
	while (git_branch_next(&ref, &type, iter) != GIT_ITEROVER)
	{
		QString name{git_reference_name(ref)};
		name.remove("refs/heads/");
		branches.append(name);

		git_reference_free(ref);
	}

	git_branch_iterator_free(iter);

	return branches;
}

QStringList GitRepository::tags() const
{
	int errorCode = 0;

	git_strarray tags;
	errorCode = git_tag_list(&tags, repository_);
	checkError(errorCode);

	QStringList tagList;
	for (size_t i = 0; i < tags.count; i++)
		tagList.append(QString{tags.strings[i]});

	git_strarray_free(&tags);

	return tagList;
}

QStringList GitRepository::revisions() const
{
	int error = 0;

	git_revwalk* walker = nullptr;
	error = git_revwalk_new(&walker, repository_);
	checkError(error);

	error = git_revwalk_push_glob(walker, "refs/heads/*");
	checkError(error);

	git_oid oid;
	QStringList revisionList;
	while (!git_revwalk_next(&oid, walker))
		revisionList.append(oidToQString(&oid));

	git_revwalk_free(walker);

	return revisionList;
}

GitRepository::HEADState GitRepository::getHEADState() const
{
	int state = 0;

	state = git_repository_head_unborn(repository_);
	if (state > 0)
	{
		if (state == 1)
			return HEADState::UNBORN;
		checkError(state);
	}

	state = git_repository_head_detached(repository_);
	if (state > 0)
	{
		if (state == 1)
			return HEADState::DETACHED;
		checkError(state);
	}

	return HEADState::ATTACHED;
}

GitRepository::GitReferenceType GitRepository::referenceType(QString reference) const
{
	int errorCode = 0;
	GitReferenceType type = GitReferenceType::INVALID;
	bool noReference = false;
	git_reference* gitReference = nullptr;
	errorCode = git_reference_lookup(&gitReference, repository_, reference.toStdString().c_str());

	if (errorCode == GIT_EINVALIDSPEC)
	{
		noReference = true;
		type = GitReferenceType::INVALID;
	}
	if (errorCode == GIT_ENOTFOUND)
	{
		noReference = true;
		type = GitReferenceType::NOTFOUND;
	}
	checkError(errorCode);

	if (!noReference)
	{
		errorCode = git_reference_is_branch(gitReference);
		if (errorCode == 1)
			type = GitReferenceType::BRANCH;

		errorCode = git_reference_is_note(gitReference);
		if (errorCode == 1)
			type = GitReferenceType::NOTE;

		errorCode = git_reference_is_remote(gitReference);
		if (errorCode == 1)
			type = GitReferenceType::REMOTE;

		if (errorCode == 1)
			type = GitReferenceType::TAG;
	}

	git_reference_free(gitReference);

	return type;
}

bool GitRepository::isValidRevisionString(QString revision) const
{
	int errorCode = 0;
	bool isValid = true;
	git_object* obj = nullptr;
	errorCode = git_revparse_single(&obj, repository_, revision.toStdString().c_str());

	if (errorCode == GIT_ENOTFOUND)
		isValid = false;
	if (errorCode == GIT_EINVALIDSPEC)
		isValid = false;
	if (errorCode == GIT_EAMBIGUOUS)
		isValid = false;

	git_object_free(obj);

	return isValid;
}

void GitRepository::loadGenericTree(const std::shared_ptr<GenericTree>& tree, const QString version)
{
	IdToGenericNodeHash persistentUnitRoots;

	const Commit* commit = getCommit(version);
	for (auto file : commit->files())
	{
		GenericNode* unitRoot = Parser::load(file->content(), file->size_, false,
														 tree->newPersistentUnit(file->relativePath_));

		Q_ASSERT(unitRoot);
		persistentUnitRoots.insert(unitRoot->id(), unitRoot);
	}

	IdToGenericNodeHash persistentUnitDeclarations;
	for (GenericNode* node : persistentUnitRoots)
		findPersistentUnitDeclarations(node, persistentUnitDeclarations);

	for (GenericNode* root : persistentUnitRoots)
	{
		GenericNode* declaration = persistentUnitDeclarations.value(root->id());

		if (declaration)
		{
			GenericNode* parent = declaration->parent();
			root->detachFromParent();
			root->setParent(parent);
			parent->attachChild(root);

			declaration->remove();
		}
	}

	SAFE_DELETE(commit);
}

int GitRepository::getMinPrefixLength() { return GIT_OID_MINPREFIXLEN; }

void GitRepository::writeRevisionIntoIndex(QString revision)
{
	int errorCode = 0;

	git_commit* gitRevision = parseCommit(revision);

	git_tree* revisionTree = nullptr;
	errorCode = git_commit_tree(&revisionTree, gitRevision);
	checkError(errorCode);

	git_index* index = nullptr;
	errorCode = git_repository_index(&index, repository_);
	checkError(errorCode);

	errorCode = git_index_read_tree(index, revisionTree);
	checkError(errorCode);

	errorCode = git_index_write(index);
	checkError(errorCode);

	git_commit_free(gitRevision);
	git_tree_free(revisionTree);
	git_index_free(index);
}

void GitRepository::writeWorkdirToIndex()
{
	int errorCode = 0;

	// load index from disk
	git_index* index = nullptr;
	errorCode = git_repository_index(&index, repository_);
	checkError(errorCode);

	// read workdir from disk into in-memory index
	errorCode = git_index_update_all(index, nullptr, nullptr, nullptr);
	checkError(errorCode);

	// write index from memory to disk
	errorCode = git_index_write(index);
	checkError(errorCode);

	git_index_free(index);
}

QString GitRepository::writeIndexToTree()
{
	int errorCode = 0;

	git_index* index = nullptr;
	errorCode = git_repository_index(&index, repository_);
	checkError(errorCode);

	git_oid treeOid;
	errorCode = git_index_write_tree(&treeOid, index);
	if (errorCode == GIT_EUNMERGED)
		Q_ASSERT(false);
	checkError(errorCode);

	QString currentSHA1 = oidToQString(&treeOid);

	git_index_free(index);

	return currentSHA1;
}

void GitRepository::newCommit(QString tree, QString message, Signature author, Signature committer, QStringList parents)
{
	int errorCode = 0;

	git_oid treeOid;
	errorCode = git_oid_fromstr(&treeOid, tree.toStdString().c_str());
	checkError(errorCode);

	git_tree* gitTree = nullptr;
	errorCode = git_tree_lookup(&gitTree, repository_, &treeOid);
	checkError(errorCode);


	git_signature* gitAuthor = createGitSignature(author);
	git_signature* gitCommitter = createGitSignature(committer);


	char* gitMessage = new char[message.size()];
	strcpy(gitMessage, message.toUtf8().data());

	const git_commit** gitParents = new const git_commit* [parents.size()];
	for (int i = 0; i < parents.size(); i++)
	{
		QString revision = parents.at(i);
		git_commit* gitCommit = parseCommit(revision);
		gitParents[i] = gitCommit;
	}

	// create commit
	git_oid newCommitOid;
	errorCode = git_commit_create(&newCommitOid, repository_, HEAD, gitAuthor, gitCommitter,
											nullptr, gitMessage, gitTree, parents.size(),
											gitParents);
	checkError(errorCode);

	git_tree_free(gitTree);
	git_signature_free(gitAuthor);
	git_signature_free(gitCommitter);
	for (int i = 0; i < parents.size(); i++)
		git_commit_free(const_cast<git_commit**>(gitParents)[i]);

	delete[] gitMessage;
}

QString GitRepository::findMergeBase(QString branchA, QString branchB) const
{
	int errorCode = 0;

	git_commit* commitA = parseCommit(branchA);
	const git_oid* oidA = git_commit_id(commitA);

	git_commit* commitB = parseCommit(branchB);
	const git_oid* oidB = git_commit_id(commitB);

	git_oid oidBase;
	errorCode = git_merge_base(&oidBase, repository_, oidA, oidB);
	// TODO proper error handling?
	checkError(errorCode);

	QString mergeBase = oidToQString(&oidBase);

	git_commit_free(commitA);
	git_commit_free(commitB);

	return mergeBase;
}

git_signature* GitRepository::createGitSignature(Signature& signature)
{
	int errorCode = 0;

	char* name = new char[signature.name_.size()];
	strcpy(name, signature.name_.toStdString().c_str());

	char* eMail = new char[signature.eMail_.size()];
	strcpy(eMail, signature.eMail_.toStdString().c_str());

	git_time_t time = signature.dateTime_.toTime_t();
	int offset = signature.timeZone_.offsetFromUtc(signature.dateTime_) / 60;

	git_signature* gitSignature = nullptr;
	errorCode = git_signature_new(&gitSignature, name, eMail, time, offset);
	checkError(errorCode);

	delete[] name;
	delete[] eMail;

	return gitSignature;
}

const QString GitRepository::PATH_HEADS{QString{QDir::separator()} + "refs" + QDir::separator() + "heads"};
const QString GitRepository::PATH_REMOTES{QString{QDir::separator()} + "refs" + QDir::separator() + "remotes"};
const QString GitRepository::PATH_TAGS{QString{QDir::separator()} + "refs" + QDir::separator() + "tags"};
const QString GitRepository::PATH_NOTES{QString{QDir::separator()} + "refs" + QDir::separator() + "notes"};

QString GitRepository::currentBranch() const
{
	HEADState state = getHEADState();
	if (state != HEADState::ATTACHED)
		return QString{};

	int errorCode = 0;

	git_reference* head = nullptr;
	errorCode = git_repository_head(&head, repository_);
	checkError(errorCode);

	const char* fullName = git_reference_name(head);

	QString branch{fullName};

	git_reference_free(head);

	return branch;
}

bool GitRepository::setReferenceTarget(QString reference, QString target)
{
	int errorCode = 0;

	git_reference* ref = nullptr;
	errorCode = git_reference_lookup(&ref, repository_, reference.toStdString().c_str());
	if (errorCode == GIT_ENOTFOUND)
		return false;
	if (errorCode == GIT_EINVALIDSPEC)
		return false;
	checkError(errorCode);

	git_commit* gitTarget = parseCommit(target);
	const git_oid* targetOID = git_commit_id(gitTarget);

	git_reference* newRef = nullptr;
	git_reference_set_target(&newRef, ref, targetOID, nullptr);
	if (errorCode == GIT_EMODIFIED)
		return false;
	checkError(errorCode);

	git_reference_free(ref);
	git_reference_free(newRef);
	git_commit_free(gitTarget);

	return true;
}

void GitRepository::TraverseCommitGraph(CommitGraph* graph, git_commit* current, const git_oid* target) const
{
	const git_oid* oid = git_commit_id(current);
	QString currentSHA1 = oidToQString(oid);

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
				QString parentSHA1 = oidToQString(parentOID);

				graph->add(parentSHA1, currentSHA1);

				TraverseCommitGraph(graph, parent, target);
			}
			else if (isConnected != 0)
				checkError(isConnected);

			git_commit_free(parent);
		}
	}
}

const CommitFile* GitRepository::getCommitFileFromWorkdir(QString relativePath) const
{
	QString fullRelativePath = path_;
	fullRelativePath.append(QDir::separator());
	fullRelativePath.append(relativePath);

	if (!QFile::exists(fullRelativePath))
		return new CommitFile{};

	QFile file{fullRelativePath};
	if ( !file.open(QIODevice::ReadOnly) )
		throw FilePersistenceException{"Could not open file " + file.fileName() + ". " + file.errorString()};

	Q_ASSERT(file.size() < std::numeric_limits<int>::max());
	int totalFileSize = static_cast<int>(file.size());

	auto mapped = reinterpret_cast<char*>(file.map(0, totalFileSize));
	Q_ASSERT(mapped);

	char* content = new char[totalFileSize];
	memcpy(content, mapped, totalFileSize);

	CommitFile* commitFile = new CommitFile{relativePath, totalFileSize, std::unique_ptr<char[]>{content}};
	file.close();

	return commitFile;
}

const CommitFile* GitRepository::getCommitFileFromIndex(QString relativePath) const
{
	// check if such a file is in the index
	git_index* index = nullptr;
	int errorCode = git_repository_index(&index, repository_);
	checkError(errorCode);

	const char* path = relativePath.toStdString().c_str();
	const git_index_entry* entry = git_index_get_bypath(index, path, 0);

	if (entry == nullptr)
		return new CommitFile{};

	git_checkout_options options;
	git_checkout_init_options(&options, GIT_CHECKOUT_OPTIONS_VERSION);

	// only checkout the file with relativePath
	git_strarray pathArray;
	pathArray.count = 1;
	pathArray.strings = new char*[1];
	pathArray.strings[0] = new char[relativePath.size()];
	memcpy(pathArray.strings[0], path, relativePath.size());
	options.paths = pathArray;

	options.checkout_strategy = GIT_CHECKOUT_FORCE;

	// make a copy of current workdir file
	QString fullRelativePath = path_;
	fullRelativePath.append(relativePath);
	QFile workdirFile{fullRelativePath};
	QString copyName = fullRelativePath;
	copyName.append("_COPY_FOR_INDEX_CHECKOUT");
	bool success = workdirFile.copy(copyName);
	Q_ASSERT(success);
	workdirFile.close();

	errorCode = git_checkout_index(repository_, nullptr, &options);
	checkError(errorCode);

	QFile file{fullRelativePath};
	if ( !file.open(QIODevice::ReadOnly) )
		throw FilePersistenceException{"Could not open file " + file.fileName() + ". " + file.errorString()};

	Q_ASSERT(file.size() < std::numeric_limits<int>::max());
	int totalFileSize = static_cast<int>(file.size());

	auto mapped = reinterpret_cast<char*>(file.map(0, totalFileSize));
	Q_ASSERT(mapped);

	char* content = new char[totalFileSize];
	memcpy(content, mapped, totalFileSize);

	CommitFile* commitFile = new CommitFile{relativePath, totalFileSize, std::unique_ptr<char[]>{content}};

	success = file.remove();
	Q_ASSERT(success);
	success = QFile::copy(copyName, fullRelativePath);
	Q_ASSERT(success);
	success = QFile::remove(copyName);
	Q_ASSERT(success);

	return commitFile;
}

const CommitFile* GitRepository::getCommitFileFromTree(QString revision, QString relativePath) const
{
	Q_ASSERT(revision != WORKDIR);
	Q_ASSERT(revision != INDEX);

	int errorCode = 0;

	git_commit* gitCommit = parseCommit(revision);
	git_tree* tree = nullptr;
	errorCode = git_commit_tree(&tree, gitCommit);
	checkError(errorCode);

	git_tree_entry* treeEntry = nullptr;
	errorCode = git_tree_entry_bypath(&treeEntry, tree, relativePath.toStdString().c_str());
	if (errorCode == GIT_ENOTFOUND)
		return new CommitFile{};
	checkError(errorCode);

	git_object* obj = nullptr;
	errorCode = git_tree_entry_to_object(&obj, repository_, treeEntry);
	checkError(errorCode);


	if (git_object_type(obj) != GIT_OBJ_BLOB)
		return new CommitFile{};


	git_blob* blob = (git_blob*)obj;

	qint64 contentSize = git_blob_rawsize(blob);

	std::unique_ptr<char[], CommitFileContentDeleter>
			content{(char*)git_blob_rawcontent(blob), [obj](char*){ git_object_free(obj);}};
	CommitFile* file = new CommitFile{relativePath, contentSize, std::move(content)};

	git_commit_free(gitCommit);
	git_tree_free(tree);
	git_tree_entry_free(treeEntry);

	return file;
}

git_commit* GitRepository::parseCommit(QString revision) const
{
	int errorCode = 0;
	git_object* obj = nullptr;

	errorCode = git_revparse_single(&obj, repository_, revision.toStdString().c_str());

	switch (errorCode)
	{
		case GIT_ENOTFOUND:
		{
			std::cout << "Error: " << revision.toStdString().c_str() << " not found!" << std::endl;
			break;
		}
		case GIT_EAMBIGUOUS:
		{
			std::cout << "Error: " << revision.toStdString().c_str() << " is ambiguous!" << std::endl;
			break;
		}
		case GIT_EINVALIDSPEC:
		{
			std::cout << "Error: " << revision.toStdString().c_str() << " is invalid!" << std::endl;
			break;
		}
	}
	checkError(errorCode);

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
		std::cout << "Error: " << revision.toStdString().c_str() << " is not a commit!" << std::endl;

		// clean up
		git_object_free(obj);

		Q_ASSERT(false);
	}
}

QString GitRepository::oidToQString(const git_oid* oid) const
{
	char sha1[GIT_OID_HEXSZ + 1]{0};

	git_oid_fmt(sha1, oid);

	return QString{sha1};
}

void GitRepository::findPersistentUnitDeclarations(GenericNode* node, IdToGenericNodeHash& declarations)
{
	if (node->type() == GenericNode::PERSISTENT_UNIT_TYPE)
		declarations.insert(node->id(), node);
	else
		for (GenericNode* child : node->children())
			findPersistentUnitDeclarations(child, declarations);
}

bool GitRepository::hasCleanIndex() const
{
	int errorCode = 0;

	git_object* obj = nullptr;
	errorCode = git_revparse_single(&obj, repository_, "HEAD^{tree}");
	checkError(errorCode);

	git_tree* tree = nullptr;
	errorCode = git_tree_lookup(&tree, repository_, git_object_id(obj));
	checkError(errorCode);

	git_diff* diff = nullptr;
	errorCode = git_diff_tree_to_index(&diff, repository_, tree, nullptr, nullptr);
	checkError(errorCode);

	size_t numDeltas = git_diff_num_deltas(diff);
	return (numDeltas == 0);
}

bool GitRepository::hasCleanWorkdir() const
{
	int errorCode = 0;

	git_object* obj = nullptr;
	errorCode = git_revparse_single(&obj, repository_, "HEAD^{tree}");
	checkError(errorCode);

	git_tree* tree = nullptr;
	errorCode = git_tree_lookup(&tree, repository_, git_object_id(obj));
	checkError(errorCode);

	git_diff* diff = nullptr;
	errorCode = git_diff_tree_to_workdir_with_index(&diff, repository_, tree, nullptr);
	checkError(errorCode);

	size_t numDeltas = git_diff_num_deltas(diff);
	return (numDeltas == 0);
}

GitRepository::SourceKind GitRepository::sourceKind(QString revision)
{
	if (revision == WORKDIR)
		return SourceKind::Workdir;
	else if (revision == INDEX)
		return SourceKind::Index;
	else
		return SourceKind::Commit;
}

QPair<GitRepository::SourceKind, GitRepository::SourceKind> GitRepository::kind(QString revisionA, QString revisionB)
{
	QPair<SourceKind, SourceKind> diffKind;

	diffKind.first = sourceKind(revisionA);
	diffKind.second = sourceKind(revisionB);

	return diffKind;
}

void GitRepository::checkError(int errorCode)
{
	if (errorCode < 0)
	{
		const git_error* lastError = giterr_last();
		throw FilePersistenceException{QString{"Error %1/%2: %3"}.arg(errorCode).arg(lastError->klass)
												 .arg(lastError->message)};
	}
}

const char* GitRepository::HEAD = "HEAD";

static int relativePathTreeWalkCallBack(const char* root,
									 const git_tree_entry* entry,
									 void* data)
{
	RelativePathData* relativePathData = (RelativePathData*) data;

	git_object* obj = nullptr;
	int errorCode = git_tree_entry_to_object(&obj, relativePathData->repository_, entry);

	if (errorCode == 0)
	{
		if (git_object_type(obj) == GIT_OBJ_BLOB)
		{
			git_object_free(obj);
			QString fileName{git_tree_entry_name(entry)};
			if (fileName.contains(relativePathData->id))
			{

			QString rootPath{root};

			relativePathData->relativePath = rootPath.append(fileName);
			return -1;
			}
		}
	}
	else
	 {
		git_object_free(obj);
		Q_ASSERT(false);
	 }

	return 0;
}

QString GitRepository::relativePathForPersistentUnit(QString persistentUnitId, QString revision) const
{
	git_commit* gitCommit = parseCommit(revision);
	git_tree* tree = nullptr;
	git_commit_tree(&tree, gitCommit);

	RelativePathData relativePathData;
	relativePathData.repository_ = repository_;
	relativePathData.id = persistentUnitId;

	git_tree_walk(tree, GIT_TREEWALK_PRE, relativePathTreeWalkCallBack, &relativePathData);

	git_commit_free(gitCommit);
	git_tree_free(tree);

	return relativePathData.relativePath;
}

}
