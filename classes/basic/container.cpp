 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/container.h>
#include <smooth/surface.h>
#include <smooth/object.h>

#ifdef __WIN32__
__declspec (dllexport)
#endif

S::Int	 S::OBJ_CONTAINER = S::Object::RequestObjectID();

S::Container::Container()
{
	self			= NIL;
	nOfObjects		= 0;
	containerType.container	= this;
	containerType		= OBJ_CONTAINER;

	nullSurface = new GUI::Surface();
	drawSurface = nullSurface;
}

S::Container::~Container()
{
	for (Int i = 0; i < nOfObjects; i++)
	{
		if (UnregisterObject(assocObjects.GetFirstEntry()) == Success) nOfObjects++;
	}

	assocObjects.DeleteAll();

	delete nullSurface;
}

S::Int S::Container::RegisterObject(Object *object)
{
	if (object == NIL) return Error;

	if (containerType == &object->possibleContainers)
	{
		if (!object->IsRegistered())
		{
			assocObjects.AddEntry(object, object->handle);
			nOfObjects++;

			object->SetContainer(this);
			object->SetRegisteredFlag();

			return Success;
		}
	}

	return Error;
}

S::Int S::Container::UnregisterObject(Object *object)
{
	if (object == NIL) return Error;

	if (containerType == &object->possibleContainers)
	{
		if (object->IsRegistered())
		{
			if (assocObjects.DeleteEntry(object->handle) == True)
			{
				nOfObjects--;

				object->UnsetRegisteredFlag();
				object->SetContainer(NIL);

				return Success;
			}
		}
	}

	return Error;
}

S::Int S::Container::GetNOfObjects()
{
	return nOfObjects;
}

S::Object *S::Container::RequestObject(Int objectHandle)
{
	return assocObjects.GetEntry(objectHandle);
}

S::Bool S::Container::IsContainerCompatible(Int objType)
{
	if (objType == OBJ_CONTAINER)	return True;
	else				return False;
}

S::ContainerType S::Container::GetContainerType()
{
	return containerType;
}

S::Object *S::Container::GetContainerObject()
{
	return self;
}

S::GUI::Surface *S::Container::GetDrawSurface()
{
	return drawSurface;
}
