 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <windows.h>
#include <gdiplus.h>

#include <smooth/graphics/backends/gdiplus/surfacegdiplus.h>
#include <smooth/graphics/surface.h>
#include <smooth/graphics/bitmap.h>
#include <smooth/graphics/color.h>
#include <smooth/misc/math.h>
#include <smooth/foreach.h>

S::GUI::SurfaceBackend *CreateSurfaceGDIPlus(S::Void *iSurface, const S::GUI::Size &maxSize)
{
	return new S::GUI::SurfaceGDIPlus(iSurface, maxSize);
}

S::Int		 surfaceGDITmp = S::GUI::SurfaceBackend::SetBackend(&CreateSurfaceGDIPlus);

S::Short	 S::GUI::SurfaceGDIPlus::surfaceDPI = -1;

S::GUI::SurfaceGDIPlus::SurfaceGDIPlus(Void *iWindow, const Size &maxSize)
{
	type = SURFACE_GDIPLUS;

	window = (HWND) iWindow;

	if (window != NIL)
	{
		HDC	 gdi_dc = GetWindowDC(window);

		size = maxSize;

		if (maxSize == Size())
		{
			size.cx	= GetDeviceCaps(gdi_dc, HORZRES) + 2;
			size.cy	= GetDeviceCaps(gdi_dc, VERTRES) + 2;
		}

		rightToLeft.SetSurfaceSize(size);

		Gdiplus::Graphics	*graphics = new Gdiplus::Graphics(gdi_dc);

		paintBitmap	= new Gdiplus::Bitmap(size.cx, size.cy, graphics);
		paintContext	= new Gdiplus::Graphics(paintBitmap);

		paintRects.Add(new Rect(Point(0, 0), size));

		delete graphics;

		ReleaseDC(window, gdi_dc);

		allocSize = size;
	}

	fontSize.SetFontSize(GetSurfaceDPI());
}

S::GUI::SurfaceGDIPlus::~SurfaceGDIPlus()
{
	if (window != NIL)
	{
		delete paintContext;
		delete paintBitmap;
		delete paintRects.GetFirst();
	}
}

S::Int S::GUI::SurfaceGDIPlus::SetSize(const Size &nSize)
{
	size = nSize;

	rightToLeft.SetSurfaceSize(size);

	if (allocSize.cx >= nSize.cx && allocSize.cy >= nSize.cy) return Success();

	if (window != NIL && !painting)
	{
		delete paintContext;
		delete paintBitmap;
		delete paintRects.GetFirst();

		paintRects.RemoveAll();

		HDC	 gdi_dc = GetWindowDC(window);

		Gdiplus::Graphics	*graphics = new Gdiplus::Graphics(gdi_dc);

		paintBitmap	= new Gdiplus::Bitmap(size.cx, size.cy, graphics);
		paintContext	= new Gdiplus::Graphics(paintBitmap);

		paintRects.Add(new Rect(Point(0, 0), size));

		delete graphics;

		ReleaseDC(window, gdi_dc);
	}

	allocSize = nSize;

	return Success();
}

const S::GUI::Size &S::GUI::SurfaceGDIPlus::GetSize() const
{
	return size;
}

S::Int S::GUI::SurfaceGDIPlus::PaintRect(const Rect &pRect)
{
	if (painting) return Error();

	if (window != NIL)
	{
		HDC			 gdi_dc = GetWindowDC(window);
		Gdiplus::Graphics	*graphics = new Gdiplus::Graphics(gdi_dc);

		Gdiplus::RectF		 srcRect(pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top);
		Gdiplus::RectF		 destRect(pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top);

		graphics->DrawImage(paintBitmap, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, Gdiplus::UnitPixel);

		delete graphics;

		ReleaseDC(window, gdi_dc);
	}

	return Success();
}

S::Int S::GUI::SurfaceGDIPlus::StartPaint(const Rect &iPRect)
{
	if (window == NIL) return Success();

	Rect	 pRect = Rect::OverlapRect(rightToLeft.TranslateRect(iPRect), *(paintRects.GetLast()));

	paintContext->SetClip(Gdiplus::Rect(pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top));

	paintRects.Add(new Rect(pRect));

	painting++;

	return Success();
}

S::Int S::GUI::SurfaceGDIPlus::EndPaint()
{
	if (!painting) return Error();

	painting--;

	if (painting == 0) PaintRect(*(paintRects.GetLast()));

	delete paintRects.GetLast();

	paintRects.RemoveNth(paintRects.Length() - 1);

	Rect	 pRect(*paintRects.GetLast());

	paintContext->SetClip(Gdiplus::Rect(pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top));

	return Success();
}

S::Void *S::GUI::SurfaceGDIPlus::GetSystemSurface() const
{
	return (Void *) window;
}

S::Short S::GUI::SurfaceGDIPlus::GetSurfaceDPI() const
{
	if (surfaceDPI != -1) return surfaceDPI;

	HDC	 dc  = GetWindowDC(0);
	Short	 dpi = GetDeviceCaps(dc, LOGPIXELSY);

	ReleaseDC(0, dc);

	surfaceDPI = dpi;

	return dpi;
}

S::Int S::GUI::SurfaceGDIPlus::SetPixel(const Point &iPoint, const Color &color)
{
	if (window == NIL) return Success();

	Point		 point = rightToLeft.TranslatePoint(iPoint);
	Gdiplus::Bitmap	 bitmap(1, 1);

	bitmap.SetPixel(0, 0, Gdiplus::Color(color.GetRed(), color.GetGreen(), color.GetBlue()));

	if (!painting)
	{
		HDC			 gdi_dc = GetWindowDC(window);
		Gdiplus::Graphics	*pGraphics = new Gdiplus::Graphics(gdi_dc);

		pGraphics->DrawImage(&bitmap, point.x, point.y);

		delete pGraphics;

		ReleaseDC(window, gdi_dc);
	}

	paintContext->DrawImage(&bitmap, point.x, point.y);

	return Success();
}

S::Int S::GUI::SurfaceGDIPlus::Line(const Point &iPos1, const Point &iPos2, const Color &color)
{
	if (window == NIL) return Success();

	Point	 pos1 = rightToLeft.TranslatePoint(iPos1);
	Point	 pos2 = rightToLeft.TranslatePoint(iPos2);

	/* Adjust to Windows GDI behavior for diagonal lines.
	 */
	if (Math::Abs(pos2.x - pos1.x) == Math::Abs(pos2.y - pos1.y))
	{
		if	(pos1.x < pos2.x) pos2.x--;
		else if (pos1.x > pos2.x) pos2.x++;

		if	(pos1.y < pos2.y) pos2.y--;
		else if (pos1.y > pos2.y) pos2.y++;
	}

	/* Adjust to Windows GDI behaviour for horizontal and vertical lines.
	 */
	if (pos1.x == pos2.x && pos1.y < pos2.y) pos2.y--;
	if (pos1.x == pos2.x && pos1.y > pos2.y) pos1.y--;

	if (pos1.y == pos2.y && pos1.x < pos2.x) pos2.x--;
	if (pos1.y == pos2.y && pos1.x > pos2.x) pos1.x--;

	if (pos1 == pos2) return SetPixel(rightToLeft.TranslatePoint(pos1), color);

	Gdiplus::Pen	 pen(Gdiplus::Color(color.GetRed(), color.GetGreen(), color.GetBlue()));

	if (!painting)
	{
		HDC			 gdi_dc = GetWindowDC(window);
		Gdiplus::Graphics	*pGraphics = new Gdiplus::Graphics(gdi_dc);

		pGraphics->DrawLine(&pen, pos1.x, pos1.y, pos2.x, pos2.y);

		delete pGraphics;

		ReleaseDC(window, gdi_dc);
	}

	paintContext->DrawLine(&pen, pos1.x, pos1.y, pos2.x, pos2.y);

	return Success();
}

S::Int S::GUI::SurfaceGDIPlus::Box(const Rect &iRect, const Color &color, Int style, const Size &ellipse)
{
	if (window == NIL) return Success();

	Rect	 rect = rightToLeft.TranslateRect(iRect);

	Gdiplus::SolidBrush	 gdip_brush(Gdiplus::Color(color.GetRed(), color.GetGreen(), color.GetBlue()));
	Gdiplus::Pen		 gdip_pen(Gdiplus::Color(color.GetRed(), color.GetGreen(), color.GetBlue()));
	Gdiplus::Rect		 gdip_rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	HDC			 gdi_dc	   = NIL;
	Gdiplus::Graphics	*pGraphics = NIL;

	if (!painting)
	{
		gdi_dc	  = GetWindowDC(window);

		pGraphics = new Gdiplus::Graphics(gdi_dc);
	}

	if (style & Rect::Filled)
	{
		if (style & Rect::Rounded)
		{
			/* ToDo: Allow drawing of rounded rects.
			 */
		}
		else
		{
			if (!painting) pGraphics->FillRectangle(&gdip_brush, gdip_rect);

			paintContext->FillRectangle(&gdip_brush, gdip_rect);
		}
	}
	else if (style == Rect::Outlined)
	{
		gdip_rect.Width--;
		gdip_rect.Height--;

		if (!painting) pGraphics->DrawRectangle(&gdip_pen, gdip_rect);

		paintContext->DrawRectangle(&gdip_pen, gdip_rect);
	}
	else if (style & Rect::Inverted)
	{
		Bitmap	 area(rect.right - rect.left, rect.bottom - rect.top);

		BlitToBitmap(iRect, area, Rect(Point(0, 0), area.GetSize()));

		area.InvertColors();

		BlitFromBitmap(area, Rect(Point(0, 0), area.GetSize()), iRect);
	}
	else if (style & Rect::Dotted)
	{
		if (!painting)
		{
			for (Int x = rect.left								 + 1; x <  rect.right;	 x += 2) pGraphics->DrawLine(&gdip_pen, x, rect.top, x, rect.top - 1);
			for (Int y = rect.top	 - (rect.right - rect.left			   ) % 2 + 2; y <  rect.bottom;	 y += 2) pGraphics->DrawLine(&gdip_pen, rect.right - 1, y, rect.right, y);
			for (Int x = rect.right	 - (rect.right - rect.left + rect.bottom - rect.top) % 2 - 2; x >= rect.left;	 x -= 2) pGraphics->DrawLine(&gdip_pen, x, rect.bottom - 1, x, rect.bottom);
			for (Int y = rect.bottom - (			     rect.bottom - rect.top) % 2 - 1; y >= rect.top;	 y -= 2) pGraphics->DrawLine(&gdip_pen, rect.left, y, rect.left - 1, y);
		}

		for (Int x = rect.left								 + 1;  x <  rect.right;	 x += 2) paintContext->DrawLine(&gdip_pen, x, rect.top, x, rect.top - 1);
		for (Int y = rect.top	 - (rect.right - rect.left			   ) % 2 + 2;  y <  rect.bottom; y += 2) paintContext->DrawLine(&gdip_pen, rect.right - 1, y, rect.right, y);
		for (Int x = rect.right	 - (rect.right - rect.left + rect.bottom - rect.top) % 2 - 2;  x >= rect.left;	 x -= 2) paintContext->DrawLine(&gdip_pen, x, rect.bottom - 1, x, rect.bottom);
		for (Int y = rect.bottom - (			     rect.bottom - rect.top) % 2 - 1;  y >= rect.top;	 y -= 2) paintContext->DrawLine(&gdip_pen, rect.left, y, rect.left - 1, y);
	}

	if (!painting)
	{
		delete pGraphics;

		ReleaseDC(window, gdi_dc);
	}

	return Success();
}

S::Int S::GUI::SurfaceGDIPlus::SetText(const String &string, const Rect &iRect, const Font &font, Bool shadow)
{
	if (window == NIL)	return Success();

	if (string == NIL)	return Error();
	if (shadow)		return SurfaceBackend::SetText(string, iRect, font, shadow);

	Rect			 rect	    = iRect;
	Int			 lineHeight = font.GetScaledTextSizeY() + 3;

	Color			 color = font.GetColor();

	Gdiplus::Font		 gdip_font(font.GetName(), fontSize.TranslateY(font.GetSize()), (font.GetWeight() >= Font::Bold    ? Gdiplus::FontStyleBold	 : Gdiplus::FontStyleRegular) |
												(font.GetStyle() & Font::Italic    ? Gdiplus::FontStyleItalic    : Gdiplus::FontStyleRegular) |
												(font.GetStyle() & Font::Underline ? Gdiplus::FontStyleUnderline : Gdiplus::FontStyleRegular) |
												(font.GetStyle() & Font::StrikeOut ? Gdiplus::FontStyleStrikeout : Gdiplus::FontStyleRegular));
	Gdiplus::SolidBrush	 gdip_brush(Gdiplus::Color(color.GetRed(), color.GetGreen(), color.GetBlue()));
	Gdiplus::StringFormat	 gdip_format(Gdiplus::StringFormatFlagsNoWrap);

	HDC			 gdi_dc	   = NIL;
	Gdiplus::Graphics	*pGraphics = NIL;

	if (!painting)
	{
		gdi_dc	  = GetWindowDC(window);

		pGraphics = new Gdiplus::Graphics(gdi_dc);
	}

	const Array<String>	&lines = string.Explode("\n");

	foreach (const String &line, lines)
	{
		Bool	 rtlCharacters = False;

		for (Int i = 0; i < line.Length(); i++) if (line[i] >= 0x0590 && line[i] <= 0x07BF) { rtlCharacters = True; break; }

		Rect		 tRect = rightToLeft.TranslateRect(rect);
		Gdiplus::RectF	 gdip_rect(tRect.left, tRect.top, tRect.right - tRect.left + 2, tRect.bottom - tRect.top);

		if (rtlCharacters) gdip_format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsDirectionRightToLeft);
		else		   gdip_format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

		if ((!rtlCharacters &&  rightToLeft.GetRightToLeft()) ||
		    ( rtlCharacters && !rightToLeft.GetRightToLeft())) gdip_format.SetAlignment(Gdiplus::StringAlignmentFar);
		else						       gdip_format.SetAlignment(Gdiplus::StringAlignmentNear);

		if (!painting) pGraphics->DrawString(line, -1, &gdip_font, gdip_rect, &gdip_format, &gdip_brush);

		paintContext->DrawString(line, -1, &gdip_font, gdip_rect, &gdip_format, &gdip_brush);

		rect.top += lineHeight;
	}

	String::ExplodeFinish();

	if (!painting)
	{
		delete pGraphics;

		ReleaseDC(window, gdi_dc);
	}

	return Success();
}

S::Int S::GUI::SurfaceGDIPlus::BlitFromBitmap(const Bitmap &bitmap, const Rect &srcRect, const Rect &iDestRect)
{
	if (window == NIL) return Success();
	if (bitmap == NIL) return Error();

	Rect			 destRect = rightToLeft.TranslateRect(iDestRect);
	HDC			 gdi_dc = GetWindowDC(window);
	Gdiplus::Graphics	*screen = new Gdiplus::Graphics(gdi_dc);
	Gdiplus::Bitmap		 gdip_bitmap((HBITMAP) bitmap.GetSystemBitmap(), NIL);

	/* Copy the image.
	 */
	if (!painting)
	{
		screen->DrawImage(&gdip_bitmap, Gdiplus::Rect(destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top), srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, Gdiplus::UnitPixel, NIL, NIL, NIL);
	}

	paintContext->DrawImage(&gdip_bitmap, Gdiplus::Rect(destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top), srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, Gdiplus::UnitPixel, NIL, NIL, NIL);

	delete screen;

	ReleaseDC(window, gdi_dc);

	return Success();
}

S::Int S::GUI::SurfaceGDIPlus::BlitToBitmap(const Rect &iSrcRect, Bitmap &bitmap, const Rect &destRect)
{
	if (window == NIL) return Success();
	if (bitmap == NIL) return Error();

	Rect	 srcRect = rightToLeft.TranslateRect(iSrcRect);

	Gdiplus::Bitmap		 gdip_bitmap((HBITMAP) bitmap.GetSystemBitmap(), NIL);
	Gdiplus::Graphics	 gdip_graphics(&gdip_bitmap);

	/* Copy the image.
	 */
	gdip_graphics.DrawImage(paintBitmap, Gdiplus::Rect(destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top), srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, Gdiplus::UnitPixel, NIL, NIL, NIL);

	HBITMAP			 hBitmap = NIL;

	gdip_bitmap.GetHBITMAP(Gdiplus::Color(), &hBitmap);

	bitmap.SetSystemBitmap(hBitmap);

	::DeleteObject(hBitmap);

	return Success();
}
