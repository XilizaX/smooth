 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_XULBOX
#define H_OBJSMOOTH_XULBOX

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class Box;
		};

		class Node;
	};

	namespace GUI
	{
		class Layer;
	};
};

#include "widget.h"

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class Box : public Widget
			{
				private:
					GUI::Layer		*layer;

					Array<Widget *, Void *>	 widgets;

					Void			 CalculateChildMetrics();
				public:
								 Box(Node *);
					virtual			~Box();

					GUI::Widget		*GetWidget() const;
			};
		};
	};
};

#endif
