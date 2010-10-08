 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/widgets/multi/list/listentry.h>
#include <smooth/gui/widgets/multi/list/listbox.h>
#include <smooth/gui/widgets/hotspot/hotspot.h>
#include <smooth/gui/window/window.h>
#include <smooth/graphics/surface.h>
#include <smooth/misc/math.h>
#include <smooth/misc/binary.h>

S::Signal2<S::Void, S::Int, S::Int>	 S::GUI::ListEntry::internalOnSelectEntry;

const S::Short	 S::GUI::ListEntry::classID = S::Object::RequestClassID();

S::GUI::ListEntry::ListEntry(const String &iText) : Widget(Point(), Size(100, 15))
{
	type		= classID;

	selected	= False;
	marked		= False;
	text		= iText;

	font.SetColor(Setup::ClientTextColor);

	hotspot		= new Hotspot(Point(), GetSize());
	markHotspot	= new Hotspot(Point(2, 3), Size(9, 9));

	hotspot->onLeftButtonDown.Connect(&ListEntry::OnSelectEntry, this);
	markHotspot->onLeftButtonDown.Connect(&ListEntry::OnClickMarkHotspot, this);

	hotspot->onMouseDrag.Connect(&ListEntry::InitDrag, this);

	Add(hotspot);
	Add(markHotspot);

	internalOnSelectEntry.Connect(&ListEntry::OnSelectOtherEntry, this);

	onChangeSize.Connect(&Hotspot::SetSize, hotspot);
}

S::GUI::ListEntry::~ListEntry()
{
	internalOnSelectEntry.Disconnect(&ListEntry::OnSelectOtherEntry, this);

	DeleteObject(hotspot);
	DeleteObject(markHotspot);
}

S::Int S::GUI::ListEntry::Paint(Int message)
{
	if (!IsRegistered())	return Error();
	if (!IsVisible())	return Success();

	switch (message)
	{
		case SP_SHOW:
		case SP_PAINT:
		case SP_MOUSEIN:
		case SP_MOUSEOUT:
			{
				Surface	*surface = GetDrawSurface();
				Rect	 frame	 = Rect(GetRealPosition(), GetSize());
				Font	 nFont	 = font;
				Bool	 gotTabs = False;

				if (mouseOver) nFont.SetColor(Setup::GradientTextColor);
				if (!IsActive()) nFont.SetColor(Setup::GrayTextColor);

				for (Int r = 0; r < text.Length(); r++) if (text[r] == '\t') { gotTabs = True; break; }

				surface->StartPaint(GetVisibleArea());

				if (mouseOver)	surface->Gradient(frame, Setup::GradientStartColor, Setup::GradientEndColor, OR_HORZ);
				else		surface->Box(frame, IsActive() ? Setup::ClientColor : Setup::BackgroundColor, Rect::Filled);

				if (selected)	surface->Box(frame, Setup::ClientTextColor, Rect::Dotted);

				if (container->GetFlags() & LF_MULTICHECKBOX)
				{
					Rect	 cbRect = Rect(GetRealPosition() + Point(2, 3), Size(9, 9));

					if (cbRect.top <= cbRect.bottom - 1)
					{
						surface->Box(cbRect, Setup::ClientColor, Rect::Filled);
						surface->Box(cbRect, Setup::GrayTextColor, Rect::Outlined);

						if (marked && cbRect.top <= cbRect.bottom - 3)
						{
							Point	 p1 = Point(cbRect.left + 2 + (IsRightToLeft() ? 1 : 0), cbRect.top + 2);
							Point	 p2 = Point(cbRect.right - 2 + (IsRightToLeft() ? 1 : 0), cbRect.bottom - 2);

							Color	 lightColor = Setup::GrayTextColor;
							Color	 darkColor = Setup::ClientTextColor;

							if (!IsActive()) darkColor = Setup::GrayTextColor;

							surface->Line(p1, p2, darkColor);
							surface->Line(p1 + Point(1, 0), p2 + Point(0, -1), lightColor);
							surface->Line(p1 + Point(0, 1), p2 + Point(-1, 0), lightColor);
							surface->Line(p1 + Point(4, 0), p2 + Point(-6, 0), darkColor);
							surface->Line(p1 + Point(4, 1), p2 + Point(-5, 0), lightColor);
							surface->Line(p1 + Point(3, 0), p2 + Point(-6, -1), lightColor);
						}
					}
				}

				Widget	*listBox = container;

				while (listBox->GetObjectType() != ListBox::classID)
				{
					listBox = listBox->GetContainer();

					if (listBox == NIL) break;
				}

				if (listBox != NIL && ((ListBox *) listBox)->GetNOfTabs() > 0 && gotTabs)
				{
					for (Int i = 0; i < ((ListBox *) listBox)->GetNOfTabs(); i++)
					{
						Rect	 rect = Rect(GetRealPosition() + Point(1 + (i > 0 && listBox != container ? listBox->GetRealPosition().x - container->GetRealPosition().x + 2 : 0), 1), GetSize() - Size(3, 2));

						rect.left += ((ListBox *) listBox)->GetNthTabOffset(i);
						rect.left += (i == 0 ? (container->GetFlags() & LF_MULTICHECKBOX ? 12 : 0) : 0);

						if (((ListBox *) listBox)->GetNOfTabs() >= i + 2) rect.right = rect.left + (((ListBox *) listBox)->GetNthTabOffset(i + 1) - ((ListBox *) listBox)->GetNthTabOffset(i)) - (i == 0 ? (container->GetFlags() & LF_MULTICHECKBOX ? 12 : 0) : 0) - 3;

						String	 tabText = GetNthTabText(i);

						if (((ListBox *) listBox)->GetNthTabOrientation(i) == OR_RIGHT)
						{
							rect.left = Math::Max(rect.left, rect.right - nFont.GetTextSizeX(tabText));
						}

						surface->SetText(tabText, rect, nFont);
					}
				}
				else
				{
					surface->SetText(text, frame + Point(1 + (container->GetFlags() & LF_MULTICHECKBOX ? 12 : 0), 1) - Size(2 + (container->GetFlags() & LF_MULTICHECKBOX ? 12 : 0), 2), nFont);
				}

				surface->EndPaint();
			}

			return Widget::Paint(SP_PAINT);
	}

	return Widget::Paint(message);
}

S::String S::GUI::ListEntry::GetNthTabText(Int i)
{
	String	 tabText;
	Int	 tabCount = 0;

	for (Int p = 0; p < text.Length(); p++)
	{
		if (tabCount == i)
		{
			for (Int q = p; q < text.Length(); q++)
			{
				if (text[q] == '\t') break;

				tabText[q - p] = text[q];
			}

			break;
		}

		if (text[p] == '\t') tabCount++;
	}

	return tabText;
}

S::Int S::GUI::ListEntry::SetMark(Bool nMarked)
{
	if (nMarked == marked) return Success();

	marked = nMarked;

	if (IsRegistered())
	{
		Paint(SP_PAINT);

		((List *) container)->onMarkEntry.Emit(this);
	}

	onToggleMark.Emit(marked);

	return Success();
}

S::Bool S::GUI::ListEntry::IsMarked()
{
	return marked;
}

S::Int S::GUI::ListEntry::Select()
{
	selected = True;

	internalOnSelectEntry.Emit(container->GetHandle(), GetHandle());

	Paint(SP_PAINT);	

	((List *) container)->onSelectEntry.Emit(this);

	onSelect.Emit();

	return Success();
}

S::Int S::GUI::ListEntry::Deselect()
{
	selected = False;

	Paint(SP_PAINT);

	onDeselect.Emit();

	return Success();
}

S::Bool S::GUI::ListEntry::IsSelected()
{
	return selected;
}

S::Void S::GUI::ListEntry::InitDrag()
{
	if (!mouseOver && container->GetObjectType() == ListBox::classID)
	{
		((ListBox *) container)->DragSelectedEntry(container->GetContainerWindow()->GetMousePosition().y < GetRealPosition().y ? True : False);
	}
}

S::Void S::GUI::ListEntry::OnClickMarkHotspot()
{
	SetMark(!marked);
}

S::Void S::GUI::ListEntry::OnSelectEntry()
{
	if (!selected || Binary::IsFlagSet(container->GetFlags(), LF_ALLOWRESELECT)) Select();
}

S::Void S::GUI::ListEntry::OnSelectOtherEntry(Int containerHandle, Int handle)
{
	if (!IsRegistered()) return;

	if (container->GetHandle() == containerHandle && GetHandle() != handle && selected) Deselect();
}
