/* Darksnow - A GUI for darkice
 * Copyright (C) 2004-2005 Rafael Diniz <rafael@riseup.net>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

#ifndef __GLOBAL_H__
#include "global.h"
#endif

#ifdef USE_TOOLTIP
#define TOOLTIP 1
#else 
#define TOOLTIP 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>

void set_all_tooltips(){


  if (TOOLTIP) {
    gtk_widget_set_tooltip_text ( entry_server, gettext("Field that indicates the name of the server.\nEg: orelha2.radiolivre.org"));
    gtk_widget_set_tooltip_text ( entry_port, gettext("Field that indicates the server port that you'll connect.\nEg: 8000"));
    gtk_widget_set_tooltip_text ( entry_mountpoint, gettext("Field that indicates the mount point for the stream.\nEg: radio_x.ogg\n Remember to put a \".ogg\" at the end of the mountpoint if you want to use ogg/vorbis!"));
    gtk_widget_set_tooltip_text ( entry_pass, gettext("Field that indicates the password for the stream, provided by the server administrator."));
    gtk_widget_set_tooltip_text ( entry_remotedump, gettext("Field that indicates the file \
the IceCast server should dump the contents of this stream on its side (server side).\nEg: remote_file_name.mp3"));
    gtk_widget_set_tooltip_text ( entry_localdump, gettext("Field that indicates the file where \
the same data sent to the stream will be dumped.\nEg: local_file_name.mp3"));
    gtk_widget_set_tooltip_text ( checkbutton_adddate, gettext("If you want to automatically \
insert a date string in the localDumpFile name, check this option."));
    gtk_widget_set_tooltip_text ( combo_verbosity, gettext("Field that indicates the verbosity level of darkice.\nUse a value greater than 0 if you want more information in 'Details Window'."));
    gtk_widget_set_tooltip_text ( combo_device, gettext("For OSS DSP audio \
device to record from, use /dev/dsp_ (eg. /dev/dsp), for ALSA, use hw:_,_ (eg. hw:0,0), and to create an \
unconnected input \
port in jack, use 'jack', or use 'jack_auto' to automatically make Jack connect \
to the first source. (if you choose jack you must set the samplerate to the same jack samplerate!)"));

  }
    
}
