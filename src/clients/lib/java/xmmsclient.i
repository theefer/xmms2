/*  XMMS2 - X Music Multiplexer System
 *  Copyright (C) 2003-2006 XMMS2 Team
 *
 *  PLUGINS ARE NOT CONSIDERED TO BE DERIVED WORK !!!
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 */

%module Xmmsclient

%include "typemaps.i"
%include "various.i"
%include "arrays_java.i"

%apply int *INOUT { int* };
%apply int *INOUT { void* };
%apply unsigned int *INOUT { unsigned int* };
%apply char **STRING_OUT { char ** };
%apply char **STRING_ARRAY { char **preference };

typedef int int32_t;
typedef int uint32_t;

%{
#include <xmmsclient/xmmsclient.h>
#include <xmmsc/xmmsc_idnumbers.h>
#include <xmmsc/xmmsc_errorcodes.h>
#include <xmmsc/xmmsc_stdint.h>
#include <callbacks.h>
#include <misc.h>

%} 

%include "src/include/xmmsclient/xmmsclient.h"
%include "include/misc.h"

%constant void (*DISCONNECT_CALLBACK)(void*) = disconnect_callback;
%constant void (*LOCK_FUNCTION)(void*) = lock_function;
%constant void (*UNLOCK_FUNCTION)(void*) = unlock_function;
%constant void (*CALLBACK_CONFIGVAL_CHANGED)(xmmsc_result_t*, void*) = callback_configval_changed;
%constant void (*CALLBACK_PLAYLIST_CHANGED)(xmmsc_result_t*, void*) = callback_playlist_changed;
%constant void (*CALLBACK_PLAYLIST_CURRENT_POSITION)(xmmsc_result_t*, void*) = callback_playlist_current_position;
%constant void (*CALLBACK_PLAYBACK_ID)(xmmsc_result_t*, void*) = callback_playback_id;
%constant void (*CALLBACK_PLAYBACK_STATUS)(xmmsc_result_t*, void*) = callback_playback_status;
%constant void (*CALLBACK_PLAYBACK_VOLUME_CHANGED)(xmmsc_result_t*, void*) = callback_playback_volume_changed;
%constant void (*CALLBACK_MEDIALIB_ENTRY_CHANGED)(xmmsc_result_t*, void*) = callback_medialib_entry_changed;
%constant void (*CALLBACK_MEDIALIB_ENTRY_ADDED)(xmmsc_result_t*, void*) = callback_medialib_entry_added;
%constant void (*CALLBACK_MEDIALIB_PLAYLIST_LOADED)(xmmsc_result_t*, void*) = callback_medialib_playlist_loaded;
%constant void (*CALLBACK_MEDIAINFO_READER_STATUS)(xmmsc_result_t*, void*) = callback_mediainfo_reader_status;
%constant void (*SIGNAL_PLAYBACK_PLAYTIME)(xmmsc_result_t*, void*) = signal_playback_playtime;
%constant void (*SIGNAL_VISUALIZATION_DATA)(xmmsc_result_t*, void*) = signal_visualization_data;
%constant void (*SIGNAL_MEDIAREADER_UNINDEXED)(xmmsc_result_t*, void*) = signal_mediareader_unindexed;
%constant void (*USER_DEFINED_CALLBACK_1)(xmmsc_result_t*, void*) = user_defined_callback_1;
%constant void (*USER_DEFINED_CALLBACK_2)(xmmsc_result_t*, void*) = user_defined_callback_2;
%constant void (*USER_DEFINED_CALLBACK_3)(xmmsc_result_t*, void*) = user_defined_callback_3;
%constant void (*CALLBACK_DICT_FOREACH_FUNCTION)(const void*, xmmsc_result_value_type_t, const void*, void*) = callback_dict_foreach_function;
%constant void (*CALLBACK_PROPDICT_FOREACH_FUNCTION)(const void*, xmmsc_result_value_type_t, const void*, const char*, void*) = callback_propdict_foreach_function;