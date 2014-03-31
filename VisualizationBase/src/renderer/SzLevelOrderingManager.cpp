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

#include "SzLevelOrderingManager.h"

namespace Visualization {

SzLevelOrderingManager::SzLevelOrderingManager()
{
}

SzLevelOrderingManager::~SzLevelOrderingManager()
{
	clear();
}

void SzLevelOrderingManager::registerOrdering(int semanticZoomLevelId, int orderingNumber)
{
	Q_ASSERT(!hasOrderingNumber(orderingNumber));

	if (orderingNumber < smallestRegisteredOrderingNumber_ || orderingNumberToId_.size() == 0)
		smallestRegisteredOrderingNumber_ = orderingNumber;

	if (orderingNumber > highestRegisteredOrderingNumber_ || orderingNumberToId_.size() == 0)
		highestRegisteredOrderingNumber_ = orderingNumber;

	orderingNumberToId_.insert(orderingNumber, semanticZoomLevelId);
}

int SzLevelOrderingManager::hasOrderingNumber(int orderingNumber)
{
	return orderingNumberToId_.contains(orderingNumber);
}

int SzLevelOrderingManager::getCoarserSemanticZoomLevel(int currentSemanticZoomLevel)
{
	int currentOrderingNumber = orderingNumberToId_.key(currentSemanticZoomLevel);
	int lowestOrderingNumberOverCurrent = highestRegisteredOrderingNumber_;

	QMap<int, int>::const_iterator i = orderingNumberToId_.constBegin();
	while (i != orderingNumberToId_.constEnd()) {
		if (i.key() > currentOrderingNumber && i.key() < lowestOrderingNumberOverCurrent)
			lowestOrderingNumberOverCurrent = i.key();

		++i;
	}

	return orderingNumberToId_.value(lowestOrderingNumberOverCurrent);
}

int SzLevelOrderingManager::getFinerSemanticZoomLevel(int currentSemanticZoomLevel)
{
	int currentOrderingNumber = orderingNumberToId_.key(currentSemanticZoomLevel);
	int highestOrderingNumberUnderCurrent = smallestRegisteredOrderingNumber_;

	QMap<int, int>::const_iterator i = orderingNumberToId_.constBegin();
	while (i != orderingNumberToId_.constEnd()) {
		if (i.key() < currentOrderingNumber && i.key() > highestOrderingNumberUnderCurrent)
			highestOrderingNumberUnderCurrent = i.key();

		++i;
	}

	return orderingNumberToId_.value(highestOrderingNumberUnderCurrent);
}

void SzLevelOrderingManager::clear()
{
	orderingNumberToId_.clear();
}

} /* namespace Visualization */
