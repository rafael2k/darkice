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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libintl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>

#ifndef __GLOBAL_H__
#include "global.h"
#endif

#ifndef __CONSTANTS_H__
#include "constants.h"
#endif

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

#ifndef __CONFIG_FILES_H__
#include "config_files.h"
#endif

#ifndef __INTERFACE_H__
#include "interface.h"
#endif

#ifdef FUNNY
#define FUN 1
#else 
#define FUN 0
#endif

/* callback that start darkice */
void dark_start( GtkWidget *widget, gpointer data) {
  char *darkice_verbosity;
  char command[512], foo, dark_output[512];

  /* if the first char of shared area is 1, means that the streaming is running and we cannot start another darkice */
  sscanf((char *) shared_area, "%c", &foo);
  if (foo == '1')
    return;


  /* call the function that load the values from widgets and save to the darksnow config file */
  darksnow_config_store(1, NULL);
  /* call the function that translate the darksnow config file to the darkice config file */
  darksnow2darkice_cfg();

  darkice_verbosity = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_verbosity));

  strcpy(command,"darkice");
  strcat(command," -v ");
  strcat(command, darkice_verbosity);
  strcat(command," -c ");
  strcat(command, darkice_cfg);
  strcat(command," > ");
  strcat(command, darksnow_temp);


  if ((pid = fork()) == -1) {
    printf("Could not start 'darkice' process, Fork error! \n");
    gtk_main_quit();
  }
  
  if (!pid) {
    /* streaming started */
    sprintf ((char *) shared_area, "11");
    system(command);
    /* if the system() returns, this means that the streaming stopped */
    sprintf ((char *) shared_area, "01");
    _exit(-1);
  }

  strcpy(dark_output, gettext("-->    Command     <--\n"));
  strcat(dark_output, command);
  strcat(dark_output, gettext("\n--> Darkice output <--\n") );

  gtk_text_buffer_set_text (buffer, dark_output , strlen(dark_output));
  
  /* print the darkice output for the first time */
  usleep(200000);
  text_box_loop (1); 
}

/* callback that stop darkice */
void dark_stop( GtkWidget *widget, gpointer data ){
  char out[32];

  system("killall darkice 2> /dev/null");
  strcpy(out, gettext("Darkice off") );
  gtk_text_buffer_set_text (buffer, out, strlen(out));

}

void dark_put_in_box ( GtkWidget *widget, gint response_id, gpointer data ) {
  FILE *f_darksnow_cfg;
  char *darksnow_path;
  char foo[256] = {0};
  char server[256] = {0};
  char port[128] = {0};
  char mountpoint[128] = {0};
  char pass[128] = {0};
  char remotedump[256] = {0};
  char localdump[256] = {0};
  char adddate;
  char radioname[128] = {0};
  char description[256] = {0};
  char url[256] = {0};
  char genre[128] = {0};
  char icecast[128] = {0};
  char format[128] = {0};
  char bitrate[128] = {0};
  char bitratemode[128] = {0};
  char samplerate[128] = {0};
  char quality[128] = {0};
  char bitspersample[128] = {0};
  char channel[128] = {0};
  char buffersize[128] = {0};
  char device[128] = {0};
  char public[128] = {0};
  char verbosity[8] = {0};
  DIR *directory;

  if (response_id != GTK_RESPONSE_ACCEPT) {
    gtk_widget_hide(file_open);
    return ;
  }

  darksnow_path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_open));
  
  if ( (directory = opendir(darksnow_path))) {
    printf("Error: %s is a directory\n", darksnow_path);
    closedir (directory);
    return;
  }

  if (!(f_darksnow_cfg = fopen(darksnow_path, "r"))) {
    printf("Error: Cannot open %s\n", darksnow_path);
    return;
  }

  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, server);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, port);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, mountpoint);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, pass);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, radioname);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, description);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, url);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, genre);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, icecast);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, format);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, bitrate);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, bitratemode);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, samplerate);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, quality);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, bitspersample);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, channel);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, buffersize);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, device);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, public);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, verbosity);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, remotedump);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, localdump);
  fscanf(f_darksnow_cfg, "%[^=]=%c\n", foo, &adddate);

  gtk_entry_set_text ( (GtkEntry *) entry_server, server);
  gtk_entry_set_text ( (GtkEntry *) entry_port, port);
  gtk_entry_set_text ( (GtkEntry *) entry_mountpoint, mountpoint);
  gtk_entry_set_text ( (GtkEntry *) entry_pass, pass);
  gtk_entry_set_text ( (GtkEntry *) entry_radioname, radioname);
  gtk_entry_set_text ( (GtkEntry *) entry_description, description);
  gtk_entry_set_text ( (GtkEntry *) entry_url, url);
  gtk_entry_set_text ( (GtkEntry *) entry_genre, genre);
  gtk_entry_set_text ( (GtkEntry *) entry_server, server);
  gtk_entry_set_text ( (GtkEntry *) entry_remotedump, remotedump);
  gtk_entry_set_text ( (GtkEntry *) entry_localdump, localdump);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_icecast))), icecast);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_format))), format);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_bitrate))), bitrate);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_bitratemode))), bitratemode);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_samplerate))), samplerate);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_quality))), quality);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_bitspersample))), bitspersample);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_channel))), channel);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_buffersize))), buffersize);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_device))), device);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_public))), public);
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_verbosity))),verbosity);


  if (adddate == '1')
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (checkbutton_adddate), TRUE);
  else
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (checkbutton_adddate), FALSE);

  fclose(f_darksnow_cfg);

  gtk_widget_hide(file_open);
  
}

void dark_write_config ( GtkWidget *widget, gint response_id, gpointer data ) {
  char *darksnow_path;

  if (response_id != GTK_RESPONSE_ACCEPT) {
    gtk_widget_hide(file_save);
    return ;
  }

  darksnow_path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_save));
  if (darksnow_config_store (0, darksnow_path))
    gtk_widget_hide(file_save);
  
}




void dark_localdump (GtkWidget *widget, gint response_id,  gpointer data){
  if (response_id != GTK_RESPONSE_ACCEPT) {
    gtk_widget_hide(file_localdump);
    return ;
  }

  gtk_entry_set_text ( (GtkEntry *) entry_localdump, gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_localdump)));
  gtk_widget_hide (file_localdump);

}

void clean_text_view_memory(){
  GtkTextIter start_iter, end_iter;

  gtk_text_buffer_get_bounds (buffer, &start_iter, &end_iter);
  gtk_text_iter_forward_lines (&start_iter, 6);
  gtk_text_iter_backward_lines (&end_iter, 6);
  gtk_text_buffer_delete(buffer, &start_iter, &end_iter);

}


gboolean status_loop (gboolean foo) {
  char status[3];
  static int alarm_id = 0;
  

  /** This checks if the status of the transmission if ON or OFF, using the shared area **/
  sscanf((char *) shared_area, "%s", status);

  /* if the update label flag is on */
  if (status[1] == '1') {
    /** streaming stopped **/
    if (status [0] == '0') {
      gtk_label_set_text(GTK_LABEL (label_status), gettext("Streaming Status: Stopped")); 
      text_box_loop(1);
      if (alarm_id)
	g_source_remove(alarm_id);
    }
    /** streaming started **/
    else {
      gtk_label_set_text(GTK_LABEL (label_status), gettext("Streaming Status: Started")); 
      text_box_loop(0);
      alarm_id = g_timeout_add (4000, (GSourceFunc) text_box_loop, 0);
    }

    strncpy((char *) shared_area+1, "0", 1);  
  }

  /* I need nothing from child, just do this to prevent <defunct> processes */
  waitpid(pid, NULL, WNOHANG);

  return TRUE;
}

#define BUFFER_SIZE 16384

gboolean text_box_loop (int option) {
  char dark_output[BUFFER_SIZE];
  char status[3];
  int i;
  static int bytes_written = 0;

  sscanf((char *) shared_area, "%s", status);
  
  if (status[0] == '1' || option == 1) {
    
    while ( (i = read (darkice_fd, dark_output, BUFFER_SIZE - 1))  && i > 0 ) {
      dark_output[i] = 0;
      gtk_text_buffer_insert_at_cursor (buffer, dark_output, strlen(dark_output));
      bytes_written = bytes_written + i;

      /* 32 * BUFFER_SIZE =  */
      if (bytes_written > 524288){
	clean_text_view_memory();
	bytes_written = 0;
      }
    }
  }
  return TRUE; 
} 

/* callback que chama a funcao de saida do programa, eh issu mermo, mata tudo mermaum!! */
gboolean delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data) {

  system("killall darkice &> /dev/null");
  gtk_main_quit ();
  return FALSE;
}

/* callback that exit without killing darkice */
gboolean delete_event_nd (GtkWidget *widget, GdkEvent  *event, gpointer data) {
  char foo;

  sscanf((char *) shared_area, "%c", &foo);
  if (foo == '1')
    kill(pid,SIGTERM);

  gtk_main_quit ();
  return FALSE;
}

gboolean main_quit (GtkWidget *widget, GdkEvent  *event, gpointer data) {
  char status = '0';

  sscanf ((char *) shared_area, "%c", &status);

  /* if darkice is running, ask for killing it */
  if (status == '1') {
    gtk_widget_show (button_yes_darkkill);
    gtk_widget_show (button_no_darkkill);
    gtk_widget_show (label_darkkill);
    gtk_widget_show (dialog_darkkill);
  }
  else
    exit(0);
  //delete_event_nd(NULL, NULL, NULL);
  
  return FALSE;
      
}


/* function that is called when darkice is not found */
void darkice_not_found() {
  char bar[192];

  /* sets up the darkice not found dialog */
  if (!FUN)
    sprintf(bar, gettext("Darkice binary 'darkice' not found on path!\nDownload darkice at http://code.google.com/p/darkice/"));
  else
    sprintf(bar, gettext("Don't you know you should have the fucking\ndarkice to run this shit!!\nGet it at http://code.google.com/p/darkice/\n"));
  
  dialog_darkdep = gtk_dialog_new ();
  label_darkdep = gtk_label_new ( bar );
  button_darkdep = gtk_button_new_with_label ( gettext("Close"));
  gtk_window_set_title(GTK_WINDOW (dialog_darkdep), gettext("Error"));

  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_action_area( GTK_DIALOG (dialog_darkdep))), button_darkdep, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area( GTK_DIALOG (dialog_darkdep))), label_darkdep, TRUE, TRUE, 0);

  g_signal_connect (G_OBJECT (dialog_darkdep), "delete_event",G_CALLBACK (delete_event), NULL);
  g_signal_connect (G_OBJECT (button_darkdep), "clicked",G_CALLBACK (delete_event), NULL);
  
  gtk_widget_show (button_darkdep);
  gtk_widget_show (label_darkdep);
  gtk_widget_show (dialog_darkdep);

  gtk_main ();

}

void dark_detail (GtkWidget *widget, gpointer data) {
  if (show_detail) {
    show_detail = 0;
    gtk_button_set_label (GTK_BUTTON(button_detail), gettext("Show Details"));
    gtk_widget_hide (scroll_text);
    gtk_widget_hide (text);
    gtk_window_resize (GTK_WINDOW(window), SIZE_X, SIZE_Y);
  }
  else{
    show_detail = 1;
    gtk_button_set_label (GTK_BUTTON(button_detail), gettext("Hide Details"));
    gtk_widget_show (text);
    gtk_widget_show (scroll_text);
    gtk_window_resize (GTK_WINDOW(window), SIZE_X, SIZE_Y+110);
  }
}
