/*
 * Copyright 2009-2012 Scott McCreary
 * Copyright 2013 Luke (noryb009)
 * Copyright 2014 Puck Meerburg
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
 
/*
  example layout, 4 rows, 4 columns = 16 hexagon tiles

              col0    col1     col2     col3
             ______            ______
            /\    /\          /\    /\
           /tl\ t/tr\        /tl\ t/tr\
 row0     /____\/____\ ____ /____\/____\______
          \    /\    /\    /\    /\    /\    /\
           \bl/ b\br/tl\ t/tr\bl/ b\br/tl\ t/tr\
            \/____\/____\/____\/____\/____\/____\
            /\    /\    /\    /\    /\    /\    /
           /tl\ t/tr\bl/ b\br/tl\ t/tr\bl/ b\br/
 row1     /____\/____\/____\/____\/____\/____\/
          \    /\    /\    /\    /\    /\    /\
           \bl/ b\br/tl\ t/tr\bl/ b\br/tl\ t/tr\
            \/____\/____\/____\/____\/____\/____\
                   \    /\    /      \    /\    /
                    \bl/ b\br/        \bl/ b\br/
                     \/____\/          \/____\/


*/

#include "HexTile.h"
#include "HexGrid.h"
#include <OS.h>
#include <stdlib.h>
#include <stdio.h>
#include "HexTileView.h"

BArchivable *
HexTile::Instantiate(BMessage *from)
{
	if(validate_instantiation(from, "HexTile"))
		return new HexTile(from);
	return NULL;
}


HexTile::HexTile(BMessage *from)
	:
	topleft(-1),
	top(-1),
	topright(-1),
	bottomleft(-1),
	bottom(-1),
	bottomright(-1),
	id(0),
	toplefttile(NULL),
	toptile(NULL),
	toprighttile(NULL),
	bottomlefttile(NULL),
	bottomtile(NULL),
	bottomrighttile(NULL)
{
	SetValues(
		from->GetInt8("HexTile::topleft", -1),
		from->GetInt8("HexTile::top", -1),
		from->GetInt8("HexTile::topright", -1),
		from->GetInt8("HexTile::bottomleft", -1),
		from->GetInt8("HexTile::bottom", -1),
		from->GetInt8("HexTile::bottomright", -1));
	id = from->GetUInt16("HexTile::id", 0);
	gridid = from->GetUInt16("HexTile::gridid", 0);
}


status_t
HexTile::Archive(BMessage *into, bool deep)
{
	status_t error;

	error = BArchivable::Archive(into, deep);
	if(error != B_OK)
		return error;

	into->AddInt8("HexTile::topleft", 		topleft);
	into->AddInt8("HexTile::top",			top);
	into->AddInt8("HexTile::topright",		topright);
	into->AddInt8("HexTile::bottomleft", 	bottomleft);
	into->AddInt8("HexTile::bottom",		bottom);
	into->AddInt8("HexTile::bottomright",	bottomright);
	into->AddUInt16("HexTile::id",			id);
	into->AddUInt16("HexTile::gridid",		gridid);
	
	return B_OK;
}


HexTile::HexTile(void)
 :	topleft(-1),
 	top(-1),
 	topright(-1),
	bottomleft(-1),
 	bottom(-1),
 	bottomright(-1),
 	id(0),
 	gridid(0),
 	toplefttile(NULL),
 	toptile(NULL),
 	toprighttile(NULL),
 	bottomlefttile(NULL),
 	bottomtile(NULL),
 	bottomrighttile(NULL)
{
}


HexTile::HexTile(const uint8 &tl, const uint8 &t, const uint8 &tr, 
				 const uint8 &bl, const uint8 &b, const uint8 &br)
{
	SetValues(tl, t, tr, bl, b, br);
	id = 0;
}

HexTile::HexTile(const HexTile &t)
 :	topleft(t.topleft),
 	top(t.top),
 	topright(t.topright),
	bottomleft(t.bottomleft),
 	bottom(t.bottom),
 	bottomright(t.bottomright),
 	id(t.id),
 	gridid(t.id),
 	toplefttile(NULL),
 	toptile(NULL),
 	toprighttile(NULL),
 	bottomlefttile(NULL),
 	bottomtile(NULL),
 	bottomrighttile(NULL)
{
}

HexTile &HexTile::operator=(const HexTile &t)
{
	SetValues(t.topleft,t.top,t.topright,t.bottomleft,t.bottom,t.bottomright);
	id = t.id;
	gridid = t.gridid;

	return *this;
}


void HexTile::SetValues(const uint8 &tl, const uint8 &t, const uint8 &tr, 
                        const uint8 &bl, const uint8 &b, const uint8 &br)
{
	topleft = tl;
	top = t;
	topright = tr;
	bottomleft = bl;
	bottom = b;
	bottomright = br;
}
