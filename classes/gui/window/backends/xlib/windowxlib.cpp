 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/window/backends/xlib/windowxlib.h>
#include <smooth/gui/window/window.h>
#include <smooth/input/pointer.h>
#include <smooth/misc/math.h>
#include <smooth/system/system.h>
#include <smooth/init.h>
#include <smooth/backends/xlib/backendxlib.h>

using namespace X11;

S::GUI::WindowBackend *CreateWindowXLib()
{
	return new S::GUI::WindowXLib();
}

S::Int	 windowXLibTmp = S::GUI::WindowBackend::SetBackend(&CreateWindowXLib);

S::Array<S::GUI::WindowXLib *, S::Void *>	 S::GUI::WindowXLib::windowBackends;

S::GUI::WindowXLib::WindowXLib(Void *iWindow)
{
	type	= WINDOW_XLIB;

	display	= Backends::BackendXLib::GetDisplay();
	im	= Backends::BackendXLib::GetIM();

	wnd	= NIL;
	oldwnd	= NIL;

	ic	= NIL;

	id	= windowBackends.Add(this);

	minSize	= Size(160, 24);

	flags	= 0;
}

S::GUI::WindowXLib::~WindowXLib()
{
	windowBackends.Remove(id);
}

S::Void S::GUI::WindowXLib::UpdateWMNormalHints()
{
	if (wnd == NIL) return;

	XSizeHints	 normal;

	normal.flags	   = (minSize != Size() ? PMinSize : 0) |
			     (maxSize != Size() ? PMaxSize : 0) |
			     (flags & WF_NORESIZE ? PPosition | PSize : 0);

	normal.x	   = pos.x;
	normal.y	   = pos.y;

	normal.width	   = size.cx;
	normal.height	   = size.cy;

	normal.min_width   = minSize.cx;
	normal.min_height  = minSize.cy;

	normal.max_width   = maxSize.cx;
	normal.max_height  = maxSize.cy;

	XSetWMNormalHints(display, wnd, &normal);
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
			if (window->wnd    == wnd ||
			    window->oldwnd == wnd) return window;
		}
	}

	return NIL;
}

S::Input::Keyboard::Key S::GUI::WindowXLib::ConvertKey(Int keySym)
{
	Input::Keyboard::Key	 key = Input::Keyboard::KeyOther;

	switch (keySym)
	{
		case XK_Left:	   key = Input::Keyboard::KeyLeft;    break;
		case XK_Up:	   key = Input::Keyboard::KeyUp;      break;
		case XK_Right:	   key = Input::Keyboard::KeyRight;   break;
		case XK_Down:	   key = Input::Keyboard::KeyDown;    break;

		case XK_Home:	   key = Input::Keyboard::KeyHome;    break;
		case XK_End:	   key = Input::Keyboard::KeyEnd;     break;
		case XK_Insert:	   key = Input::Keyboard::KeyInsert;  break;
		case XK_Delete:	   key = Input::Keyboard::KeyDelete;  break;
		case XK_Prior:	   key = Input::Keyboard::KeyPrior;   break;
		case XK_Next:	   key = Input::Keyboard::KeyNext;    break;

		case XK_Return:	   key = Input::Keyboard::KeyReturn;  break;
		case XK_BackSpace: key = Input::Keyboard::KeyBack;    break;
		case XK_Tab:	   key = Input::Keyboard::KeyTab;     break;

		case XK_space:	   key = Input::Keyboard::KeySpace;   break;

		case XK_Shift_L:
		case XK_Shift_R:   key = Input::Keyboard::KeyShift;   break;

		case XK_Control_L:
		case XK_Control_R: key = Input::Keyboard::KeyControl; break;

		case XK_Alt_L:
		case XK_Alt_R:	   key = Input::Keyboard::KeyAlt;     break;

		case XK_Escape:	   key = Input::Keyboard::KeyEscape;  break;

		case XK_F1:	   key = Input::Keyboard::KeyF1;      break;
		case XK_F2:	   key = Input::Keyboard::KeyF2;      break;
		case XK_F3:	   key = Input::Keyboard::KeyF3;      break;
		case XK_F4:	   key = Input::Keyboard::KeyF4;      break;
		case XK_F5:	   key = Input::Keyboard::KeyF5;      break;
		case XK_F6:	   key = Input::Keyboard::KeyF6;      break;
		case XK_F7:	   key = Input::Keyboard::KeyF7;      break;
		case XK_F8:	   key = Input::Keyboard::KeyF8;      break;
		case XK_F9:	   key = Input::Keyboard::KeyF9;      break;
		case XK_F10:	   key = Input::Keyboard::KeyF10;     break;
		case XK_F11:	   key = Input::Keyboard::KeyF11;     break;
		case XK_F12:	   key = Input::Keyboard::KeyF12;     break;
		case XK_F13:	   key = Input::Keyboard::KeyF13;     break;
		case XK_F14:	   key = Input::Keyboard::KeyF14;     break;
		case XK_F15:	   key = Input::Keyboard::KeyF15;     break;
		case XK_F16:	   key = Input::Keyboard::KeyF16;     break;
		case XK_F17:	   key = Input::Keyboard::KeyF17;     break;
		case XK_F18:	   key = Input::Keyboard::KeyF18;     break;
		case XK_F19:	   key = Input::Keyboard::KeyF19;     break;
		case XK_F20:	   key = Input::Keyboard::KeyF20;     break;
		case XK_F21:	   key = Input::Keyboard::KeyF21;     break;
		case XK_F22:	   key = Input::Keyboard::KeyF22;     break;
		case XK_F23:	   key = Input::Keyboard::KeyF23;     break;
		case XK_F24:	   key = Input::Keyboard::KeyF24;     break;
	}

	if	(keySym >= '0' && keySym <= '9') key = (Input::Keyboard::Key)  keySym;
	else if	(keySym >= 'A' && keySym <= 'Z') key = (Input::Keyboard::Key)  keySym;
	else if	(keySym >= 'a' && keySym <= 'z') key = (Input::Keyboard::Key) (keySym + ('A' - 'a'));

	return key;
}

S::Int S::GUI::WindowXLib::ProcessSystemMessages(XEvent *e)
{
	static Int	 focusWndID = -1;

	/* Process system events not relevant
	 * to portable Window implementation.
	 */
	switch (e->type)
	{
		case CreateNotify:
			onCreate.Emit();

			break;
		case DestroyNotify:
			onDestroy.Emit();

			oldwnd = NIL;

			break;
		case MapNotify:
			/* Set internal focus window.
			 */
			focusWndID = id;

			onEvent.Call(SM_GETFOCUS, 0, 0);

			break;
		case UnmapNotify:
			/* Clear internal focus window.
			 */
			if (focusWndID == id) focusWndID = -1;

			break;
		case SelectionRequest:
			if (selection != NIL)
			{
				XEvent	 respond;

				if (e->xselectionrequest.target == XA_STRING)
				{
					XChangeProperty(display, e->xselectionrequest.requestor, e->xselectionrequest.property, XA_STRING, 8, PropModeReplace, (unsigned char *) (char *) selection, selection.Length());

					respond.xselection.property = e->xselectionrequest.property;
				}
				else if (e->xselectionrequest.target == XA_UTF8_STRING(display))
				{
					XChangeProperty(display, e->xselectionrequest.requestor, e->xselectionrequest.property, XA_UTF8_STRING(display), 8, PropModeReplace, (unsigned char *) selection.ConvertTo("UTF-8"), strlen(selection.ConvertTo("UTF-8")));

					respond.xselection.property = e->xselectionrequest.property;
				}
				else
				{
					/* Strings only please!
					 */
					respond.xselection.property = None;
				}

				respond.xselection.type	     = SelectionNotify;
				respond.xselection.display   = e->xselectionrequest.display;
				respond.xselection.requestor = e->xselectionrequest.requestor;
				respond.xselection.selection = e->xselectionrequest.selection;
				respond.xselection.target    = e->xselectionrequest.target;
				respond.xselection.time	     = e->xselectionrequest.time;

				XSendEvent(display, e->xselectionrequest.requestor, 0, 0, &respond);
				XFlush(display);
			}

			break;
		case SelectionClear:
			selection = NIL;

			break;
	}

	/* Convert Xlib events to smooth messages.
	 */
	switch (e->type)
	{
		/* Mouse events:
		 */
		case MotionNotify:
			Input::Pointer::UpdatePosition(Window::GetWindow((Void *) e->xmotion.window), e->xmotion.x_root, e->xmotion.y_root);

			onEvent.Call(SM_MOUSEMOVE, 0, 0);

			break;
		case EnterNotify:
			Input::Pointer::UpdatePosition(Window::GetWindow((Void *) e->xcrossing.window), e->xcrossing.x_root, e->xcrossing.y_root);

			onEvent.Call(SM_MOUSEMOVE, 0, 0);

			break;
		case LeaveNotify:
			Input::Pointer::UpdatePosition(NIL, e->xcrossing.x_root, e->xcrossing.y_root);

			onEvent.Call(SM_MOUSEMOVE, 0, 0);

			break;
		case ButtonPress:
			/* Update pointer position in Input::Pointer.
			 */
			Input::Pointer::UpdatePosition(Window::GetWindow((Void *) e->xbutton.window), e->xbutton.x_root, e->xbutton.y_root);

			/* Reject button messages if we do not have the focus and
			 * we are not an utility window.
			 *
			 * This usually means that some modal window is blocking.
			 */
			if (!(flags & WF_TOPMOST && flags & WF_NOTASKBUTTON && flags & WF_THINBORDER))
			{
				::Window	 focusWnd = None;
				int		 revertTo = RevertToNone;

				if (XGetInputFocus(display, &focusWnd, &revertTo) == 0)
				{
					if (focusWnd != wnd) break;
				}
			}

			/* Pass message to smooth window.
			 */
			{
				static XButtonEvent	 prevButtonEvent;
				static Bool		 prevButtonEventInitialized = False;

				if (!prevButtonEventInitialized)
				{
					prevButtonEvent.button = 0;
					prevButtonEvent.time   = 0;
					prevButtonEvent.x      = 0;
					prevButtonEvent.y      = 0;

					prevButtonEventInitialized = True;
				}

				if ((e->xbutton.button == Button1 || e->xbutton.button == Button3) &&
				     e->xbutton.button == prevButtonEvent.button		   &&
				     e->xbutton.time   <= prevButtonEvent.time + 500		   &&
				     Math::Abs(e->xbutton.x - prevButtonEvent.x) <= 5		   &&
				     Math::Abs(e->xbutton.y - prevButtonEvent.y) <= 5)
				{
					if	(e->xbutton.button == Button1) onEvent.Call(SM_LBUTTONDBLCLK, 0, 0);
					else if (e->xbutton.button == Button3) onEvent.Call(SM_RBUTTONDBLCLK, 0, 0);
				}
				else
				{
					if	(e->xbutton.button == Button1) onEvent.Call(SM_LBUTTONDOWN, 0, 0);
					else if (e->xbutton.button == Button3) onEvent.Call(SM_RBUTTONDOWN, 0, 0);
					else if (e->xbutton.button == Button4) onEvent.Call(SM_MOUSEWHEEL, 120, 0);
					else if (e->xbutton.button == Button5) onEvent.Call(SM_MOUSEWHEEL, -120, 0);
				}

				prevButtonEvent = e->xbutton;
			}

			/* Grab the keyboard focus if we don't have it already.
			 */
			if (wnd != NIL && focusWndID != id && e->xbutton.button <= 3)
			{
				WindowXLib	*focusWnd = windowBackends.Get(focusWndID);

				if (focusWnd != NIL) focusWnd->onEvent.Call(SM_LOSEFOCUS, Window::GetWindow((Void *) wnd)->GetHandle(), 0);

				focusWndID = id;

				onEvent.Call(SM_GETFOCUS, 0, 0);
			}

			/* Send an event to update widget under cursor if necessary.
			 */
			onEvent.Call(SM_MOUSEMOVE, 0, 0);

			break;
		case ButtonRelease:
			/* Update pointer position in Input::Pointer.
			 */
			Input::Pointer::UpdatePosition(Window::GetWindow((Void *) e->xbutton.window), e->xbutton.x_root, e->xbutton.y_root);

			/* Pass message to smooth window.
			 */
			if	(e->xbutton.button == Button1) onEvent.Call(SM_LBUTTONUP, 0, 0);
			else if (e->xbutton.button == Button3) onEvent.Call(SM_RBUTTONUP, 0, 0);

			break;

		/* Keyboard events:
		 */
		case KeymapNotify:
			XRefreshKeyboardMapping(&e->xmapping);

			break;
		case KeyPress:
			Input::Keyboard::UpdateKeyState(ConvertKey(XKeycodeToKeysym(display, e->xkey.keycode, 0)), True);

			onEvent.Call(SM_KEYDOWN, ConvertKey(XKeycodeToKeysym(display, e->xkey.keycode, 0)), 0);

			/* Convert keyboard event to input string and
			 * call SM_CHAR event for each character.
			 */
			{
				Status	 status;
				char	 text[32];
				Int	 numChars = Xutf8LookupString(ic, &e->xkey, text, 32, NIL, &status);

				text[numChars] = 0;

				if (status & XLookupChars)
				{
					String	 string;

					string.ImportFrom("UTF-8", text);

					for (Int i = 0; i < string.Length(); i++) onEvent.Call(SM_CHAR, string[i], 0);
				}
			}

			break;
		case KeyRelease:
			Input::Keyboard::UpdateKeyState(ConvertKey(XKeycodeToKeysym(display, e->xkey.keycode, 0)), False);

			onEvent.Call(SM_KEYUP, ConvertKey(XKeycodeToKeysym(display, e->xkey.keycode, 0)), 0);

			break;

		/* Paint events:
		 */
		case Expose:
			/* Fill update rect.
			 */
			if (updateRect == Rect())
			{
				updateRect.left	  = e->xexpose.x;
				updateRect.top	  = e->xexpose.y;
				updateRect.right  = e->xexpose.x + e->xexpose.width;
				updateRect.bottom = e->xexpose.y + e->xexpose.height;
			}
			else
			{
				updateRect.left	  = Math::Min(updateRect.left, e->xexpose.x);
				updateRect.top	  = Math::Min(updateRect.top, e->xexpose.y);
				updateRect.right  = Math::Max(updateRect.right, e->xexpose.x + e->xexpose.width);
				updateRect.bottom = Math::Max(updateRect.bottom, e->xexpose.y + e->xexpose.height);
			}

			/* Emit paint event if this was the
			 * last queued expose notification.
			 */
			if (e->xexpose.count == 0)
			{
				onEvent.Call(SM_PAINT, 0, 0);

				updateRect = Rect();
			}

			break;

		/* Window state change events:
		 */
		case ConfigureNotify:
			/* Get screen coordinates for window.
			 */
			{
				::Window	 child = 0;

				XTranslateCoordinates(display, wnd, RootWindow(display, DefaultScreen(display)), 0, 0, &e->xconfigure.x, &e->xconfigure.y, &child);
			}

			/* Emit window metrics and paint events.
			 */
			{
				static Size	 prevSize;

				pos  = Point(e->xconfigure.x, e->xconfigure.y);
				size = Size(e->xconfigure.width, e->xconfigure.height);

				onEvent.Call(SM_WINDOWMETRICS, ((e->xconfigure.x + 32768) << 16) | (e->xconfigure.y + 32768), ((e->xconfigure.width + 32768) << 16) | (e->xconfigure.height + 32768));

				if (size != prevSize)
				{
					updateRect.left	  = Math::Min(updateRect.left, 0);
					updateRect.top	  = Math::Min(updateRect.top, 0);
					updateRect.right  = Math::Max(updateRect.right, e->xconfigure.width);
					updateRect.bottom = Math::Max(updateRect.bottom, e->xconfigure.height);

					onEvent.Call(SM_PAINT, 0, 0);

					updateRect = Rect();
				}

				prevSize = size;
			}

			break;
		case FocusIn:
			focusWndID = id;

			onEvent.Call(SM_GETFOCUS, 0, 0);

			break;
		case FocusOut:
			Input::Keyboard::ResetKeyState();

			/* Get the window that now has the focus.
			 */
			{
				::Window	 xFocusWnd;
				int		 revertTo;

				XGetInputFocus(display, &xFocusWnd, &revertTo);

				Window		*focusWnd = Window::GetWindow((Void *) xFocusWnd);

				onEvent.Call(SM_LOSEFOCUS, focusWnd != NIL ? focusWnd->GetHandle() : -1, 0);
			}

			break;

		/* Window manager events:
		 */
		case ClientMessage:
			if (e->xclient.message_type == XInternAtom(display, "WM_PROTOCOLS", False))
			{
				if ((Atom) e->xclient.data.l[0] == XInternAtom(display, "WM_DELETE_WINDOW", False))
				{
					if (doClose.Call()) Close();
				}
				else if ((Atom) e->xclient.data.l[0] == XInternAtom(display, "_NET_WM_PING", False))
				{
					e->xclient.window = RootWindow(display, DefaultScreen(display));

					XSendEvent(display, e->xclient.window, 0, SubstructureNotifyMask | SubstructureRedirectMask, e);
					XFlush(display);
				}
			}

			break;
	}

	return Success();
}

S::Int S::GUI::WindowXLib::Open(const String &title, const Point &pos, const Size &size, Int iFlags)
{
	flags = iFlags;

	XSetWindowAttributes	 attributes;

	attributes.background_pixel = Setup::BackgroundColor;
	attributes.bit_gravity	    = NorthWestGravity;

	if ((flags & WF_TOPMOST) && (flags & WF_NOTASKBUTTON) && (flags & WF_THINBORDER))
	{
		attributes.save_under	     = True;
		attributes.override_redirect = True;
	}
	else
	{
		attributes.save_under	     = False;
		attributes.override_redirect = False;
	}

	wnd = XCreateWindow(display, RootWindow(display, 0), pos.x, pos.y, size.cx + sizeModifier.cx, size.cy + sizeModifier.cy, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWBitGravity | CWSaveUnder | CWOverrideRedirect, &attributes);

	if (wnd != NIL)
	{
		/* Create input context.
		 */
		ic = XCreateIC(im, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, wnd, NULL);

		/* Get mask of filter events for IC.
		 */
		long	 filterEvents = 0;

		XGetICValues(ic, XNFilterEvents, &filterEvents, NULL);

		/* Select event types we want to receive.
		 */
		XSelectInput(display, wnd, ExposureMask | FocusChangeMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | StructureNotifyMask | filterEvents);

		/* Opt in to the WM_DELETE_WINDOW and _NET_WM_PING protocols.
		 */
		Atom	 protocols[2] = { XInternAtom(display, "WM_DELETE_WINDOW", False),
					  XInternAtom(display, "_NET_WM_PING", False) };

		XSetWMProtocols(display, wnd, protocols, 2);

		/* Process the CreateNotify event again, as GetWindowBackend
		 * cannot find the correct backend until wnd is set.
		 */
		XEvent	 e;

		e.type = CreateNotify;

		ProcessSystemMessages(&e);

		/* Create drawing surface.
		 */
		if ((flags & WF_THINBORDER) || (flags & WF_NORESIZE)) drawSurface = new Surface((Void *) wnd, size);
		else						      drawSurface = new Surface((Void *) wnd);

		drawSurface->SetSize(size);

		/* Set window title.
		 */
		SetTitle(title);

		/* Set minimum and maximum size.
		 */
		if (flags & WF_NORESIZE)
		{
			minSize = size + sizeModifier;
			maxSize = size + sizeModifier;
		}

		UpdateWMNormalHints();

		/* Configure window type.
		 */
		Atom	 windowTypeAtom	       = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
		Atom	 windowStateAtom       = XInternAtom(display, "_NET_WM_STATE", False);

		Atom	 windowTypeNormalAtom  = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
		Atom	 windowTypeDialogAtom  = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
		Atom	 windowTypeUtilityAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", False);

		Atom	 windowStateModalAtom  = XInternAtom(display, "_NET_WM_STATE_MODAL", False);

		if ((flags & WF_TOPMOST) && (flags & WF_NOTASKBUTTON) && (flags & WF_THINBORDER))
		{
			XChangeProperty(display, wnd, windowTypeAtom, XA_ATOM, 32, PropModeReplace, (unsigned char *) &windowTypeUtilityAtom, 1);
		}
		else if (flags & WF_MODAL)
		{
			XChangeProperty(display, wnd, windowTypeAtom, XA_ATOM, 32, PropModeReplace, (unsigned char *) &windowTypeDialogAtom, 1);
			XChangeProperty(display, wnd, windowStateAtom, XA_ATOM, 32, PropModeReplace, (unsigned char *) &windowStateModalAtom, 1);
		}
		else
		{
			XChangeProperty(display, wnd, windowTypeAtom, XA_ATOM, 32, PropModeReplace, (unsigned char *) &windowTypeNormalAtom, 1);
		}

		/* Set transient hint.
		 */
		Widget	*container = Window::GetWindow((Void *) wnd)->GetContainer();

		if (container != NIL)
		{
			Window		*parent = container->GetContainerWindow();
			WindowXLib	*leader = FindLeaderWindow();

			if	(parent != NIL) XSetTransientForHint(display, wnd, (::Window) parent->GetSystemWindow());
			else if (leader != NIL) XSetTransientForHint(display, wnd, leader->wnd);
		}

		return Success();
	}

	return Error();
}

S::Int S::GUI::WindowXLib::Close()
{
	if (wnd == NIL) return Success();

	/* Delete surface.
	 */
	if (drawSurface != nullSurface) delete drawSurface;

	drawSurface = nullSurface;

	/* Destroy input context and window.
	 */
	XDestroyIC(ic);
	XDestroyWindow(display, wnd);

	oldwnd	= wnd;
	wnd	= NIL;

	ic	= NIL;

	return Success();
}

S::Int S::GUI::WindowXLib::RequestClose()
{
	if (doClose.Call()) return Close();

	return Success();
}

S::GUI::WindowXLib *S::GUI::WindowXLib::FindLeaderWindow()
{
	/* The leader window is the newest non topmost window.
	 */
	for (Int i = windowBackends.Length() - 1; i >= 0; i--)
	{
		WindowXLib	*backend = windowBackends.GetNth(i);

		if (backend != this && backend->wnd != NIL && !(backend->flags & WF_TOPMOST)) return backend;
	}

	return NIL;
}

S::Int S::GUI::WindowXLib::SetTitle(const String &nTitle)
{
	XTextProperty	 titleProp;
	XTextProperty	 titlePropUTF8;

	const char	*title = nTitle.ConvertTo("ISO-8859-1");
	const char	*titleUTF8 = nTitle.ConvertTo("UTF-8");

	/* Set ISO encoded window name.
	 */
	if (XStringListToTextProperty((char **) &title, 1, &titleProp) != 0)
	{
		XSetWMName(display, wnd, &titleProp);
		XSetWMIconName(display, wnd, &titleProp);

		XFree(titleProp.value);
	}

	/* Set UTF-8 encoded window name.
	 */
	if (Xutf8TextListToTextProperty(display, (char **) &titleUTF8, 1, XUTF8StringStyle, &titlePropUTF8) == 0)
	{
		XSetTextProperty(display, wnd, &titlePropUTF8, XInternAtom(display, "_NET_WM_NAME", False));
		XSetTextProperty(display, wnd, &titlePropUTF8, XInternAtom(display, "_NET_WM_ICON_NAME", False));

		XFree(titlePropUTF8.value);
	}

	return Success();
}

S::Int S::GUI::WindowXLib::SetMinimumSize(const Size &nMinSize)
{
	minSize = nMinSize + sizeModifier;

	UpdateWMNormalHints();

	return Success();
}

S::Int S::GUI::WindowXLib::SetMaximumSize(const Size &nMaxSize)
{
	maxSize = nMaxSize + sizeModifier;

	UpdateWMNormalHints();

	return Success();
}

S::Int S::GUI::WindowXLib::Show()
{
	if (wnd == NIL) return Success();

	XMapRaised(display, wnd);
	XFlush(display);

	return Success();
}

S::Int S::GUI::WindowXLib::Hide()
{
	if (wnd == NIL) return Success();

	XUnmapWindow(display, wnd);
	XFlush(display);

	return Success();
}

S::Int S::GUI::WindowXLib::SetMetrics(const Point &nPos, const Size &nSize)
{
	if (nPos == pos && nSize == size) return Success();

	XMoveResizeWindow(display, wnd, nPos.x, nPos.y, nSize.cx + sizeModifier.cx, nSize.cy + sizeModifier.cy);
	XFlush(display);

	drawSurface->SetSize(nSize);

	return Success();
}

S::Int S::GUI::WindowXLib::Raise()
{
	if (wnd == NIL) return Success();

	/* FixMe: Are there cases where we need to do anything here?
	 */
	XFlush(display);

	return Success();
}
