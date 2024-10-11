#pragma once

extern "C" __declspec(dllexport) long __stdcall SendAdvOrder(wchar_t* sym, wchar_t* sec, wchar_t* cur, wchar_t* exch, int direction, wchar_t* vol, double sl, double tp);