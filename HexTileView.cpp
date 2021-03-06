/*
 * Copyright 2009-2017 Scott McCreary
 * Copyright 2014-2016 Puck Meerburg
 * Copyright 2013 Luke (noryb009)
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */

#include "HexTileView.h"
#include "HexTile.h"
#include <Font.h>
#include <Bitmap.h>
#include <String.h>
#include <stdio.h>
#include <Message.h>
#include <Window.h>

static bool sInit=false;

static float sFontHeight = 0;
static float sFontWidth = 0;
static float sFontSize = 0;

static BPoint sPoint1;
static BPoint sPoint2;
static BPoint sPoint3;
static BPoint sPoint4;
static BPoint sPoint5;
static BPoint sPoint6;


void HexTileView::CalcLayout(uint8 tilesize)
{
	sInit = true;

	switch(tilesize)
	{
		case TILESIZE_1:
		{
			sFontSize = 11;
			break;
		}
		case TILESIZE_2:
		{
			sFontSize = 15;
			break;
		}
		case TILESIZE_3:
		{
			sFontSize = 19;
			break;
		}
		case TILESIZE_4:
		{
			sFontSize = 23;
			break;
		}
		case TILESIZE_5:
		{
			sFontSize = 28;
			break;
		}
		default:
		{
			tilesize = TILESIZE_3;
			sFontSize = 19;
		}
	}

	BFont font(be_bold_font);
	font.SetSize(sFontSize);

	font_height fh;
	font.GetHeight(&fh);

	// We only use the ascent value because we're not using any characters
	// with descenders and leading isn't necessary in this instance, either.
	sFontHeight = fh.ascent;
	sFontWidth = font.StringWidth("0");

	sPoint1.x = (tilesize / 4.0) - (sFontWidth / 2.0);
	sPoint1.y = (tilesize / 3.0) + (sFontHeight / 2.0) - 1;

	sPoint2.x = (tilesize - sFontWidth) / 2.0;
	sPoint2.y = sPoint1.y - (tilesize / 8.0);

	sPoint3.x = (tilesize / 4.0) * 3.0 - (sFontWidth / 2.0);
	sPoint3.y = sPoint1.y;

	sPoint4.x = sPoint3.x;
	sPoint4.y = (tilesize / 3.0) * 2.0 + (sFontHeight / 2.0) - 3;

	sPoint5.x = sPoint2.x;
	sPoint5.y = sPoint4.y + (tilesize / 8.0);

	sPoint6.x = sPoint1.x;
	sPoint6.y = sPoint4.y;
}

HexTileView::HexTileView(const BPoint &pt, uint8 tilesize, const char *name, const int32 &resize,
					const int32 &flags)
 :	BView(BRect(pt.x,pt.y,pt.x+tilesize,pt.y+tilesize),name,resize,flags),
 	fMouseDown(false),
 	fTile(NULL)
{
	fOtherTile = new HexTile();
	
	if((tilesize < TILESIZE_1) || (tilesize > TILESIZE_5))
		tilesize = TILESIZE_3;

	if(!sInit)
		CalcLayout(tilesize);

	BFont font(be_bold_font);
	font.SetSize(sFontSize);
	SetFont(&font);
	SetViewColor(B_TRANSPARENT_COLOR);
}

void HexTileView::AttachedToWindow()
{
	//if(Parent() == NULL)
	//	return;
	//SetViewColor(Parent()->ViewColor());
}

void HexTileView::Draw(BRect r)
{
	DrawHexTile(this,Bounds(), true);
}

rgb_color HexTileView::NumberColor(int num)
{
	rgb_color white = {255,255,255,255};
	rgb_color black = {0,0,0,255};

	switch (num)
    {
        case 0 :
	    case 1 :
	    case 2 :
	    case 3 :
	    case 6 :
	    case 7 :
	    case 11 :
	    case 13 :
	    case 15 : return white;
	              break;
	    case 4 :
	    case 5 :
	    case 8 :
	    case 9 :
	    case 10 :
	    case 12 :
	    case 14 : return black;
	              break;
    }
    return black;
}

void DrawHexTile(HexTileView *owner, BRect r, bool lockedIn)
{
/*      points 1-6 are as shown here
          1_____2
         /\    /\
        /  \  /  \
      6/____\/____\3
       \    /\    /
        \  /  \  /
         \/____\/
         5      4
*/

	static rgb_color highlight = { 255,255,255,255 };
	static rgb_color shadow = { 90,90,90,255 };
	static rgb_color white = { 255,255,255,255 };
	static rgb_color base = { 224,224,224,255 };
	static rgb_color trianglecolor[] = { { 0,0,0,255 }, //Black
	                            { 145,97,0,255 },       //Brown
	                            { 224,0,0,255 },        //Red
	                            { 255,126,0,255 },      //Orange
	                            { 255,255,0,255 },      //Yellow
	                            { 0,208,0,255 },        //Green
	                            { 63,63,255,255 },      //Blue
	                            { 124,33,176,255 },     //Purple
	                            { 192,192,192,255 },    //Grey
	                            { 255,255,255,255 },    //White 
	                            { 255,153,153,255 },    //
	                            { 102,102,0,255 },      //
	                            { 0,204,255,255 },      //
	                            { 181,166,66,255 },     //
	                            { 234,234,174,255 },    //
	                            { 204,50,153,255 } };   //

	rgb_color numcolor;
	BPoint 	point1, point2, point3, point4, point5, point6,
			hexagoncenter, linesize;

	hexagoncenter.x = (r.RightTop().x-r.LeftTop().x)/2;
	hexagoncenter.y = (r.LeftBottom().y-r.LeftTop().y)/2;
	linesize.x = (r.RightTop().x - r.LeftTop().x)/2;
	linesize.y = (r.LeftBottom().y - r.LeftTop().y)/2;

	point1.x = (hexagoncenter.x - (linesize.x*0.5));
	point1.y = (hexagoncenter.y - (linesize.y*0.866));
	point2.x = (hexagoncenter.x + (linesize.x*0.5));
	point2.y = (hexagoncenter.y - (linesize.y*0.866));
	point3.x = (hexagoncenter.x + linesize.x);
	point3.y = (hexagoncenter.y);
	point4.x = (hexagoncenter.x + (linesize.x*0.5));
	point4.y = (hexagoncenter.y + (linesize.y*0.866));
	point5.x = (hexagoncenter.x - (linesize.x*0.5));
	point5.y = (hexagoncenter.y + (linesize.y*0.866));
	point6.x = (hexagoncenter.x - linesize.x);
	point6.y = (hexagoncenter.y);

	// make bounding rect transparent

	owner->SetDrawingMode(B_OP_COPY);

	if(!lockedIn)
	{
		owner->SetHighColor(B_TRANSPARENT_COLOR);
		owner->FillRect(owner->Bounds());
	}
	
	owner->SetHighColor(base);
	owner->SetDrawingMode(B_OP_COPY);

	if(owner->GetTile()->IsEmpty())
	{
		// Draw empty hexagon
		owner->BeginLineArray(6);
		owner->SetHighColor(shadow);
		owner->SetLowColor(base);
		owner->StrokeLine(point5, point6);
		owner->StrokeLine(point6, point1);
		owner->StrokeLine(point1, point2);
		owner->SetHighColor(highlight);
		owner->StrokeLine(point2, point3);
		owner->StrokeLine(point3, point4);
		owner->StrokeLine(point4, point5);
		owner->EndLineArray();
		return;
	}

	// frame
	owner->SetHighColor(highlight);
	owner->StrokeLine(point2, point3);
	owner->StrokeLine(point3, point4);
	owner->StrokeLine(point4, point5);

	owner->SetHighColor(shadow);
	owner->StrokeLine(point5, point6);
	owner->StrokeLine(point6, point1);
	owner->StrokeLine(point1, point2);

	// Color Triangles

	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->top]);
    owner->FillTriangle(point1, point2, hexagoncenter);

	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->topright]);
	owner->FillTriangle(point2, point3, hexagoncenter);

	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->bottomright]);
	owner->FillTriangle(point3, point4, hexagoncenter);

	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->bottom]);
	owner->FillTriangle(point4, point5, hexagoncenter);

	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->bottomleft]);
	owner->FillTriangle(point5, point6, hexagoncenter);

	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->topleft]);
	owner->FillTriangle(point6, point1, hexagoncenter);

	// dividers

	owner->SetHighColor(shadow);
	owner->SetPenSize(3.0);
	owner->StrokeLine(point1, point4);
	owner->StrokeLine(point2, point5);
	owner->StrokeLine(point3, point6);
	owner->SetHighColor(white);
	owner->SetPenSize(1.0);
	owner->StrokeLine(point1, point4);
	owner->StrokeLine(point2, point5);
	owner->StrokeLine(point3, point6);
	owner->SetHighColor(shadow);
	owner->SetPenSize(1.0);
	owner->StrokeLine(point1, point2);
	owner->StrokeLine(point2, point3);
	owner->StrokeLine(point3, point4);
	owner->StrokeLine(point4, point5);
	owner->StrokeLine(point5, point6);
	owner->StrokeLine(point6, point1);

	owner->SetDrawingMode(B_OP_ALPHA); //BLEND
	BRect bevel(r);
	bevel.InsetBy(1,1);
	owner->SetHighColor(255,255,255);

	// Numbers
	BString string;

	owner->SetDrawingMode(B_OP_ALPHA);

	string = "";
	switch ((int)owner->GetTile()->topleft)
		{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
				string << (int)owner->GetTile()->topleft;
				break;
			case 10:
				string << "A";
				break;
			case 11:
				string << "B";
				break;
			case 12:
				string << "C";
				break;
			case 13:
				string << "D";
				break;
			case 14:
				string << "E";
				break;
			case 15:
				string << "F";
				break;
		}
	numcolor = owner->NumberColor((int)owner->GetTile()->topleft);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sPoint1);

	string="";
	switch ((int)owner->GetTile()->top)
		{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
				string << (int)owner->GetTile()->top;
				break;
			case 10:
				string << "A";
				break;
			case 11:
				string << "B";
				break;
			case 12:
				string << "C";
				break;
			case 13:
				string << "D";
				break;
			case 14:
				string << "E";
				break;
			case 15:
				string << "F";
				break;
		}
	numcolor = owner->NumberColor((int)owner->GetTile()->top);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sPoint2);

	string="";
	switch ((int)owner->GetTile()->topright)
		{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
				string << (int)owner->GetTile()->topright;
				break;
			case 10:
				string << "A";
				break;
			case 11:
				string << "B";
				break;
			case 12:
				string << "C";
				break;
			case 13:
				string << "D";
				break;
			case 14:
				string << "E";
				break;
			case 15:
				string << "F";
				break;
		}
	numcolor = owner->NumberColor((int)owner->GetTile()->topright);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sPoint3);

	string="";
	switch ((int)owner->GetTile()->bottomright)
		{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
				string << (int)owner->GetTile()->bottomright;
				break;
			case 10:
				string << "A";
				break;
			case 11:
				string << "B";
				break;
			case 12:
				string << "C";
				break;
			case 13:
				string << "D";
				break;
			case 14:
				string << "E";
				break;
			case 15:
				string << "F";
				break;
		}
	numcolor = owner->NumberColor((int)owner->GetTile()->bottomright);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sPoint4);

	string="";
	switch ((int)owner->GetTile()->bottom)
		{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
				string << (int)owner->GetTile()->bottom;
				break;
			case 10:
				string << "A";
				break;
			case 11:
				string << "B";
				break;
			case 12:
				string << "C";
				break;
			case 13:
				string << "D";
				break;
			case 14:
				string << "E";
				break;
			case 15:
				string << "F";
				break;
		}
	numcolor = owner->NumberColor((int)owner->GetTile()->bottom);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sPoint5);

	string="";
	switch ((int)owner->GetTile()->bottomleft)
		{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
			case 8 :
			case 9 :
				string << (int)owner->GetTile()->bottomleft;
				break;
			case 10:
				string << "A";
				break;
			case 11:
				string << "B";
				break;
			case 12:
				string << "C";
				break;
			case 13:
				string << "D";
				break;
			case 14:
				string << "E";
				break;
			case 15:
				string << "F";
				break;
		}
	numcolor = owner->NumberColor((int)owner->GetTile()->bottomleft);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sPoint6);
}

void HexTileView::SetTile(HexTile *tile)
{
	fTile = tile;
	fTile->gridid = fGridId;
	Invalidate();
}

void HexTileView::MouseDown(BPoint pt)
{
	fMouseDown = true;

	if(!fTile->IsEmpty())
		DoDrag();
}

void HexTileView::MouseMoved(BPoint pt,uint32 code, const BMessage *msg)
{
}

void HexTileView::MouseUp(BPoint pt)
{
	fMouseDown = false;
}

void HexTileView::MessageReceived(BMessage *msg)
{
	msg->PrintToStream();
	if(msg->WasDropped())
	{
		printf("Dropped tile\n");
		HexTile *tile = dynamic_cast<HexTile *>(HexTile::Instantiate(msg));
		
		if(!tile)
			return;

		if(!fTile->IsEmpty())
		{
			printf("    Not empty\n");
			msg->SendReply(B_MESSAGE_NOT_UNDERSTOOD);
			return;
		}

		tile->PrintToStream();

		// Well, we've gotten this far, so this tile must be empty and the other one not.
		// Now we actually have to do some game logic. We can only do this from the
		// hosting window, so post a message to determine the logic
		if(Window())
		{
			BMessage post(M_CHECK_DROP);
			post.AddMessage("original", msg);
			post.AddPointer("tile", tile);
			post.AddPointer("to", this);
			printf("    Asking window to check drop\n");
			Window()->PostMessage(&post, Window(), this);
		}
		return;
	}

	if(msg->what == B_MESSAGE_NOT_UNDERSTOOD) {
		printf("Drop failed\n");
		
		fOtherTile->PrintToStream();
		fTile->PrintToStream();
		
		HexTile *tile = fOtherTile;
		fOtherTile = fTile;
		fTile = tile;
		fTile->gridid = fGridId;
		Invalidate();
		return;
	}

	BView::MessageReceived(msg);
}

void HexTileView::DoDrag(void)
{
	fFakeEmpty = true;
	
	BBitmap *bitmap = new BBitmap(Bounds(), B_RGBA32, true);
	HexTileView *view = new HexTileView(BPoint(0,0), (uint8) Bounds().IntegerWidth(),NULL,0,0);
	view->SetTile(GetTile());

	bitmap->AddChild(view);
	bitmap->Lock();

	BFont font;
	GetFont(&font);
	view->SetFont(&font);

	DrawHexTile(view, Bounds(), false);

	view->Sync();
	bitmap->Unlock();

	BMessage msg(B_SIMPLE_DATA);

	if(GetTile()->Archive(&msg) != B_OK)
		debugger("Archiving failed!");

	*fOtherTile = *fTile;
	fTile->MakeEmpty();
	Invalidate();

	DragMessage(&msg, bitmap, B_OP_OVER, 
		BPoint(Bounds().Width() / 2, Bounds().Height() / 2), this);
}
