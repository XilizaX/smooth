 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_SEMAPHORE_
#define _H_OBJSMOOTH_SEMAPHORE_

namespace smooth
{
	class Semaphore;
};

#include "object.h"

namespace smooth
{
	class SMOOTHAPI Semaphore : public Object
	{
		private:
			LiSASemaphore	*semaphore;
		public:
					 Semaphore();
					~Semaphore();

			Int		 Wait();
			Int		 Release();
	};

	SMOOTHVAR Int OBJ_SEMAPHORE;
};

#endif
