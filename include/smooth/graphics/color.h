 /* The smooth Class Library
  * Copyright (C) 1998-2004 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_COLOR_
#define _H_OBJSMOOTH_COLOR_

namespace smooth
{
	namespace GUI
	{
		class Color;
	};
};

#include "../definitions.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 RGBA	= 0;	// Red, Green, Blue, Alpha
		const Int	 HSV	= 1;	// Hue, Saturation, Value
		const Int	 YUV	= 2;	// Value, Blue, Red
		const Int	 CMY	= 3;	// Cyan, Magenta, Yellow (subtractive)
		const Int	 CMYK	= 4;	// Cyan, Magenta, Yellow, Black
		const Int	 GRAY	= 5;	// Black

		class SMOOTHAPI Color
		{
			protected:
				Int		 colorSpace;
				Long		 color;
			public:
						 Color();
						 Color(const Color &);
						 Color(Long, Int = RGBA);
						 Color(Int, Int, Int, Int = RGBA);

				Int		 GetRed()					{ return color & 255; }
				Int		 GetGreen()					{ return (color >> 8) & 255; }
				Int		 GetBlue()					{ return (color >> 16) & 255; }
				Int		 GetAlpha()					{ return (color >> 24) & 255; }

				Void		 SetColor(Long nColor, Int nColorSpace = RGBA)	{ color = nColor; colorSpace = nColorSpace; }
				Void		 SetColor(Int r, Int g, Int b, Int c = RGBA)	{ color = r + g * 256 + b * 65536; colorSpace = c; }

				Color		 ConvertTo(Int);
				Color		 Grayscale()					{ return Color(ConvertTo(GRAY), ConvertTo(GRAY), ConvertTo(GRAY)); }

				Color		 Average(Color color2)				{ return Color((GetRed() + color2.GetRed()) / 2, (GetGreen() + color2.GetGreen()) / 2, (GetBlue() + color2.GetBlue()) / 2, (GetAlpha() + color2.GetAlpha()) / 2); }

				Color		 Downsample(Int);
				Color		 Upsample(Int);

				Color &operator	 =(const Long nColor)				{ SetColor(nColor); return *this; }
				operator	 Long() const					{ return color; }
		};
	};
};

#endif