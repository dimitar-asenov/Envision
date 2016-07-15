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

#pragma once

#include "../filepersistence_api.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

struct FILEPERSISTENCE_API Chunk
{
		bool stable_{};
		bool noConflicts_{};
		QList<Model::NodeIdType> spanA_;
		QList<Model::NodeIdType> spanB_;
		QList<Model::NodeIdType> spanBase_;
		QList<Model::NodeIdType> spanMerged_;

		Chunk();
		Chunk(bool stable, QList<Model::NodeIdType> idListA, QList<Model::NodeIdType> idListB,
			  QList<Model::NodeIdType> idListBase);
};

class FILEPERSISTENCE_API Diff3Parse
{
	friend class ListMergeComponent;
	friend class ListMergeComponentV2;
	public:
		/**
		 * Computes stable and unstable chunks. This is what's called a diff3 parse in the paper by Khanna, Kunal,
		 * Pierce: A Formal Investigation of Diff3 available at http://www.cis.upenn.edu/~bcpierce/papers/diff3-short.pdf.
		 *
		 * The caller is responsible for deleting the returned chunks.
		 */
		static QList<Chunk*> computeChunks(const QList<Model::NodeIdType> idListA, const QList<Model::NodeIdType> idListB,
													const QList<Model::NodeIdType> idListBase);

		static QList<Model::NodeIdType> backtrackLCS(int** data, const QList<Model::NodeIdType> listA,
																	const QList<Model::NodeIdType> listB, int posA, int posB);
		static QList<Model::NodeIdType> backtrackLCS3(int*** data, const QList<Model::NodeIdType> listA,
																	const QList<Model::NodeIdType> listB,
																	const QList<Model::NodeIdType> listC,
																	int posA, int posB, int posC);

		static QList<Model::NodeIdType> longestCommonSubsequence(const QList<Model::NodeIdType> listA,
																					const QList<Model::NodeIdType> listB);
		static QList<Model::NodeIdType> longestCommonSubsequence3(const QList<Model::NodeIdType> listA,
																					 const QList<Model::NodeIdType> listB,
																					 const QList<Model::NodeIdType> listC);

};

inline Chunk::Chunk() : noConflicts_{true} {}

}
