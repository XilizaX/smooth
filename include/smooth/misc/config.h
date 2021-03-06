 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CONFIGURATION
#define H_OBJSMOOTH_CONFIGURATION

namespace smooth
{
	class Configuration;
};

#include "string.h"
#include "../xml/document.h"

namespace smooth
{
	class SMOOTHAPI Configuration
	{
		private:
			String		 fileName;

			String		 activeConfig;

			XML::Document	*configFile;
			XML::Node	*ownRoot;

			XML::Node	*FindConfigurationNode(const String &) const;
			XML::Node	*FindSectionNode(const String &) const;
			XML::Node	*FindValueNode(const String &, const String &) const;
		public:
					 Configuration();
					 Configuration(const String &, Bool = True);
					 Configuration(const Configuration &);
					~Configuration();

			Int		 Open(const String & = "config.xml", Bool = True);
			Int		 Save();
			Int		 Close();

			Int		 AddConfiguration(const String &);
			Int		 RemoveConfiguration(const String &);

			Int		 GetNOfConfigurations() const;
			String		 GetNthConfigurationName(Int) const;

			Int		 SetActiveConfiguration(const String &);
			Int		 SetParentConfiguration(const String &);

			String		 GetConfigurationName() const;
			Int		 SetConfigurationName(const String &);

			Int		 GetIntValue(const String &, const String &, Int = 0);
			Int		 SetIntValue(const String &, const String &, Int);

			String		 GetStringValue(const String &, const String &, const String & = NIL);
			Int		 SetStringValue(const String &, const String &, const String &);
	};
};

#endif
