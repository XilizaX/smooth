 /* The smooth Class Library
  * Copyright (C) 1998-2019 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DRIVER_SOCKS5
#define H_OBJSMOOTH_DRIVER_SOCKS5

namespace smooth
{
	namespace IO
	{
		class DriverANSI;
	};
};

#include "../../definitions.h"
#include "../driver.h"

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI DriverSOCKS5 : public Driver
		{
			private:
				unsigned int	 stream;
				Bool		 closeStream;
			public:
						 DriverSOCKS5(const String &, Int, const String &, Int, const String & = NIL, const String & = NIL);
						~DriverSOCKS5();

				Int		 ReadData(UnsignedByte *, Int);
				Int		 WriteData(const UnsignedByte *, Int);

				Bool		 Close();
		};
	};
};

#endif
