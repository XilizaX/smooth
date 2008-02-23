 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/window/backends/xlib/windowxlib.h>
#include <smooth/gui/window/window.h>
#include <smooth/misc/math.h>
#include <smooth/system/system.h>
#include <smooth/init.h>
#include <smooth/backends/xlib/backendxlib.h>

S::GUI::WindowBackend *CreateWindowXLib()
{
	return new S::GUI::WindowXLib();
}

S::Int	 windowXLibTmp = S::GUI::WindowBackend::SetBackend(&CreateWindowXLib);

S::Array<S::GUI::WindowXLib *, S::Void *>	 S::GUI::WindowXLib::windowBackends;

S::GUI::WindowXLib::WindowXLib(Void *iWindow)
{
	type	= WINDOW_XLIB;

	wnd	= NIL;
	display	= Backends::BackendXLib::GetDisplay();

	id	= windowBackends.Add(this);
}

S::GUI::WindowXLib::~WindowXLib()
{
	windowBackends.Remove(id);
}

S::Void *S::GUI::WindowXLib::GetSystemWindow() const
{
	return (Void *) wnd;
}

S::GUI::WindowXLib *S::GUI::WindowXLib::GetWindowBackend(::Window wnd)
{
	if (wnd == 0) return NIL;

	for (Int i = 0; i < windowBackends.Length(); i++)
	{
		WindowXLib	*window = windowBackends.GetNth(i);

		if (window != NIL)
		{
			if (window->wnd == wnd) return window;
		}
	}

	return NIL;
}

S::Int S::GUI::WindowXLib::ProcessSystemMessages(XEvent *e)
{
	switch (e->type)
	{
		case CreateNotify:
			break;
		case DestroyNotify:
			break;
		case MapNotify:
			onEvent.Call(SM_PAINT, 0, 0);

			XFlush(display);

			break;
		case UnmapNotify:
			break;
		case MotionNotify:
			onEvent.Call(SM_MOUSEMOVE, 0, 0);

			break;
		case ButtonPress:
			if	(e->xbutton.button = Button1) onEvent.Call(SM_LBUTTONDOWN, 0, 0);
			else if (e->xbutton.button = Button2) onEvent.Call(SM_RBUTTONDOWN, 0, 0);

			break;
		case ButtonRelease:
			if	(e->xbutton.button = Button1) onEvent.Call(SM_LBUTTONUP, 0, 0);
			else if (e->xbutton.button = Button2) onEvent.Call(SM_RBUTTONUP, 0, 0);

			break;
		case KeyPress:
			break;
		case KeyRelease:
			break;
	}

	return Success();
}

S::Int S::GUI::WindowXLib::Open(const String &title, const Point &pos, const Size &size, Int flags)
{
	XSetWindowAttributes	 attributes;

	attributes.background_pixel	= BlackPixel(display, 0);
	attributes.border_pixel		= BlackPixel(display, 0);
	attributes.override_redirect	= True;

	wnd = XCreateWindow(display, RootWindow(display, 0), pos.x, pos.y, size.cx, size.cy, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWBorderPixel | CWOverrideRedirect, &attributes);

	/* Select event types wanted */
	XSelectInput(display, wnd, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);

	if (wnd != NIL)
	{
		XMapWindow(display, wnd);

		XFlush(display);

		drawSurface = new Surface((Void *) wnd);
		drawSurface->SetSize(size);

		return Success();
	}

	return Error();
}

S::Int S::GUI::WindowXLib::Close()
{
	if (drawSurface != nullSurface) delete drawSurface;

	drawSurface = nullSurface;

	XDestroyWindow(display, wnd);

	return Success();
}
