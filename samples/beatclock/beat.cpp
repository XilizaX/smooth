 /* The SMOOTH Windowing Toolkit
  * Copyright (C) 1998-2002 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include "beat.h"

SMOOTHVoid SMOOTH::Main()
{
	BeatClock	*app = new BeatClock();

	SMOOTH::Loop();

	delete app;
}

BeatClock::BeatClock()
{
	SetText("BeatClock");

	actbeats = -1;
	actcbeats = -1;
	actccbeats = -1;
	wmpaint = true;
	timeformat = 0;
	centi = true;
	isalarm = false;
	alarmexec = false;
	alarmbeats = 0;
	alarmsecs = 0;
	alarmoption = 0;
	wpx = 16;
	wpy = 16;
	modechange = 3;
	timezone = 0;

	InputValues();

	wnd	= new SMOOTHWindow("BeatClock");
	title	= new SMOOTHTitlebar(false, false, true);
	menubar	= new SMOOTHMenubar();
	timer	= new SMOOTHTimer();
	dragcontrol = new SMOOTHDragControl();

	timer->SetProc(SMOOTHProc(BeatClock, this, PaintTime));

	menubar->AddEntry("Mode", NIL, SMOOTHProc(BeatClock, this, Mode));
	menubar->AddEntry("Options", NIL, SMOOTHProc(BeatClock, this, Options));
	menubar->AddEntry()->SetOrientation(OR_RIGHT);
	menubar->AddEntry("Info", NIL, SMOOTHProc(BeatClock, this, Info))->SetOrientation(OR_RIGHT);

	RegisterObject(wnd);

	wnd->RegisterObject(dragcontrol);
	wnd->RegisterObject(title);
	wnd->RegisterObject(menubar);
	wnd->RegisterObject(timer);

	wnd->SetIcon(SMOOTH::LoadImage("beat.pci", 0, NIL));
	wnd->SetPaintProc(SMOOTHProc(BeatClock, this, PaintAll));
	wnd->SetExStyle(WS_EX_TOPMOST|WS_EX_TOOLWINDOW);
	wnd->SetMetrics(SMOOTHPoint(wpx, wpy), SMOOTHSize(164 * SMOOTH::Setup::FontSize, 103 * SMOOTH::Setup::FontSize));
	wnd->SetMessageProc(SMOOTHMessageProc(BeatClock, this, MessageProc));
	wnd->Show();

	timer->Start(50);
}

BeatClock::~BeatClock()
{
	timer->Stop();

	RegisterValues();

	UnregisterObject(wnd);

	wnd->UnregisterObject(title);
	wnd->UnregisterObject(menubar);
	wnd->UnregisterObject(timer);
	wnd->UnregisterObject(dragcontrol);

	delete wnd;
	delete title;
	delete menubar;
	delete timer;
	delete dragcontrol;
}

SMOOTHVoid BeatClock::MessageProc(SMOOTHInt message, SMOOTHInt wParam, SMOOTHInt lParam)
{
	if (message == WM_WINDOWPOSCHANGED)
	{
		wpx = ((LPWINDOWPOS) lParam)->x;
		wpy = ((LPWINDOWPOS) lParam)->y;
	}
}

SMOOTHVoid BeatClock::Options()
{
	optionsdialog	= new SMOOTHWindow("BeatClock Options");
	mainlayer	= new SMOOTHLayer();
	display		= new SMOOTHLayer("Display");
	alarm		= new SMOOTHLayer("Alarm");
	misc		= new SMOOTHLayer("Misc");
	info		= new SMOOTHLayer("Info");
	optitle		= new SMOOTHTitlebar(false, false, true);

	SMOOTHPoint	 pos;
	SMOOTHSize	 size;

// Configuring "Display" Layer:

	pos.x = 7;
	pos.y = 11;
	size.cx = 230;
	size.cy = 65;

	display_group1 = new SMOOTHGroupBox("Time format", pos, size);

	pos.x = 245;
	size.cx = 121;

	display_group2 = new SMOOTHGroupBox("Timezone", pos, size);

	pos.x = 17;
	pos.y = 24;
	size.cx = 100;
	size.cy = 0;

	display_option1 = new SMOOTHOptionBox("Internet Beats", pos, size, &timeformat, 0, SMOOTHProc(BeatClock, this, OptionsBeats));

	pos.y = 49;

	display_option2 = new SMOOTHOptionBox("Standard (STF)", pos, size, &timeformat, 1, SMOOTHProc(BeatClock, this, OptionsSTF));

	pos.x = 126;
	pos.y = 24;

	if (timeformat == 0)	display_check1 = new SMOOTHCheckBox("Show centibeats", pos, size, &centi, SMOOTHProc(BeatClock, this, OptionsPaint));
	else			display_check1 = new SMOOTHCheckBox("Show seconds", pos, size, &centi, SMOOTHProc(BeatClock, this, OptionsPaint));

	pos.x = 255;

	display_option3 = new SMOOTHOptionBox("CET", pos, size, &timezone, 0, SMOOTHProc(BeatClock, this, OptionsPaint));

	pos.y = 49;

	display_option4 = new SMOOTHOptionBox("Local time", pos, size, &timezone, 1, SMOOTHProc(BeatClock, this, OptionsPaint));

	display->RegisterObject(display_group1);
	display->RegisterObject(display_group2);
	display->RegisterObject(display_option1);
	display->RegisterObject(display_option2);
	display->RegisterObject(display_option3);
	display->RegisterObject(display_option4);
	display->RegisterObject(display_check1);

// Configuring "Alarm" layer:

	pos.x = 7;
	pos.y = 7;

	alarm_check1 = new SMOOTHCheckBox("Enable alarm", pos, size, &isalarm, SMOOTHProc(BeatClock, this, toggleAlarmState));

	pos.y = 35;
	pos.x = 9;

	alarm_text1 = new SMOOTHText("Ring at:", pos);

	pos.x = 53;
	pos.y = 32;
	size.cx = 34;

	if (timeformat == 0)	alarmtext = SMOOTHString::IntToString(alarmbeats);
	else			alarmtext = convertSecondsToTimeString(alarmsecs);

	alarm_edit1 = new SMOOTHEditBox(alarmtext, pos, size, EDB_ALPHANUMERIC, 5, NULLPROC);

	pos.y = 35;
	pos.x = 94;

	if (timeformat == 0)	alarm_text2 = new SMOOTHText("Internet Beats", pos);
	else			alarm_text2 = new SMOOTHText("Hours/Minutes", pos);

	pos.y = 21;
	pos.x = 172;
	size.cx = 0;

	alarm_option1 = new SMOOTHOptionBox("only once", pos, size, &alarmoption, 0, NULLPROC);

	pos.y += 25;

	alarm_option2 = new SMOOTHOptionBox("every day", pos, size, &alarmoption, 1, NULLPROC);

	alarm->RegisterObject(alarm_check1);
	alarm->RegisterObject(alarm_text1);
	alarm->RegisterObject(alarm_text2);
	alarm->RegisterObject(alarm_edit1);
	alarm->RegisterObject(alarm_option1);
	alarm->RegisterObject(alarm_option2);

	toggleAlarmState();

// Configuring "Misc" layer:

	pos.x = 7;
	pos.y = 11;
	size.cx = 359;
	size.cy = 65;

	misc_group1 = new SMOOTHGroupBox("'Mode' button action", pos, size);

	pos.x = 17;
	pos.y = 24;
	size.cx = 164;
	size.cy = 0;

	if (timeformat == 0)	misc_option1 = new SMOOTHOptionBox("Show/hide centibeats", pos, size, &modechange, 0, NULLPROC);
	else			misc_option1 = new SMOOTHOptionBox("Show/hide seconds", pos, size, &modechange, 0, NULLPROC);

	pos.y = 49;

	misc_option2 = new SMOOTHOptionBox("Change time format", pos, size, &modechange, 1, NULLPROC);

	pos.x = 191;
	pos.y = 24;

	misc_option3 = new SMOOTHOptionBox("Change timezone", pos, size, &modechange, 2, NULLPROC);

	pos.y = 49;

	misc_option4 = new SMOOTHOptionBox("Change format and timezone", pos, size, &modechange, 3, NULLPROC);

	misc->RegisterObject(misc_group1);
	misc->RegisterObject(misc_option1);
	misc->RegisterObject(misc_option2);
	misc->RegisterObject(misc_option3);
	misc->RegisterObject(misc_option4);

// Configuring "Info" layer:

	pos.x = 6;
	pos.y = 5;

	info_text1 = new SMOOTHText("BeatClock version 2.0\n\nDeveloped by Robert Kausch 2000-2002\nGive it to all your friends!", pos);

	pos.x = 222;

	info_text2 = new SMOOTHText("\n\neMail: robert.kausch@gmx.net", pos);

	info->RegisterObject(info_text1);
	info->RegisterObject(info_text2);

// Configuring main layer:

	pos.x = 175;
	pos.y = 29;
	size.cx = 0;
	size.cy = 0;

	main_button1 = new SMOOTHButton("OK", NIL, pos, size, SMOOTHProc(BeatClock, this, OptionsOK));
	main_button1->SetOrientation(OR_LOWERRIGHT);

	pos.x = 87;

	main_button2 = new SMOOTHButton("Cancel", NIL, pos, size, SMOOTHProc(BeatClock, this, OptionsCancel));
	main_button2->SetOrientation(OR_LOWERRIGHT);

	pos.x = 7;
	pos.y = 7;
	size.cx = 376;
	size.cy = 105;

	main_reg1 = new SMOOTHTabRegister(pos, size);

	RegisterObject(optionsdialog);

	optionsdialog->RegisterObject(optitle);
	optionsdialog->RegisterObject(mainlayer);

	mainlayer->RegisterObject(main_button1);
	mainlayer->RegisterObject(main_button2);
	mainlayer->RegisterObject(main_reg1);

	main_reg1->RegisterObject(display);
	main_reg1->RegisterObject(misc);
	main_reg1->RegisterObject(alarm);
	main_reg1->RegisterObject(info);

	optionsdialog->SetIcon(SMOOTH::LoadImage("beat.pci", 0, NIL));
	optionsdialog->SetMetrics(SMOOTHPoint(100, 100), SMOOTHSize(397, 181));
	optionsdialog->SetKillProc(SMOOTHKillProc(BeatClock, this, OptionsKillProc));
	optionsdialog->SetStyle(SS_MODAL);

	oldtf = timeformat;
	oldct = centi;
	oldmc = modechange;
	oldtz = timezone;
	oldia = isalarm;
	oldab = alarmbeats;
	oldas = alarmsecs;
	oldat = alarmtext;
	oldao = alarmoption;

	optionsdialog->Stay();

	display->UnregisterObject(display_group1);
	display->UnregisterObject(display_group2);
	display->UnregisterObject(display_option1);
	display->UnregisterObject(display_option2);
	display->UnregisterObject(display_option3);
	display->UnregisterObject(display_option4);
	display->UnregisterObject(display_check1);

	alarm->UnregisterObject(alarm_check1);
	alarm->UnregisterObject(alarm_text1);
	alarm->UnregisterObject(alarm_text2);
	alarm->UnregisterObject(alarm_edit1);
	alarm->UnregisterObject(alarm_option1);
	alarm->UnregisterObject(alarm_option2);

	misc->UnregisterObject(misc_group1);
	misc->UnregisterObject(misc_option1);
	misc->UnregisterObject(misc_option2);
	misc->UnregisterObject(misc_option3);
	misc->UnregisterObject(misc_option4);

	info->UnregisterObject(info_text1);
	info->UnregisterObject(info_text2);

	mainlayer->UnregisterObject(main_button1);
	mainlayer->UnregisterObject(main_button2);
	mainlayer->UnregisterObject(main_reg1);

	main_reg1->UnregisterObject(display);
	main_reg1->UnregisterObject(alarm);
	main_reg1->UnregisterObject(misc);
	main_reg1->UnregisterObject(info);

	UnregisterObject(optionsdialog);

	optionsdialog->UnregisterObject(optitle);
	optionsdialog->UnregisterObject(mainlayer);

	delete optitle;
	delete optionsdialog;
	delete display;
	delete alarm;
	delete misc;
	delete info;
	delete mainlayer;
	delete main_button1;
	delete main_button2;
	delete main_reg1;
	delete info_text1;
	delete info_text2;
	delete alarm_check1;
	delete alarm_text1;
	delete alarm_text2;
	delete alarm_edit1;
	delete alarm_option1;
	delete alarm_option2;
	delete misc_group1;
	delete misc_option1;
	delete misc_option2;
	delete misc_option3;
	delete misc_option4;
	delete display_group1;
	delete display_group2;
	delete display_check1;
	delete display_option1;
	delete display_option2;
	delete display_option3;
	delete display_option4;
}

SMOOTHVoid BeatClock::OptionsOK()
{
	if (timeformat == 0)
	{
		alarmbeats = min(999, alarm_edit1->GetText().ToInt());
		alarmsecs = convertBeatsToSeconds(alarmbeats);
	}
	else
	{
		alarmsecs = min(86340, convertTimeStringToSeconds(alarm_edit1->GetText()));
		alarmbeats = convertSecondsToBeats(alarmsecs);
	}

	SMOOTH::CloseWindow(optionsdialog);
}

SMOOTHVoid BeatClock::OptionsCancel()
{
	SMOOTH::CloseWindow(optionsdialog);

	timeformat = oldtf;
	centi = oldct;
	modechange = oldmc;
	timezone = oldtz;
	isalarm = oldia;
	alarmbeats = oldab;
	alarmsecs = oldas;
	alarmtext = oldat;
	alarmoption = oldao;
}

SMOOTHVoid BeatClock::OptionsBeats()
{
	display_check1->SetText("Show centibeats");
	misc_option1->SetText("Show/hide centibeats");
	alarm_text2->SetText("Internet Beats");
	alarm_edit1->SetText(SMOOTHString::IntToString(min(999, convertSecondsToBeats(convertTimeStringToSeconds(alarm_edit1->GetText())))));
}

SMOOTHVoid BeatClock::OptionsSTF()
{
	display_check1->SetText("Show seconds");
	misc_option1->SetText("Show/hide seconds");
	alarm_text2->SetText("Hours/Minutes");
	alarm_edit1->SetText(convertSecondsToTimeString(min(86340, convertBeatsToSeconds(alarm_edit1->GetText().ToInt()))));
}

SMOOTHVoid BeatClock::OptionsPaint()
{
	PaintAll();
}

SMOOTHBool BeatClock::OptionsKillProc()
{
	return true;
}

SMOOTHVoid BeatClock::toggleAlarmState()
{
	if (isalarm)
	{
		alarm_edit1->Activate();
		alarm_option1->Activate();
		alarm_option2->Activate();
	}
	else
	{
		alarm_edit1->Deactivate();
		alarm_option1->Deactivate();
		alarm_option2->Deactivate();
	}
}

SMOOTHVoid BeatClock::Mode()
{
	if (modechange == 0)
	{
		switch (centi)
		{
			case true:
				centi = false;
				break;
			case false:
				centi = true;
				break;
		}
	}
	else if (modechange == 1)
	{
		switch (timeformat)
		{
			case 0:
				timeformat = 1;
				break;
			case 1:
				timeformat = 0;
				break;
		}
	}
	else if (modechange == 2)
	{
		switch (timezone)
		{
			case 0:
				timezone = 1;
				break;
			case 1:
				timezone = 0;
				break;
		}
	}
	else if (modechange == 3)
	{
		switch (timeformat)
		{
			case 0:
				timeformat = 1;
				break;
			case 1:
				timeformat = 0;
				break;
		}

		switch (timezone)
		{
			case 0:
				timezone = 1;
				break;
			case 1:
				timezone = 0;
				break;
		}
	}

	PaintAll();
}

SMOOTHVoid BeatClock::PaintTime()
{
	wmpaint = false;
	PaintAll();
	wmpaint = true;
}

SMOOTHVoid BeatClock::PaintAll()
{
	HDC			 dc = GetWindowDC(wnd->hwnd);
	RECT			 textrect;
	SYSTEMTIME		 time;
	TIME_ZONE_INFORMATION	 tzi;
	double			 mseconds = 0;
	int			 first;
	int			 day;
	int			 beats = 0;
	int			 cbeats = 0;
	int			 ccbeats = 0;
	SMOOTHString		 btext = "@";
	SMOOTHString		 btext2 = "@";
	HBRUSH			 brush = CreateSolidBrush(SMOOTH::Setup::BackgroundColor);
	HFONT			 hfont;
	HFONT			 holdfont;

	GetLocalTime(&time);
	GetTimeZoneInformation(&tzi);

	if (timezone == 1)
	{
		mseconds = time.wHour*3600000 + time.wMinute*60000 + time.wSecond*1000 + time.wMilliseconds;
	}
	else if (timezone == 0)
	{
		mseconds = time.wHour*3600000 + (time.wMinute+tzi.Bias+60)*60000 + time.wSecond*1000 + time.wMilliseconds;

		if (tzi.StandardDate.wYear == 0)
		{
			day = 1;
			first = GetDayOfWeek(1, tzi.StandardDate.wMonth, time.wYear);

			while (first != tzi.StandardDate.wDayOfWeek)
			{
				day++;
				first++;
				if (first == 7) first = 0;
			}

			for (int i = 1; i < tzi.StandardDate.wDay; i++)
			{
				day += 7;
			}

			if (OutOfMonth(day, tzi.StandardDate.wMonth, time.wYear)) day -= 7;

			tzi.StandardDate.wDay = day;
		}

		if (tzi.DaylightDate.wYear == 0)
		{
			day = 1;
			first = GetDayOfWeek(1, tzi.DaylightDate.wMonth, time.wYear);

			while (first != tzi.DaylightDate.wDayOfWeek)
			{
				day++;
				first++;
				if (first == 7) first = 0;
			}

			for (int i = 1; i < tzi.DaylightDate.wDay; i++)
			{
				day += 7;
			}

			if (OutOfMonth(day, tzi.DaylightDate.wMonth, time.wYear)) day -= 7;

			tzi.DaylightDate.wDay = day;
		}

		if (tzi.StandardDate.wMonth != 0 && tzi.DaylightDate.wMonth != 0)
		{
			if ((tzi.StandardDate.wMonth > tzi.DaylightDate.wMonth) && ((tzi.StandardDate.wMonth > time.wMonth && tzi.DaylightDate.wMonth < time.wMonth) || ((tzi.StandardDate.wMonth == time.wMonth && tzi.StandardDate.wDay > time.wDay) || (tzi.DaylightDate.wMonth == time.wMonth && tzi.DaylightDate.wDay < time.wDay)) || ((tzi.StandardDate.wMonth == time.wMonth && tzi.StandardDate.wDay == time.wDay && tzi.StandardDate.wHour > time.wHour) || (tzi.DaylightDate.wMonth == time.wMonth && tzi.DaylightDate.wDay == time.wDay && tzi.DaylightDate.wHour < time.wHour))))
			{
				mseconds = mseconds + tzi.DaylightBias * 60000;
			}
			else if ((tzi.StandardDate.wMonth < tzi.DaylightDate.wMonth) && !((tzi.StandardDate.wMonth > time.wMonth && tzi.DaylightDate.wMonth < time.wMonth) || ((tzi.StandardDate.wMonth == time.wMonth && tzi.StandardDate.wDay > time.wDay) || (tzi.DaylightDate.wMonth == time.wMonth && tzi.DaylightDate.wDay < time.wDay)) || ((tzi.StandardDate.wMonth == time.wMonth && tzi.StandardDate.wDay == time.wDay && tzi.StandardDate.wHour > time.wHour) || (tzi.DaylightDate.wMonth == time.wMonth && tzi.DaylightDate.wDay == time.wDay && tzi.DaylightDate.wHour < time.wHour))))
			{
				mseconds = mseconds + tzi.DaylightBias * 60000;
			}
		}

		while (mseconds > 86400000) mseconds = mseconds - 86400000;
		while (mseconds < 0) mseconds = mseconds + 86400000;
	}

	if (timeformat == 0)
	{
		beats = (int) (mseconds / 86400);
		cbeats = (int) (((double) mseconds / 86400 - (double) beats) * 100);
		ccbeats = 0;

		btext[1] = beats/100 + 48;
		btext[2] = beats%100/10 + 48;
		btext[3] = beats%100%10 + 48;
		btext[4] = 0;

		btext2 = btext;

		btext[4] = 58;
		btext[5] = 47;
		btext[6] = 47;

		btext[7] = cbeats/10 + 48;
		btext[8] = cbeats%10 + 48;
		btext[9] = 0;
	}
	else if (timeformat == 1)
	{
		beats = (int) (mseconds / 3600000);
		cbeats = (int) (((double) mseconds / 3600000 - (double) beats) * 60);
		ccbeats = (int) ((((double) mseconds / 3600000 - (double) beats) * 60 - (double) cbeats) * 60);

		btext[0] = beats/10 + 48;
		btext[1] = beats%10 + 48;
		btext[2] = 58;
		btext[3] = cbeats/10 + 48;
		btext[4] = cbeats%10 + 48;
		btext[5] = 0;

		btext2 = btext;

		btext[5] = 58;
		btext[6] = ccbeats/10 + 48;
		btext[7] = ccbeats%10 + 48;
		btext[8] = 0;
	}

	if (isalarm && timeformat == 0)
	{
		if (beats == alarmbeats && !alarmexec)
		{
			alarmexec = true;

			if (alarmoption == 0) isalarm = false;

			SMOOTH::MessageBox(SMOOTHString("It is @").Append(SMOOTHString::IntToString(beats)).Append(" internet beats!"), "BeatClock alarm", MB_OK, IDI_INFORMATION);
		}

		if (beats != alarmbeats && alarmexec)
		{
			alarmexec = false;
		}
	}
	else if (isalarm && timeformat == 1)
	{
		if (((int) (mseconds / 1000)) == alarmsecs && !alarmexec)
		{
			alarmexec = true;

			if (alarmoption == 0) isalarm = false;

			SMOOTH::MessageBox(SMOOTHString("The time is ").Append(convertSecondsToTimeString(alarmsecs)).Append("!"), "BeatClock alarm", MB_OK, IDI_INFORMATION);
		}

		if (((int) (mseconds / 1000)) != alarmsecs && alarmexec)
		{
			alarmexec = false;
		}
	}

	if (actbeats != beats || actcbeats != cbeats || actccbeats != ccbeats || wmpaint)
	{
		switch (centi)
		{
			case true:
				textrect.left = (int) ((16 * SMOOTH::Setup::FontSize) + 0.5);
				textrect.top = (int) ((53 * SMOOTH::Setup::FontSize) + 0.5);
				textrect.right = wnd->GetObjectProperties()->size.cx - (int) ((16 * SMOOTH::Setup::FontSize) + 0.5);
				textrect.bottom = textrect.top + (int) ((40 * SMOOTH::Setup::FontSize) + 0.5);
				FillRect(dc, &textrect, brush);
				if (timeformat == 1) textrect.left = (int) ((25 * SMOOTH::Setup::FontSize) + 0.5);

				SetBkMode(dc, TRANSPARENT);
				SetTextColor(dc, RGB(0, 0, 0));

				if (SMOOTH::Setup::enableUnicode)	hfont = CreateFontW(-MulDiv(21, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FF_ROMAN, SMOOTHString("Arial"));
				else					hfont = CreateFontA(-MulDiv(21, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FF_ROMAN, SMOOTHString("Arial"));

				holdfont = (HFONT) SelectObject(dc, hfont);

				if (SMOOTH::Setup::enableUnicode)	DrawTextW(dc, btext, -1, &textrect, DT_LEFT);
				else					DrawTextA(dc, btext, -1, &textrect, DT_LEFT);

				SelectObject(dc, holdfont);
				::DeleteObject(hfont);

				actbeats = beats;
				actcbeats = cbeats;
				actccbeats = ccbeats;
				break;
			case false:
				if (actbeats != beats || (timeformat == 1 && actcbeats != cbeats) || wmpaint)
				{
					textrect.left = (int) ((16 * SMOOTH::Setup::FontSize) + 0.5);
					textrect.top = (int) ((53 * SMOOTH::Setup::FontSize) + 0.5);
					textrect.right = wnd->GetObjectProperties()->size.cx - (int) ((16 * SMOOTH::Setup::FontSize) + 0.5);
					textrect.bottom = textrect.top + (int) ((40 * SMOOTH::Setup::FontSize) + 0.5);
					FillRect(dc, &textrect, brush);
					textrect.left = (int) (44 * SMOOTH::Setup::FontSize);
					if (timeformat == 1) textrect.left = (int) ((45 * SMOOTH::Setup::FontSize) + 0.5);

					SetBkMode(dc, TRANSPARENT);
					SetTextColor(dc, RGB(0, 0, 0));

					if (SMOOTH::Setup::enableUnicode)	hfont = CreateFontW(-MulDiv(21, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FF_ROMAN, SMOOTHString("Arial"));
					else					hfont = CreateFontA(-MulDiv(21, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FF_ROMAN, SMOOTHString("Arial"));

					holdfont = (HFONT) SelectObject(dc, hfont);


					if (SMOOTH::Setup::enableUnicode)	DrawTextW(dc, btext2, -1, &textrect, DT_LEFT);
					else					DrawTextA(dc, btext2, -1, &textrect, DT_LEFT);

					SelectObject(dc, holdfont);
					::DeleteObject(hfont);
				}
				actbeats = beats;
				actcbeats = cbeats;
				actccbeats = ccbeats;
				break;
		}
	}

	ReleaseDC(wnd->hwnd, dc);

	::DeleteObject(brush);
}

SMOOTHVoid BeatClock::Info()
{
	SMOOTH::MessageBox("BeatClock version 2.0\n\nDeveloped by Robert Kausch 2000-2002\nGive it to all your friends!\n\neMail: robert.kausch@gmx.net", "Info", MB_OK, IDI_ASTERISK);
}

SMOOTHInt BeatClock::GetDayOfWeek(SMOOTHInt day, SMOOTHInt month, SMOOTHInt year)
{
	month += 10;
	year = (month - 24) / 12 + year;
	day = (((13 * (month - (month / 13) * 12) - 1) / 5 + day + 77 + 5 * (year - (year / 100) * 100) / 4 + year / 400 - (year / 100) * 2) % 7);

	return day;
}

SMOOTHBool BeatClock::OutOfMonth(SMOOTHInt day, SMOOTHInt month, SMOOTHInt year)
{
	if (day > 31) return true;
	else if (day > 30 && (month == 4 || month == 6 || month == 9 || month == 11)) return true;
	else if (day > 29 && month == 2) return true;
	else if (day > 28 && month == 2 && (year%4 != 0 || (year%100 == 0 && year%400 != 0))) return true;
	else return false;
}

SMOOTHInt BeatClock::convertBeatsToSeconds(SMOOTHInt beats)
{
	return (int) (beats * 86.4);
}

SMOOTHInt BeatClock::convertSecondsToBeats(SMOOTHInt seconds)
{
	return (int) (seconds / 86.4);
}

SMOOTHString BeatClock::convertSecondsToTimeString(SMOOTHInt seconds)
{
	SMOOTHString	 rstring = "";

	if (((int) (seconds / 3600)) < 10) rstring.Append("0");

	rstring.Append(SMOOTHString::IntToString(seconds / 3600));
	rstring.Append(":");

	if (((int) ((seconds % 3600) / 60)) < 10) rstring.Append("0");

	rstring.Append(SMOOTHString::IntToString((seconds % 3600) / 60));

	return rstring;
}

SMOOTHInt BeatClock::convertTimeStringToSeconds(SMOOTHString time)
{
	SMOOTHString	 buffer;
	int		 counter = 0;
	int		 rval;

	buffer[0] = time[counter];
	counter++;

	if (time[counter] != ':')
	{
		buffer[1] = time[counter];
		counter++;
	}

	counter++;

	rval = buffer.ToInt() * 3600;

	buffer[0] = time[counter];
	counter++;

	if (time[counter] != ':')
	{
		buffer[1] = time[counter];
		counter++;
	}

	rval = rval + buffer.ToInt() * 60;

	return rval;
}

SMOOTHVoid BeatClock::RegisterValues()
{
	HKEY	 beatclock;

	RegCreateKey(HKEY_LOCAL_MACHINE, _TEXT("Software\\BeatClock"), &beatclock);

	RegSetValueEx(beatclock, _TEXT("TimeFormat"), 0, REG_DWORD, (BYTE *) &timeformat, 4);
	RegSetValueEx(beatclock, _TEXT("TimeZone"), 0, REG_DWORD, (BYTE *) &timezone, 4);
	RegSetValueEx(beatclock, _TEXT("CentiBeats"), 0, REG_DWORD, (BYTE *) &centi, 4);
	RegSetValueEx(beatclock, _TEXT("ModeChange"), 0, REG_DWORD, (BYTE *) &modechange, 4);
	RegSetValueEx(beatclock, _TEXT("Alarm"), 0, REG_DWORD, (BYTE *) &isalarm, 4);
	RegSetValueEx(beatclock, _TEXT("AlarmBeats"), 0, REG_DWORD, (BYTE *) &alarmbeats, 4);
	RegSetValueEx(beatclock, _TEXT("AlarmSecs"), 0, REG_DWORD, (BYTE *) &alarmsecs, 4);
	RegSetValueEx(beatclock, _TEXT("AlarmOption"), 0, REG_DWORD, (BYTE *) &alarmoption, 4);
	RegSetValueEx(beatclock, _TEXT("WindowPosX"), 0, REG_DWORD, (BYTE *) &wpx, 4);
	RegSetValueEx(beatclock, _TEXT("WindowPosY"), 0, REG_DWORD, (BYTE *) &wpy, 4);

	RegCloseKey(beatclock);
}

SMOOTHVoid BeatClock::InputValues()
{
	HKEY	beatclock;
	DWORD	size;
	DWORD	type;

	if (RegOpenKey(HKEY_LOCAL_MACHINE, _TEXT("Software\\BeatClock"), &beatclock) != ERROR_SUCCESS)
	{
		RegisterValues();
	}
	else
	{
		RegQueryValueEx(beatclock, _TEXT("TimeFormat"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("TimeFormat"), 0, &type, (BYTE *) &timeformat, &size);

		RegQueryValueEx(beatclock, _TEXT("TimeZone"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("TimeZone"), 0, &type, (BYTE *) &timezone, &size);

		RegQueryValueEx(beatclock, _TEXT("CentiBeats"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("CentiBeats"), 0, &type, (BYTE *) &centi, &size);

		RegQueryValueEx(beatclock, _TEXT("ModeChange"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("ModeChange"), 0, &type, (BYTE *) &modechange, &size);

		RegQueryValueEx(beatclock, _TEXT("Alarm"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("Alarm"), 0, &type, (BYTE *) &isalarm, &size);

		RegQueryValueEx(beatclock, _TEXT("AlarmBeats"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("AlarmBeats"), 0, &type, (BYTE *) &alarmbeats, &size);

		RegQueryValueEx(beatclock, _TEXT("AlarmSecs"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("AlarmSecs"), 0, &type, (BYTE *) &alarmsecs, &size);

		RegQueryValueEx(beatclock, _TEXT("AlarmOption"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("AlarmOption"), 0, &type, (BYTE *) &alarmoption, &size);

		RegQueryValueEx(beatclock, _TEXT("WindowPosX"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("WindowPosX"), 0, &type, (BYTE *) &wpx, &size);

		RegQueryValueEx(beatclock, _TEXT("WindowPosY"), 0, &type, 0, &size);
		RegQueryValueEx(beatclock, _TEXT("WindowPosY"), 0, &type, (BYTE *) &wpy, &size);

		RegCloseKey(beatclock);
	}
}