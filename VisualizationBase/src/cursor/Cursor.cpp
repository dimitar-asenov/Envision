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

#include "Cursor.h"
#include "../items/Item.h"
#include "../items/ItemStyle.h"

namespace Visualization {

Cursor::Cursor(Item* owner, CursorType type, Item* visualization)
	: owner_{owner}, visualization_{nullptr}, type_{type}, notLocationEquivalent_{false}
{
	if (visualization) setVisualization(visualization);
}

Cursor::Cursor(const Cursor& other) :	position_{other.position_}, region_{other.region_}, owner_{other.owner_},
		type_{other.type_}, notLocationEquivalent_{other.notLocationEquivalent_}
{}

Cursor::~Cursor()
{
	setVisualization(nullptr);
}

Cursor* Cursor::clone() const
{
	return new Cursor{*this};
}

void Cursor::update()
{
	if (type_ == BoxCursor)
	{
		setRegion( owner_->boundingRect().toRect() );
		setPosition( region_.center() );
	}
}

Item* Cursor::owner() const
{
	return owner_;
}

void Cursor::setVisualization(Item* visualization)
{
	SAFE_DELETE_ITEM(visualization_);
	visualization_ = visualization;

	if (visualization_)
	{
		auto item = owner();
		while (item)
		{
			if (item->flags() & Item::ItemIgnoresTransformations)
			{
				visualization_->setFlag(Item::ItemIgnoresTransformations);
				break;
			}
			item = item->parent();
		}
	}
}

bool Cursor::isSame(Cursor* other)
{
	return owner() == other->owner() && position() == other->position();
}

bool Cursor::isAtBoundary() const
{
	return false;
}

bool Cursor::isLocationEquivalent(Cursor* c)
{
	return isLocationEquivalent(c->notLocationEquivalent(), c->type(), c->isAtBoundary(), c->owner());
}

bool Cursor::isLocationEquivalent(bool otherNotLocationEquivalent, CursorType otherType, bool otherIsAtBoundary,
		Item* otherOwner)
{
	if (otherNotLocationEquivalent || notLocationEquivalent() ) return false;
	if (otherType != type() || otherType == BoxCursor) return false;
	if (!otherIsAtBoundary && ! isAtBoundary()) return false;
	if (owner() == otherOwner) return false;
	if (!( owner()->isAncestorOf(otherOwner) && allowEquivalentCursorsAcrossBoundaries(owner(), otherOwner))
			&& !(otherOwner->isAncestorOf(owner()) && allowEquivalentCursorsAcrossBoundaries(otherOwner, owner()) ) )
		return false;

	return true;
}

bool Cursor::allowEquivalentCursorsAcrossBoundaries(Item* parent, Item* child)
{
	if (!parent->style()->allowEquivalentCursorsThroughBoundary()) return false;
	while (child != parent)
	{
		if (!child->style()->allowEquivalentCursorsThroughBoundary()) return false;
		child = child->parent();
	}
	return true;
}

QRect Cursor::sceneRegion()
{
	return owner()->mapToScene(region()).boundingRect().toRect();
}

}
