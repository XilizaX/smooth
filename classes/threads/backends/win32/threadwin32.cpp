 /* The smooth Class Library
  * Copyright (C) 1998-2016 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/threads/backends/win32/threadwin32.h>

S::Threads::ThreadBackend *CreateThreadWin32(S::Void *iThread)
{
	return new S::Threads::ThreadWin32(iThread);
}

S::Int	 threadWin32Tmp = S::Threads::ThreadBackend::SetBackend(&CreateThreadWin32);

S::Threads::ThreadWin32::ThreadWin32(Void *iThread)
{
	type	 = THREAD_WIN32;

	thread	 = NIL;
	threadID = -1;
	myThread = False;

	if (iThread != NIL) thread = (HANDLE) iThread;
}

S::Threads::ThreadWin32::~ThreadWin32()
{
	if (myThread) Stop();
}

S::Void *S::Threads::ThreadWin32::GetSystemThread() const
{
	return (Void *) threadID;
}

S::Int S::Threads::ThreadWin32::GetThreadID() const
{
	return threadID;
}

S::Bool S::Threads::ThreadWin32::IsCurrentThread() const
{
	return (threadID == (Int) GetCurrentThreadId());
}

S::Int S::Threads::ThreadWin32::Start(Void (*threadProc)(Void *), Void *threadParam)
{
	Stop();

	info.threadProc	 = threadProc;
	info.threadParam = threadParam;

	thread	 = CreateThread(NULL, 0, Caller, &info, 0, (DWORD *) &threadID);
	myThread = True;

	return Success();
}

S::Int S::Threads::ThreadWin32::Stop()
{
	if (thread == NIL) return Error();

	HANDLE	 self	 = thread;
	Bool	 running = IsRunning();

	thread = NIL;

	if (running)  TerminateThread(self, 0);
	if (myThread) CloseHandle(self);

	return Success();
}

S::Int S::Threads::ThreadWin32::Wait()
{
	if (thread == NIL) return Error();

	Bool	 running = IsRunning();

	if (running)  WaitForSingleObject(thread, INFINITE);
	if (myThread) CloseHandle(thread);

	thread = NIL;

	return Success();
}

S::Bool S::Threads::ThreadWin32::IsRunning() const
{
	DWORD	 exitCode = 0;

	GetExitCodeThread(thread, &exitCode);

	return (exitCode == STILL_ACTIVE);
}

S::Void S::Threads::ThreadWin32::Exit()
{
	if (!IsCurrentThread()) return;

	CloseHandle(thread);

	thread = NIL;
}

DWORD WINAPI S::Threads::ThreadWin32::Caller(LPVOID param)
{
	ThreadInfo	*info = (ThreadInfo *) param;

	CoInitialize(NIL);

	info->threadProc(info->threadParam);

	CoUninitialize();

	return 0;
}
