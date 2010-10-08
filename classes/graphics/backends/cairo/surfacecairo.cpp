 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/graphics/backends/cairo/surfacecairo.h>
#include <smooth/graphics/surface.h>
#include <smooth/graphics/bitmap.h>
#include <smooth/graphics/color.h>
#include <smooth/misc/math.h>

#ifdef __WIN32__
#	include <cairowin/cairo-win32.h>
#else
#	include <cairo/cairo-xlib.h>
#	include <smooth/backends/xlib/backendxlib.h>
#endif

S::GUI::SurfaceBackend *CreateSurfaceCairo(S::Void *iSurface, const S::GUI::Size &maxSize)
{
	return new S::GUI::SurfaceCairo(iSurface, maxSize);
}

S::Int		 surfaceCairoTmp = S::GUI::SurfaceBackend::SetBackend(&CreateSurfaceCairo);

S::Short	 S::GUI::SurfaceCairo::surfaceDPI = -1;

S::GUI::SurfaceCairo::SurfaceCairo(Void *iWindow, const Size &maxSize)
{
	type	= SURFACE_CAIRO;

#ifdef __WIN32__
	window	= (HWND) iWindow;

	gdi_dc	= NIL;
#else
	display	= Backends::BackendXLib::GetDisplay();
	visual	= XDefaultVisual(display, XDefaultScreen(display));

	window	= (Window) iWindow;
#endif

	context = NIL;
	surface = NIL;

	if (window != NIL)
	{
		size = maxSize;

#ifdef __WIN32__
		HDC	 gdi_dc = GetWindowDC(window);

		if (maxSize == Size())
		{
			size.cx	= GetDeviceCaps(gdi_dc, HORZRES) + 2;
			size.cy	= GetDeviceCaps(gdi_dc, VERTRES) + 2;
		}
#else
		XGetWindowAttributes(display, window, &windowAttributes);

		if (maxSize == Size())
		{
			size.cx = XDisplayWidth(display, XDefaultScreen(display)) + 2;
			size.cy = XDisplayHeight(display, XDefaultScreen(display)) + 2;
		}
#endif

		rightToLeft.SetSurfaceSize(size);

#ifdef __WIN32__
		paintContext = CreateCompatibleDC(gdi_dc);

		HBITMAP	 bitmap = CreateCompatibleBitmap(gdi_dc, size.cx, size.cy);

		paintBitmap	  = (HBITMAP) SelectObject(paintContext, bitmap);
		paintSurfaceCairo = cairo_win32_surface_create(paintContext);

		ReleaseDC(window, gdi_dc);
#else
		paintBitmap	  = XCreatePixmap(display, DefaultRootWindow(display), size.cx, size.cy, windowAttributes.depth);
		paintSurfaceCairo = cairo_xlib_surface_create(display, paintBitmap, visual, size.cx, size.cy);
#endif

		paintRects.Add(new Rect(Point(0, 0), size));

		paintContextCairo = cairo_create(paintSurfaceCairo);
	
		cairo_set_antialias(paintContextCairo, CAIRO_ANTIALIAS_NONE);

		allocSize = size;
	}
}

S::GUI::SurfaceCairo::~SurfaceCairo()
{
	if (window != NIL)
	{
		cairo_destroy(paintContextCairo);
		cairo_surface_destroy(paintSurfaceCairo);

#ifdef __WIN32__
		HBITMAP	 bitmap = (HBITMAP) SelectObject(paintContext, paintBitmap);

		DeleteDC(paintContext);
		::DeleteObject(bitmap);
#else
		XFreePixmap(display, paintBitmap);
#endif

		delete paintRects.GetFirst();
	}
}

S::Int S::GUI::SurfaceCairo::SetSize(const Size &nSize)
{
	size = nSize;

	rightToLeft.SetSurfaceSize(size);

	if (allocSize.cx >= nSize.cx && allocSize.cy >= nSize.cy) return Success();

	if (window != NIL && !painting)
	{
		cairo_destroy(paintContextCairo);
		cairo_surface_destroy(paintSurfaceCairo);

#ifdef __WIN32__
		HBITMAP	 bitmap = (HBITMAP) SelectObject(paintContext, paintBitmap);

		::DeleteObject(bitmap);
#else
		XFreePixmap(display, paintBitmap);
#endif

		delete paintRects.GetFirst();

		paintRects.RemoveAll();

#ifdef __WIN32__
		HDC	 gdi_dc = GetWindowDC(window);

		bitmap = CreateCompatibleBitmap(gdi_dc, size.cx, size.cy);

		paintBitmap	  = (HBITMAP) SelectObject(paintContext, bitmap);
		paintSurfaceCairo = cairo_win32_surface_create(paintContext);

		ReleaseDC(window, gdi_dc);
#else
		paintBitmap	  = XCreatePixmap(display, DefaultRootWindow(display), size.cx, size.cy, windowAttributes.depth);
		paintSurfaceCairo = cairo_xlib_surface_create(display, paintBitmap, visual, size.cx, size.cy);
#endif

		paintRects.Add(new Rect(Point(0, 0), size));

		paintContextCairo = cairo_create(paintSurfaceCairo);

		cairo_set_antialias(paintContextCairo, CAIRO_ANTIALIAS_NONE);
	}

	allocSize = nSize;

	return Success();
}

const S::GUI::Size &S::GUI::SurfaceCairo::GetSize() const
{
	return size;
}

S::Int S::GUI::SurfaceCairo::PaintRect(const Rect &pRect)
{
	if (painting) return Error();

	if (window != NIL)
	{
#ifdef __WIN32__
		HDC	 gdi_dc = GetWindowDC(window);

		BitBlt(gdi_dc, pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top, paintContext, pRect.left, pRect.top, SRCCOPY);

		ReleaseDC(window, gdi_dc);
#else
		GC	 gc = XCreateGC(display, window, 0, NIL);

		XCopyArea(display, paintBitmap, window, gc, pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top, pRect.left, pRect.top);

		XFreeGC(display, gc);
#endif
	}

	return Success();
}

S::Int S::GUI::SurfaceCairo::StartPaint(const Rect &iPRect)
{
	if (window == NIL) return Success();

	Rect	 pRect = Rect::OverlapRect(rightToLeft.TranslateRect(fontSize.TranslateRect(iPRect)), *(paintRects.GetLast()));

	cairo_save(paintContextCairo);
	cairo_rectangle(paintContextCairo, pRect.left, pRect.top, pRect.right - pRect.left, pRect.bottom - pRect.top);
	cairo_clip(paintContextCairo);

	paintRects.Add(new Rect(pRect));

	painting++;

	return Success();
}

S::Int S::GUI::SurfaceCairo::EndPaint()
{
	if (!painting) return Error();

	painting--;

	if (painting == 0) PaintRect(*(paintRects.GetLast()));

	delete paintRects.GetLast();

	paintRects.Remove(paintRects.GetNthIndex(paintRects.Length() - 1));

	cairo_restore(paintContextCairo);

	return Success();
}

S::Void *S::GUI::SurfaceCairo::GetSystemSurface() const
{
	return (Void *) window;
}

S::Short S::GUI::SurfaceCairo::GetSurfaceDPI() const
{
	if (surfaceDPI != -1) return surfaceDPI;

	Short	 dpi = 96;

	surfaceDPI = dpi;

	return dpi;
}

S::Void S::GUI::SurfaceCairo::CreateCairoContext()
{
	if (context != NIL || surface != NIL) return;

#ifdef __WIN32__
	gdi_dc = GetWindowDC(window);

	surface = cairo_win32_surface_create(gdi_dc);
#else
	surface = cairo_xlib_surface_create(display, window, visual, size.cx, size.cy);
#endif

	context = cairo_create(surface);

	cairo_set_antialias(context, CAIRO_ANTIALIAS_NONE);
}

S::Void S::GUI::SurfaceCairo::DestroyCairoContext()
{
	if (context == NIL || surface == NIL) return;

	cairo_destroy(context);
	cairo_surface_destroy(surface);

	context = NIL;
	surface = NIL;

#ifdef __WIN32__
	ReleaseDC(window, gdi_dc);

	gdi_dc = NIL;
#endif
}

S::Int S::GUI::SurfaceCairo::SetPixel(const Point &point, const Color &color)
{
	if (window == NIL) return Success();

	if (!painting)
	{
		CreateCairoContext();

		cairo_set_source_rgb(context, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
		cairo_rectangle(context, rightToLeft.TranslateX(fontSize.TranslateX(point.x)), rightToLeft.TranslateY(fontSize.TranslateY(point.y)), 1, 1);
		cairo_fill(context);

		DestroyCairoContext();
	}

	cairo_set_source_rgb(paintContextCairo, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
	cairo_rectangle(paintContextCairo, rightToLeft.TranslateX(fontSize.TranslateX(point.x)), rightToLeft.TranslateY(fontSize.TranslateY(point.y)), 1, 1);
	cairo_fill(paintContextCairo);

	return Success();
}

S::Int S::GUI::SurfaceCairo::Line(const Point &iPos1, const Point &iPos2, const Color &color)
{
	if (window == NIL) return Success();

	Point	 pos1 = iPos1;
	Point	 pos2 = iPos2;

	/* Adjust to Windows GDI behavior for diagonal lines.
	 */
	if (Math::Abs(pos2.x - pos1.x) == Math::Abs(pos2.y - pos1.y))
	{
		if (pos1.x < pos2.x && pos1.y > pos2.y) { pos1.y++; pos2.y++; }
		if (pos1.x > pos2.x && pos1.y < pos2.y) { pos1.x++; pos2.x++; }
	} 

	pos1 = rightToLeft.TranslatePoint(fontSize.TranslatePoint(pos1));
	pos2 = rightToLeft.TranslatePoint(fontSize.TranslatePoint(pos2));

	/* Adjust to X11 behaviour.
	 */
#ifdef __WIN32__
	Float	 xAdjust = 0;
#else
	Float	 xAdjust = 0.5;
#endif

	if (!painting)
	{
		CreateCairoContext();

		cairo_set_source_rgb(context, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
		cairo_set_line_width(context, 1);
		cairo_move_to(context, pos1.x + xAdjust, pos1.y);
		cairo_line_to(context, pos2.x + xAdjust, pos2.y);
		cairo_stroke(context);

		DestroyCairoContext();
	}

	cairo_set_source_rgb(paintContextCairo, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
	cairo_set_line_width(paintContextCairo, 1);
	cairo_move_to(paintContextCairo, pos1.x + xAdjust, pos1.y);
	cairo_line_to(paintContextCairo, pos2.x + xAdjust, pos2.y);
	cairo_stroke(paintContextCairo);

	return Success();
}

S::Int S::GUI::SurfaceCairo::Box(const Rect &iRect, const Color &color, Int style, const Size &ellipse)
{
	if (window == NIL) return Success();

	Rect	 rect = rightToLeft.TranslateRect(fontSize.TranslateRect(iRect));

	CreateCairoContext();

	cairo_set_source_rgb(context, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
	cairo_set_source_rgb(paintContextCairo, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);

	if (style & Rect::Filled)
	{
		if (style & Rect::Rounded)
		{
		}
		else
		{
			if (!painting)
			{
				cairo_rectangle(context, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
				cairo_fill(context);
			}

			cairo_rectangle(paintContextCairo, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
			cairo_fill(paintContextCairo);
		}
	}
	else if (style == Rect::Outlined)
	{
		if (!painting)
		{
			cairo_set_line_width(context, 1);
			cairo_rectangle(context, rect.left + 0.5, rect.top + 0.5, rect.right - rect.left - 1, rect.bottom - rect.top - 1);
			cairo_stroke(context);
		}

		cairo_set_line_width(paintContextCairo, 1);
		cairo_rectangle(paintContextCairo, rect.left + 0.5, rect.top + 0.5, rect.right - rect.left - 1, rect.bottom - rect.top - 1);
		cairo_stroke(paintContextCairo);
	}
	else if (style & Rect::Inverted)
	{
	}
	else if (style & Rect::Dotted)
	{
		Bool	 dot = False;
		Int	 x;
		Int	 y = rect.top;

		for (x = rect.left; x < rect.right - 1; x++)
		{
			if (dot == True)
			{
				if (!painting)
				{
					cairo_rectangle(context, x, y, 1, 1);
					cairo_fill(context);
				}

				cairo_rectangle(paintContextCairo, x, y, 1, 1);
				cairo_fill(paintContextCairo);

				dot = False;
			}
			else dot = True;
		}

		x = rect.right - 1;

		for (y = rect.top; y < rect.bottom; y++)
		{
			if (dot == True)
			{
				if (!painting)
				{
					cairo_rectangle(context, x, y, 1, 1);
					cairo_fill(context);
				}

				cairo_rectangle(paintContextCairo, x, y, 1, 1);
				cairo_fill(paintContextCairo);

				dot = False;
			}
			else dot = True;
		}

		y = rect.bottom - 1;

		for (x = rect.right - 2; x >= rect.left; x--)
		{
			if (dot == True)
			{
				if (!painting)
				{
					cairo_rectangle(context, x, y, 1, 1);
					cairo_fill(context);
				}

				cairo_rectangle(paintContextCairo, x, y, 1, 1);
				cairo_fill(paintContextCairo);

				dot = False;
			}
			else dot = True;
		}

		x = rect.left;

		for (y = rect.bottom - 2; y >= rect.top; y--)
		{
			if (dot == True)
			{
				if (!painting)
				{
					cairo_rectangle(context, x, y, 1, 1);
					cairo_fill(context);
				}

				cairo_rectangle(paintContextCairo, x, y, 1, 1);
				cairo_fill(paintContextCairo);

				dot = False;
			}
			else dot = True;
		}
	}

	DestroyCairoContext();

	return Success();
}

S::Int S::GUI::SurfaceCairo::SetText(const String &string, const Rect &iRect, const Font &font, Bool shadow)
{
	if (window == NIL)	return Success();

	if (string == NIL)	return Error();
	if (shadow)		return SurfaceBackend::SetText(string, iRect, font, shadow);

	int	 lines = 1;
	int	 offset = 0;
	int	 origoffset;
	int	 txtsize = string.Length();
	String	 line;
	Rect	 rect = iRect;
	Int	 lineHeight = font.GetTextSizeY() + 3;

	if (rightToLeft.GetRightToLeft()) rect.right--;

	for (Int j = 0; j < txtsize; j++) if (string[j] == 10) lines++;

	for (Int i = 0; i < lines; i++)
	{
		Bool	 rtlCharacters = False;

		origoffset = offset;

		for (Int j = 0; j <= txtsize; j++)
		{
			if (j + origoffset == txtsize)
			{
				line[j] = 0;
				break;
			}

			if (string[j + origoffset] == 10 || string[j + origoffset] == 0)
			{
				offset++;
				line[j] = 0;
				break;
			}
			else
			{
				offset++;
				line[j] = string[j + origoffset];

				if (line[j] >= 0x0590 && line[j] <= 0x07BF) rtlCharacters = True;
			}
		}

		rect = rightToLeft.TranslateRect(fontSize.TranslateRect(rect));

		if (!painting)
		{
			CreateCairoContext();

			cairo_select_font_face(context, font.GetName(),
					       (font.GetStyle() == Font::Italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL),
					       (font.GetWeight() == Font::Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL));

			cairo_set_font_size(context, font.GetSize() * fontSize.TranslateY(96) / 72.0);

			cairo_set_source_rgb(context, font.GetColor().GetRed() / 255.0, font.GetColor().GetGreen() / 255.0, font.GetColor().GetBlue() / 255.0);

			cairo_move_to(context, rect.left, rect.top + font.GetSize() * fontSize.TranslateY(96) / 72.0);
			cairo_show_text(context, line.ConvertTo("UTF-8"));

			DestroyCairoContext();
		}

		cairo_select_font_face(paintContextCairo, font.GetName(),
				       (font.GetStyle() == Font::Italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL),
				       (font.GetWeight() == Font::Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL));

		cairo_set_font_size(paintContextCairo, font.GetSize() * fontSize.TranslateY(96) / 72.0);

		cairo_set_source_rgb(paintContextCairo, font.GetColor().GetRed() / 255.0, font.GetColor().GetGreen() / 255.0, font.GetColor().GetBlue() / 255.0);

		cairo_move_to(paintContextCairo, rect.left, rect.top + font.GetSize() * fontSize.TranslateY(96) / 72.0);
		cairo_show_text(paintContextCairo, line.ConvertTo("UTF-8"));

		rect.top += lineHeight;
	}

	return Success();
}

S::Int S::GUI::SurfaceCairo::Gradient(const Rect &iRect, const Color &color1, const Color &color2, Int style)
{
	if (window == NIL) return Success();

	Rect	 rect = rightToLeft.TranslateRect(fontSize.TranslateRect(iRect));

	cairo_pattern_t	*pattern = NIL;

	switch (style)
	{
		case OR_HORZ:
			pattern = cairo_pattern_create_linear(0, 0, rect.right - rect.left, 0);

			if (rightToLeft.GetRightToLeft())
			{
				cairo_pattern_add_color_stop_rgb(pattern, 1, color1.GetRed() / 255.0, color1.GetGreen() / 255.0, color1.GetBlue() / 255.0);
				cairo_pattern_add_color_stop_rgb(pattern, 0, color2.GetRed() / 255.0, color2.GetGreen() / 255.0, color2.GetBlue() / 255.0);
			}
			else
			{
				cairo_pattern_add_color_stop_rgb(pattern, 0, color1.GetRed() / 255.0, color1.GetGreen() / 255.0, color1.GetBlue() / 255.0);
				cairo_pattern_add_color_stop_rgb(pattern, 1, color2.GetRed() / 255.0, color2.GetGreen() / 255.0, color2.GetBlue() / 255.0);
			}

			break;
		case OR_VERT:
			pattern = cairo_pattern_create_linear(0, 0, 0, rect.bottom - rect.top);

			cairo_pattern_add_color_stop_rgb(pattern, 0, color1.GetRed() / 255.0, color1.GetGreen() / 255.0, color1.GetBlue() / 255.0);
			cairo_pattern_add_color_stop_rgb(pattern, 1, color2.GetRed() / 255.0, color2.GetGreen() / 255.0, color2.GetBlue() / 255.0);

			break;
	}

	if (!painting)
	{
		CreateCairoContext();

		cairo_set_source(context, pattern);
		cairo_rectangle(context, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		cairo_fill(context);

		DestroyCairoContext();
	}

	cairo_set_source(paintContextCairo, pattern);
	cairo_rectangle(paintContextCairo, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	cairo_fill(paintContextCairo);

	cairo_pattern_destroy(pattern);

	return Success();
}

S::Int S::GUI::SurfaceCairo::BlitFromBitmap(const Bitmap &oBitmap, const Rect &srcRect, const Rect &iDestRect)
{
	if (window  == NIL) return Success();
	if (oBitmap == NIL) return Error();

	Bitmap	 bitmap	  = oBitmap;
	Rect	 destRect = rightToLeft.TranslateRect(fontSize.TranslateRect(iDestRect));

#ifdef __WIN32__
	HDC	 gdi_dc	  = GetWindowDC(window);
	HDC	 cdc	  = CreateCompatibleDC(gdi_dc);
	HBITMAP	 backup	  = (HBITMAP) SelectObject(cdc, bitmap.GetSystemBitmap());

	if ((destRect.right - destRect.left == srcRect.right - srcRect.left) && (destRect.bottom - destRect.top == srcRect.bottom - srcRect.top))
	{
		if (!painting)
		{
			BitBlt(gdi_dc, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, cdc, srcRect.left, srcRect.top, SRCCOPY);
		}

		BitBlt(paintContext, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, cdc, srcRect.left, srcRect.top, SRCCOPY);
	}
	else
	{
		if (!painting)
		{
			SetStretchBltMode(gdi_dc, HALFTONE);
			StretchBlt(gdi_dc, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, cdc, srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, SRCCOPY);
		}

		SetStretchBltMode(paintContext, HALFTONE);
		StretchBlt(paintContext, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, cdc, srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, SRCCOPY);
	}

	SelectObject(cdc, backup);

	DeleteDC(cdc);
	ReleaseDC(window, gdi_dc);
#else
	GC	 gc = XCreateGC(display, window, 0, NIL);

	if ((destRect.right - destRect.left == srcRect.right - srcRect.left) && (destRect.bottom - destRect.top == srcRect.bottom - srcRect.top))
	{
		if (!painting)
		{
			XCopyArea(display, (Pixmap) bitmap.GetSystemBitmap(), window, gc, srcRect.left, srcRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, destRect.left, destRect.top);
		}

		XCopyArea(display, (Pixmap) bitmap.GetSystemBitmap(), paintBitmap, gc, srcRect.left, srcRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, destRect.left, destRect.top);
	}
	else
	{
		/* ToDo: Allow copying from bitmaps of different
		 *	 size than destination.
		 */
	}

	XFreeGC(display, gc);
#endif

	return Success();
}

S::Int S::GUI::SurfaceCairo::BlitToBitmap(const Rect &iSrcRect, const Bitmap &oBitmap, const Rect &destRect)
{
	if (window  == NIL) return Success();
	if (oBitmap == NIL) return Error();

	Bitmap	 bitmap	 = oBitmap;
	Rect	 srcRect = rightToLeft.TranslateRect(fontSize.TranslateRect(iSrcRect));

#ifdef __WIN32__
	HDC	 gdi_dc	 = GetWindowDC(window);
	HDC	 cdc	 = CreateCompatibleDC(gdi_dc);
	HBITMAP	 backup	 = (HBITMAP) SelectObject(cdc, bitmap.GetSystemBitmap());

	if ((destRect.right - destRect.left == srcRect.right - srcRect.left) && (destRect.bottom - destRect.top == srcRect.bottom - srcRect.top))
	{
		BitBlt(cdc, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, gdi_dc, srcRect.left, srcRect.top, SRCCOPY);
	}
	else
	{
		SetStretchBltMode(cdc, HALFTONE);
		StretchBlt(cdc, destRect.left, destRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, gdi_dc, srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, SRCCOPY);
	}

	SelectObject(cdc, backup);

	DeleteDC(cdc);
	ReleaseDC(window, gdi_dc);
#else
	GC	 gc = XCreateGC(display, (Pixmap) bitmap.GetSystemBitmap(), 0, NIL);

	if ((destRect.right - destRect.left == srcRect.right - srcRect.left) && (destRect.bottom - destRect.top == srcRect.bottom - srcRect.top))
	{
		XCopyArea(display, window, (Pixmap) bitmap.GetSystemBitmap(), gc, srcRect.left, srcRect.top, destRect.right - destRect.left, destRect.bottom - destRect.top, destRect.left, destRect.top);
	}
	else
	{
		/* ToDo: Allow copying to bitmaps of different
		 *	 size than original.
		 */
	}

	XFreeGC(display, gc);
#endif

	return Success();
}
