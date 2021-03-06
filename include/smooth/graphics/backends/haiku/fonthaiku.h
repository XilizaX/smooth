 /* The smooth Class Library
  * Copyright (C) 1998-2017 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FONTHAIKU
#define H_OBJSMOOTH_FONTHAIKU

namespace smooth
{
	namespace GUI
	{
		class FontHaiku;
	};
};

#include "../fontbackend.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 FONT_HAIKU	= 6;

		class FontHaiku : public FontBackend
		{
			public:
				static Int	 Initialize();

						 FontHaiku(const String &, Short, Short, Short, const Color &);
						~FontHaiku();

				Size		 GetTextSize(const String &) const;
		};
	};
};

#endif
