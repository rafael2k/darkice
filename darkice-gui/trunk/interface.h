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


#define __INTERFACE_H__

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

/* Callback for button_start click */
void dark_start (GtkWidget *widget, gpointer data);

/* Callback for button_stop click */
void dark_stop (GtkWidget *widget, gpointer data);

/* Callback for button_load click */
void dark_load (GtkWidget *widget, gpointer data);

/* Callback for OK button of the "Open File" file selector, displayed by dark_open */
void dark_put_in_box (GtkWidget *widget, gint response_id, gpointer data);

/* Callback for OK button of the "Save File" file selector, displayed by dark_save */
void dark_write_config ( GtkWidget *widget, gint response_id, gpointer data );

/* Callback for the local dump file selector 'ok' button */
void dark_localdump (GtkWidget *widget, gint response_id,  gpointer data);

/* Cleans the buffer inside the view box */
void clean_text_view_memory();

/* The function that updates the streaming status */
gboolean status_loop (gboolean foo);

/* The function that updates the darkice output window, when option == 1, means that     */
/* the "streaming_started" flag will be ignored, if option == 0, the "streaming_started" */
/* flag will not be ignored and the debug window will not be updated.                    */
gboolean text_box_loop (int option);

/* Callback that call the gtk_main_quit() */
gboolean delete_event (GtkWidget *widget, GdkEvent *event, gpointer data );

/* Callback that call the gtk_main_quit() without killing darkice */
gboolean delete_event_nd (GtkWidget *widget, GdkEvent *event, gpointer data );

/* Callback that shows a dialog asking for killing or not darkice before quitting */
gboolean main_quit (GtkWidget *widget, GdkEvent  *event, gpointer data);

/* darkice not found function */
void darkice_not_found();

/* callback function that is called when Detail button is clicked */
void dark_detail (GtkWidget *widget, gpointer data);
