 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/graphics/backends/cocoa/fontcocoa.h>
#include <smooth/graphics/surface.h>

S::GUI::FontBackend *CreateFontCocoa(const S::String &iFontName, S::Short iFontSize, S::Short iFontWeight, S::Short iFontStyle, const S::GUI::Color &iFontColor)
{
	return new S::GUI::FontCocoa(iFontName, iFontSize, iFontWeight, iFontStyle, iFontColor);
}

S::Int	 fontCocoaTmp = S::GUI::FontBackend::SetBackend(&CreateFontCocoa);

S::GUI::FontCocoa::FontCocoa(const String &iFontName, Short iFontSize, Short iFontWeight, Short iFontStyle, const Color &iFontColor) : FontBackend(iFontName, iFontSize, iFontWeight, iFontStyle, iFontColor)
{
	type = FONT_COCOA;
}

S::GUI::FontCocoa::~FontCocoa()
{
}

S::GUI::Size S::GUI::FontCocoa::GetTextSize(const String &iText, Bool scaled) const
{
	if (iText == NIL) return Size();

	Float	 dpi = Surface().GetSurfaceDPI();

	NSFont			*font	    = [[NSFontManager sharedFontManager] fontWithFamily: [NSString stringWithUTF8String: fontName.ConvertTo("UTF-8")]
											 traits: (fontStyle & Font::Italic ? NSItalicFontMask : 0) | (fontWeight >= Font::Bold ? NSBoldFontMask : 0)
											 weight: 5
											   size: fontSize * dpi / 72.0];
	NSDictionary		*attributes = [NSDictionary dictionaryWithObject: font
									  forKey: NSFontAttributeName];
	NSAttributedString	*string	    = [[NSAttributedString alloc] initWithString: [NSString stringWithUTF8String: iText.ConvertTo("UTF-8")]
									      attributes: attributes];

	NSSize	 size = [string size];

	[string release];

	if (scaled || Math::Abs(dpi - 96.0) < 0.1) return Size(size.width, size.height - 2);
	else					   return Size(size.width, size.height - 2) * 96.0 / dpi;
}