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

#include "Diff3Parse.h"

namespace FilePersistence {

Chunk::Chunk(bool stable, QList<Model::NodeIdType> idListA, QList<Model::NodeIdType> idListB,
						QList<Model::NodeIdType> idListBase) :
	stable_{stable}, noConflicts_{true}, spanA_{idListA}, spanB_{idListB}, spanBase_{idListBase}
{
	if (stable_)
	{
		Q_ASSERT(spanA_ == spanB_ && spanB_ == spanBase_);
		spanMerged_ = spanBase_;
	}
	else
		spanMerged_ = {};
}

QList<Chunk*> Diff3Parse::computeChunks(const QList<Model::NodeIdType> idListA,
																								 const QList<Model::NodeIdType> idListB,
																								 const QList<Model::NodeIdType> idListBase)
{
	auto stableIDs = longestCommonSubsequence3(idListBase, idListA, idListB);

	QList<Chunk*> chunks;
	auto iterLCS = stableIDs.constBegin();
	auto iterA = idListA.constBegin();
	auto iterB = idListB.constBegin();
	auto iterBase = idListBase.constBegin();
	bool lastWasEmpty = true;
	QList<Model::NodeIdType> listA;
	QList<Model::NodeIdType> listB;
	QList<Model::NodeIdType> listBase;
	while (iterLCS != stableIDs.constEnd())
	{
		while (iterLCS != stableIDs.constEnd() && (*iterLCS == *iterA) && (*iterLCS == *iterB) && (*iterLCS == *iterBase))
		{
			listA.append(*iterLCS);
			listB.append(*iterLCS);
			listBase.append(*iterLCS);
			lastWasEmpty = false;
			++iterA;
			++iterB;
			++iterBase;
			++iterLCS;
		}
		// After above loop, all elements of stable chunk are added by this if()
		// And it goes into next iteration
		if (!lastWasEmpty)
		{
			// Add elements to the stable chunk
			chunks.append(new Chunk(true, listA, listB, listBase));
			listA.clear();
			listB.clear();
			listBase.clear();
			lastWasEmpty = true;
			continue;
			// While loop Terminates after this continue
		}

		// Add elements to the unstable chunk
		// We are guaranteed here that there will be some elements in Unstable chunk
		while (*iterA != *iterLCS)
		{
			listA.append(*iterA);
			iterA++;
		}
		while (*iterB != *iterLCS)
		{
			listB.append(*iterB);
			iterB++;
		}
		while (*iterBase != *iterLCS)
		{
			listBase.append(*iterBase);
			iterBase++;
		}
		chunks.append(new Chunk(false, listA, listB, listBase));
		listA.clear();
		listB.clear();
		listBase.clear();
	}

	// Add the elements to the last unstable chunk
	while (iterA != idListA.constEnd())
	{
		listA.append(*iterA);
		lastWasEmpty = false;
		iterA++;
	}
	while (iterB != idListB.constEnd())
	{
		listB.append(*iterB);
		lastWasEmpty = false;
		iterB++;
	}
	while (iterBase != idListBase.constEnd())
	{
		listBase.append(*iterBase);
		lastWasEmpty = false;
		iterBase++;
	}
	if (!lastWasEmpty)	chunks.append(new Chunk(false, listA, listB, listBase));

	return chunks;
}

QList<Model::NodeIdType> Diff3Parse::longestCommonSubsequence(const QList<Model::NodeIdType> listA,
																			const QList<Model::NodeIdType> listB)
{
	int m = listA.size() + 1;
	int n = listB.size() + 1;

	int** lcsLength = new int*[m];
	for (int i = 0; i < m; ++i)
		lcsLength[i] = new int[n];

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			lcsLength[i][j] = 0;

	for (int i = 1; i < m; ++i)
		for (int j = 1; j < n; ++j)
			if (listA.at(i-1) == listB.at(j-1))
				lcsLength[i][j] = lcsLength[i-1][j-1] + 1;
			else
				lcsLength[i][j] = std::max(lcsLength[i][j-1], lcsLength[i-1][j]);

	QList<Model::NodeIdType> lcs = backtrackLCS(lcsLength, listA, listB, listA.size(), listB.size());

	for (int i = 0; i < m; ++i)
		delete[] lcsLength[i];

	delete[] lcsLength;

	return lcs;
}

QList<Model::NodeIdType> Diff3Parse::longestCommonSubsequence3(const QList<Model::NodeIdType> listA,
																				const QList<Model::NodeIdType> listB,
																				  const  QList<Model::NodeIdType> listC)
{
	int m = listA.size() + 1;
	int n = listB.size() + 1;
	int o = listC.size() + 1;

	int*** lcsLength = new int**[m];
	for (int i = 0; i < m; ++i)
	{
		lcsLength[i] = new int*[n];
		for (int j = 0; j < n; ++j)
			lcsLength[i][j] = new int[o];
	}

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			for (int k = 0; k < o; ++k)
				lcsLength[i][j][k] = 0;

	for (int i = 1; i < m; ++i)
		for (int j = 1; j < n; ++j)
			for (int k = 1; k < o; ++k)
				if ((listA.at(i-1) == listB.at(j-1)) && (listA.at(i-1) == listC.at(k-1)))
					lcsLength[i][j][k] = lcsLength[i-1][j-1][k-1] + 1;
				else
					lcsLength[i][j][k] = std::max(lcsLength[i-1][j][k], std::max(lcsLength[i][j-1][k], lcsLength[i][j][k-1]));

	QList<Model::NodeIdType> lcs = backtrackLCS3(lcsLength, listA, listB, listC, listA.size(), listB.size(), listC.size());

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			delete[] lcsLength[i][j];
	for (int i = 0; i < m; ++i)
		delete[] lcsLength[i];

	delete[] lcsLength;

	return lcs;
}

QList<Model::NodeIdType> Diff3Parse::backtrackLCS(int** data, const QList<Model::NodeIdType> listA,
															const QList<Model::NodeIdType> listB, int posA, int posB)
{
	if (posA == 0 || posB == 0)
		return QList<Model::NodeIdType>{};
	else if (listA.at(posA-1) == listB.at(posB-1))
		return backtrackLCS(data, listA, listB, posA-1, posB-1)<<listA.at(posA-1);
	else
		if (data[posA][posB-1] > data[posA-1][posB])
			return backtrackLCS(data, listA, listB, posA, posB-1);
		else
			return backtrackLCS(data, listA, listB, posA-1, posB);
}

QList<Model::NodeIdType> Diff3Parse::backtrackLCS3(int*** data, const QList<Model::NodeIdType> listA,
																			const QList<Model::NodeIdType> listB,
																			const QList<Model::NodeIdType> listC,
																			int posA, int posB, int posC)
{
	if (posA == 0 || posB == 0 || posC == 0)
		return QList<Model::NodeIdType>{};
	else if ((listA.at(posA-1) == listB.at(posB-1)) && (listA.at(posA-1) == listC.at(posC-1)))
		return backtrackLCS3(data, listA, listB, listC, posA-1, posB-1, posC-1) << listA.at(posA-1);
	else
		if (data[posA][posB][posC-1] > std::max(data[posA][posB-1][posC], data[posA-1][posB][posC]))
			return backtrackLCS3(data, listA, listB, listC, posA, posB, posC-1);
		else if (data[posA][posB-1][posC] > data[posA-1][posB][posC])
			return backtrackLCS3(data, listA, listB, listC, posA, posB-1, posC);
		else
			return backtrackLCS3(data, listA, listB, listC, posA-1, posB, posC);
}

}
