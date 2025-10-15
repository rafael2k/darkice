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

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

/* shared area */
extern void *shared_area;

/* darkice execution child process pid */
extern int pid;

/* file descriptor used by darkice output pipe */
extern int darkice_fd;

/* the user home */
extern char home[64];
extern char darksnow_cfg[64];
extern char darkice_cfg[64];
extern char darksnow_temp[64];
extern char show_detail;

/* main window Widgets */
extern GtkWidget *window;
extern GtkWidget *button_start;
extern GtkWidget *button_stop;
extern GtkWidget *button_detail;
extern GtkWidget *label_status;
extern GtkWidget *table;

/* About Dialog box */
extern GtkWidget *dialog_about;

/* darkice not found dialog box */
extern GtkWidget *dialog_darkdep;
extern GtkWidget *label_darkdep;
extern GtkWidget *button_darkdep;

/* "kill darkice?" dialog box */
extern GtkWidget *dialog_darkkill;
extern GtkWidget *label_darkkill;
extern GtkWidget *button_yes_darkkill;
extern GtkWidget *button_no_darkkill;
extern GtkWidget *button_cancel_darkkill;

/* menu stuff */
extern GtkWidget *menu_bar;
extern GtkWidget *menu_file;
extern GtkWidget *menu_item_file;
extern GtkWidget *menu_item_open;
extern GtkWidget *menu_item_save;
extern GtkWidget *menu_item_quit_nd;
extern GtkWidget *menu_item_quit;
extern GtkWidget *menu_help;
extern GtkWidget *menu_item_help;
extern GtkWidget *menu_item_about;
extern GtkWidget *vbox;

/* open and save file selectors */
extern GtkWidget *file_open;
extern GtkWidget *file_save;
extern GtkWidget *file_localdump;

/* notebook stuff */
extern GtkWidget *notebook;
extern GtkWidget *label_notebook_server;
extern GtkWidget *table_server;
extern GtkWidget *label_notebook_audio;
extern GtkWidget *table_audio;
extern GtkWidget *label_notebook_description;
extern GtkWidget *table_description;

/* darksnow configuration stuff - button + entry */

extern GtkWidget *button_localdump;
extern GtkWidget *entry_localdump;
extern GtkWidget *checkbutton_adddate;
extern GtkWidget *label_remotedump;
extern GtkWidget *entry_remotedump;

/* darksnow configuration stuff - label + entry */
extern GtkWidget *label_server;
extern GtkWidget *entry_server;
extern GtkWidget *label_port;
extern GtkWidget *entry_port;
extern GtkWidget *label_mountpoint;
extern GtkWidget *entry_mountpoint;
extern GtkWidget *label_pass;
extern GtkWidget *entry_pass;
extern GtkWidget *label_radioname;
extern GtkWidget *entry_radioname;
extern GtkWidget *label_description;
extern GtkWidget *entry_description;
extern GtkWidget *label_url;
extern GtkWidget *entry_url;
extern GtkWidget *label_genre;
extern GtkWidget *entry_genre;

/* darksnow configuration stuff - label + combo box */
extern GtkWidget *label_icecast;
extern GtkWidget *combo_icecast;
extern GtkWidget *label_format;
extern GtkWidget *combo_format;
extern GtkWidget *label_bitrate;
extern GtkWidget *combo_bitrate;
extern GtkWidget *label_bitratemode;
extern GtkWidget *combo_bitratemode;
extern GtkWidget *label_samplerate;
extern GtkWidget *combo_samplerate;
extern GtkWidget *label_quality;
extern GtkWidget *combo_quality;
extern GtkWidget *label_bitspersample;
extern GtkWidget *combo_bitspersample;
extern GtkWidget *label_channel;
extern GtkWidget *combo_channel;
extern GtkWidget *label_buffersize;
extern GtkWidget *combo_buffersize;
extern GtkWidget *label_device;
extern GtkWidget *combo_device;
extern GtkWidget *label_public;
extern GtkWidget *combo_public;
extern GtkWidget *label_verbosity;
extern GtkWidget *combo_verbosity;

/* text view stuff */
extern GtkWidget *text;
extern GtkTextBuffer *buffer;
extern GtkTextTagTable *tag_table;
extern GtkTextTag *tag;

/* text view has to scroll */
extern GtkWidget *scroll_text;

/* The GTK ToolTips */
extern GtkTooltip *tooltip;

#endif
