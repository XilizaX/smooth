 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_BUTTON_
#define _H_OBJSMOOTH_BUTTON_

namespace smooth
{
	namespace GUI
	{
		class Button;
		class Tooltip;
	};

	class Timer;
};

#include "widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Button : public Widget
		{
			private:
				Tooltip		*tooltip;
				Timer		*tipTimer;

				Void		 ActivateTooltip();
			protected:
				HBITMAP		 bitmap;
				Size		 bmpSize;
			public:
						 Button(String, HBITMAP, Point, Size);
						~Button();

				virtual Int	 Paint(Int);
				Int		 Process(Int, Int, Int);
		};
	};

	SMOOTHVAR Int OBJ_BUTTON;
};

#endif
