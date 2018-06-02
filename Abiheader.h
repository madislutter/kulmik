#pragma once

#include "Liige.h"
#include "Toode.h"

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }
const char endl = '\n';

using clear_cb = void (*) ();
using draw_cb = void (*) (byte row, const char* content);
using draw_F_cb = void (*) (byte row, const __FlashStringHelper* content);
using buffer_fill_cb = byte (*) (int idx, byte amount, char* buffer, byte buffer_len);
using salvesta_liige_cb = bool (*) (Liige &l);
using salvesta_toode_cb = bool (*) (Toode &t);
using salvesta_tehing_cb = bool (*) (Liige &l, Toode &t);
