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

#include "History.h"

#include "GitRepository.h"
#include "ChangeDescription.h"

#include "../simple/Parser.h"

namespace FilePersistence {

History::History(Model::NodeIdType rootNodeId, const CommitGraph* historyGraph, const GitRepository* repository)
{
	rootNodeId_ = rootNodeId;
	historyGraph_ = historyGraph;

	const CommitGraphItem start = historyGraph_->start();
	relevantCommits_.insert(start.commitSHA_);

	QSet<Model::NodeIdType> trackedIDs = trackSubtree(repository);
	detectRelevantCommits(&start, trackedIDs, repository);
}

History::~History()
{
	delete historyGraph_;
}


void History::detectRelevantCommits(const CommitGraphItem* current, QSet<Model::NodeIdType> trackedIDs,
												const GitRepository* repository)
{
	for (CommitGraphItem* child : current->children_)
	{
		Diff diff = repository->diff(current->commitSHA_, child->commitSHA_);
		IdToChangeDescriptionHash changes = diff.changes();

		bool isRelevant = false;
		for (ChangeDescription* change : changes.values())
		{
			if (trackedIDs.contains(change->id()))
			{
				isRelevant = true;
				break;
			}
		}

		if (isRelevant)
		{
			relevantCommits_.insert(child->commitSHA_);
			QSet<Model::NodeIdType> newTrackedIDs = updateTrackedIDs(newTrackedIDs, &diff);

			detectRelevantCommits(child, newTrackedIDs, repository);
		}
		else
			detectRelevantCommits(child, trackedIDs, repository);
	}
}

QSet<Model::NodeIdType> History::trackSubtree(const GitRepository* repository) const
{
	// TODO adapt to new GitRepository

	QString startCommitSpec = historyGraph_->start().commitSHA_;
	Commit startCommit = repository->getCommit(startCommitSpec);

	QSet<Model::NodeIdType> trackedIDs;

	/*
	for (QString file : startCommit.files_)
	{
		const char* data = commitContent.content_.value(file);
		qint64 dataLength = commitContent.size_.value(file);
		GenericNodeAllocator* allocator = new GenericNodeAllocator();
		GenericNode* fileRoot = Parser::load(data, dataLength, false, allocator);

		GenericNode* subtreeRoot = fileRoot->find(rootNodeId_);
		if (subtreeRoot != nullptr)
		{
			QList<GenericNode*> stack;
			stack.append(subtreeRoot);
			while (!stack.isEmpty())
			{
				GenericNode* current = stack.last();
				stack.removeLast();

				trackedIDs.insert(current->id());

				for (GenericNode* child : current->children())
					stack.append(child);
			}
		}
		delete allocator;
	}
	*/

	return trackedIDs;
}

QSet<Model::NodeIdType> History::updateTrackedIDs(const QSet<Model::NodeIdType> trackedIDs, const Diff* diff) const
{
	QSet<Model::NodeIdType> newTrackedIDs(trackedIDs);
	QSet<Model::NodeIdType> coveredStationaryOld;
	QSet<Model::NodeIdType> coveredStationaryNew;

	IdToChangeDescriptionHash changes = diff->changes();

	IdToChangeDescriptionHash stationary = diff->changes(ChangeType::Stationary);
	for (ChangeDescription* change : stationary.values())
	{
		ChangeDescription::UpdateFlags flags = change->flags();
		if (trackedIDs.contains(change->id()) && flags.testFlag(ChangeDescription::Children))
		{
			QList<const GenericNode*> nodes;
			const GenericNode* current = nullptr;

			const GenericNode* oldNode = change->oldNode();
			if (!coveredStationaryOld.contains(change->id()))
			{
				nodes.append(oldNode);
				coveredStationaryOld.insert(change->id());
			}
			while (!nodes.isEmpty())
			{
				current = nodes.last();
				nodes.removeLast();

				for (GenericNode* child : current->children())
				{
					Model::NodeIdType id = child->id();
					IdToChangeDescriptionHash::iterator iter = changes.find(id);
					Q_ASSERT(iter != changes.end());
					ChangeDescription* childChange = iter.value();
					ChangeType type = childChange->type();

					switch (type)
					{
						case ChangeType::Deleted:
						{
							newTrackedIDs.remove(id);
							nodes.append(child);
						}
							break;

						case ChangeType::Moved:
						{
							QList<const GenericNode*> movedNodes;
							const GenericNode* currentMoved;

							movedNodes.append(child);
							while (!movedNodes.isEmpty())
							{
								currentMoved = movedNodes.last();
								movedNodes.removeLast();

								newTrackedIDs.remove(currentMoved->id());

								for (GenericNode* childChild : currentMoved->children())
								{
									movedNodes.append(childChild);
									iter = changes.find(childChild->id());
									if (iter != changes.end())
									{
										ChangeDescription* description = iter.value();
										if (description->type() == ChangeType::Stationary)
											coveredStationaryOld.insert(childChild->id());
									}
								}
							}
						}
							break;

						case ChangeType::Stationary:
						{
							if (!coveredStationaryOld.contains(child->id()))
								nodes.append(child);
						}
							break;

						default:
							Q_ASSERT(false);
					}
				}
			}

			Q_ASSERT(nodes.isEmpty());
			const GenericNode* newNode = change->newNode();
			if (!coveredStationaryNew.contains(change->id()))
			{
				nodes.append(newNode);
				coveredStationaryNew.insert(change->id());
			}
			while (!nodes.isEmpty())
			{
				current = nodes.last();
				nodes.removeLast();

				for (GenericNode* child : current->children())
				{
					Model::NodeIdType id = child->id();
					IdToChangeDescriptionHash::iterator iter = changes.find(id);
					Q_ASSERT(iter != changes.end());
					ChangeDescription* childChange = iter.value();
					ChangeType type = childChange->type();

					switch (type)
					{
						case ChangeType::Added:
						{
							newTrackedIDs.insert(id);
							nodes.append(child);
						}
							break;

						case ChangeType::Moved:
						{
							QList<const GenericNode*> movedNodes;
							const GenericNode* currentMoved;

							movedNodes.append(child);
							while (!movedNodes.isEmpty())
							{
								currentMoved = movedNodes.last();
								movedNodes.removeLast();

								newTrackedIDs.remove(currentMoved->id());

								for (GenericNode* childChild : currentMoved->children())
								{
									movedNodes.append(childChild);
									iter = changes.find(childChild->id());
									if (iter != changes.end())
									{
										ChangeDescription* description = iter.value();
										if (description->type() == ChangeType::Stationary)
											coveredStationaryNew.insert(childChild->id());
									}
								}
							}
						}
							break;

						case ChangeType::Stationary:
						{
							if (!coveredStationaryNew.contains(child->id()))
								nodes.append(child);
						}
							break;

						default:
							Q_ASSERT(false);
					}
				}
			}
		}
	}
	return newTrackedIDs;
}

} /* namespace FilePersistence */
