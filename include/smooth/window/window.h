 /* The smooth Class Library
  * Copyright (C) 1998-2004 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_WINDOW_
#define _H_OBJSMOOTH_WINDOW_

namespace smooth
{
	namespace GUI
	{
		class Window;
		class WindowBackend;
		class PopupMenu;
		class Layer;
	};

	namespace System
	{
		class Timer;
	};
};

#include "../widget.h"
#include "../container.h"
#include "../primitives/rect.h"
#include "../loop.h"
#include "../menu/menu.h"
#include "../graphics/bitmap.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 WF_MODAL		= 1;
		const Int	 WF_SYSTEMMODAL		= 2;
		const Int	 WF_TOPMOST		= 4;
		const Int	 WF_APPTOPMOST		= 8;
		const Int	 WF_NORESIZE		= 16;
		const Int	 WF_NOTASKBUTTON	= 32;
		const Int	 WF_DELAYPAINT		= 64;
		const Int	 WF_THINBORDER		= 128;

		const Int	 WO_SEPARATOR	= 1;
		const Int	 WO_NOSEPARATOR	= 2;

		class SMOOTHAPI Window : public Widget, public Container
		{
			protected:
				WindowBackend			*backend;

				Int				 stay;
				Bool				 maximized;

				Bool				 created;
				Bool				 destroyed;

				Bool				 firstPaint;

				Bitmap				 icon;

				Rect				 innerOffset;
				Rect				 updateRect;
				Rect				 timedUpdateRect;

				String				 defaultStatus;

				PopupMenu			*trackMenu;

				Layer				*mainLayer;
				System::Timer			*paintTimer;

				Bool				 Create();

				Void				 CalculateOffsets();

				Void				 PopupProc();
			public:
				static const Int		 classID;

				static Int			 nOfActiveWindows;
				Int				 value;

				Bool				 initshow;

								 Window(String = NIL, Void * = NIL);
								~Window();

				Int				 SetIcon(const Bitmap &);
				Bitmap				&GetIcon();

				Int				 SetMetrics(const Point &, const Size &);

				Int				 SetText(String);

				Layer				*GetMainLayer();

				Int				 SetStatusText(String);
				String				 GetStatusText();

				Int				 SetDefaultStatusText(String);
				Int				 RestoreDefaultStatusText();

				Rect				 GetWindowRect();
				Rect				 GetClientRect();
				Rect				 GetRestoredWindowRect();

				Rect				 GetUpdateRect();
				Int				 SetUpdateRect(Rect);

				Int				 SetMinimumSize(Size);
				Int				 SetMaximumSize(Size);

				Int				 Show();
				Int				 Hide();

				Int				 Minimize();

				Int				 Maximize();
				Int				 Restore();

				Bool				 IsMaximized();

				Int				 Stay();
				Int				 Close();

				Bool				 IsInUse();

				Int				 Paint(Int);
				Int				 Process(Int, Int, Int);

				Int				 MouseX();
				Int				 MouseY();

				Bool				 IsMouseOn(Rect);

				Surface				*GetDrawSurface();
				Void				*GetSystemWindow();

				static Window			*GetWindow(Void *);

				Int				 RegisterObject(Object *);
				Int				 UnregisterObject(Object *);
			signals:
				Signal0<Void>			 onCreate;

				Signal0<Void>			 onPaint;
				Signal0<Void>			 onResize;
				Signal0<Void>			 onPeek;
				Signal3<Void, Int, Int, Int>	 onEvent;
			callbacks:
				Callback2<Menu *, Int, Int>	 getTrackMenu;
				Callback0<Bool>			 doQuit;
			slots:
				Void				 PaintTimer();
		};
	};
};

#endif