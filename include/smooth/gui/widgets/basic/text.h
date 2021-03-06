 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TEXT
#define H_OBJSMOOTH_TEXT

namespace smooth
{
	namespace GUI
	{
		class Text;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Text : public Widget
		{
			protected:
				Size			 align;
			public:
				static const Short	 classID;

							 Text(const String &, const Point &, const Size & = Size(0, 0));
				virtual			~Text();

				virtual Int		 Paint(Int);
			accessors:
				Int			 SetText(const String &);

				Int			 SetFont(const Font &);
		};
	};
};

#endif
