 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MDIWINDOW
#define H_OBJSMOOTH_MDIWINDOW

namespace smooth
{
	namespace GUI
	{
		namespace MDI
		{
			class Window;
		};
	};
};

#include "../window/window.h"

namespace smooth
{
	namespace GUI
	{
		namespace MDI
		{
			class SMOOTHAPI Window : public GUI::Window
			{
				public:
					static const Short	 classID;

								 Window(const String &, const Point &, const Size &);
					virtual			~Window();

					virtual Bool		 IsTypeCompatible(Short) const;
			};
		};
	};
};

#endif
