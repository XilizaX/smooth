 /* The smooth Class Library
  * Copyright (C) 1998-2019 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DRIVER_WIN32
#define H_OBJSMOOTH_DRIVER_WIN32

#include <windows.h>

namespace smooth
{
	namespace IO
	{
		class DriverWin32;
	};
};

#include "../../definitions.h"
#include "../driver.h"

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI DriverWin32 : public Driver
		{
			private:
				HANDLE	 stream;
				Bool	 closeStream;
			public:
					 DriverWin32(const String &, Int);
					 DriverWin32(HANDLE);
					~DriverWin32();

				Int	 ReadData(UnsignedByte *, Int);
				Int	 WriteData(const UnsignedByte *, Int);

				Int64	 Seek(Int64);

				Bool	 Truncate(Int64);
				Bool	 Flush();

				Bool	 Close();

				Int64	 GetSize() const;
				Int64	 GetPos() const;
		};
	};
};

#endif
