 /* The smooth Class Library
  * Copyright (C) 1998-2019 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_OUTSTREAM
#define H_OBJSMOOTH_OUTSTREAM

namespace smooth
{
	namespace IO
	{
		class InStream;
		class OutStream;
		class Driver;
		class Filter;
	};
};

#include <stdio.h>

#include "../definitions.h"
#include "stream.h"

#define OutputNumberIntel OutputNumber

namespace smooth
{
	namespace IO
	{
		const Short	 OS_APPEND	= 0;
		const Short	 OS_REPLACE	= 1;

		class SMOOTHAPI OutStream : public Stream
		{
			friend class InStream;

			private:
				static Int	 defaultPackageSize;

				InStream	*inStream;

				Bool		 InitBitstream		();
				Bool		 CompleteBitstream	();

				Bool		 WriteData		();
			public:
						 OutStream		(Int, Driver *);
						 OutStream		(Int, const String &, Int = OS_APPEND);
						 OutStream		(Int, FILE *);
						 OutStream		(Int, Void *, Long);
						 OutStream		(Int, InStream *);
				virtual		~OutStream		();

				Bool		 OutputNumber		(Int64, Int);
				Bool		 OutputNumberRaw	(Int64, Int);

				Bool		 OutputBits		(Int64, Int);

				Bool		 OutputString		(const String &);
				Bool		 OutputLine		(const String & = NIL);
				Bool		 OutputData		(const Void *, Int);

				Bool		 SetPackageSize		(Int);

				Bool		 SetFilter		(Filter *);
				Bool		 RemoveFilter		();

				Bool		 Close			();

				Bool		 Seek			(Int64);
				Bool		 RelSeek		(Int64);

				Bool		 Truncate		(Int64);
				Bool		 Flush			();

		};
	};
};

#endif
