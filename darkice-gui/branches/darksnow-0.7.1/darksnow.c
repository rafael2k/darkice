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
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>


#ifndef __GLOBAL_H__
#include "global.h"
#endif

/* some defines */
#ifndef __CONSTANTS_H__
#include "constants.h"
#endif

#ifndef __INTERFACE_H__
#include "interface.h"
#endif

#ifndef __TOOLTIPS_H__
#include "tooltips.h"
#endif

int main( int  argc, char *argv[], char *envp[]) {
  DIR *directory;
  char *foo;
  char bar[256];
  char locale[16];
  int i = 0, j = 0, fd;

  if (!gtk_init_check (&argc, &argv)) {
    fprintf(stderr, "Display cannot be initialized, wait for the curses interface :-P\n");
    exit (-1);
  }

  if (envp[i] == NULL) {
    fprintf(stderr, "hummmm, Environment variables not found?\n");
    exit(-1);
  }
  

  /* Sets the shared area                             */
  /* 00 means streaming stopped, don't update label   */
  /* 01 means streaming stopped, update label         */
  /* 10 means streaming started, don't update label   */
  /* 11 means streaming started, update label         */
  shared_area = mmap (0, 3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
  sprintf ((char *) shared_area, "00"); 

  /* get the user HOME */
  while (envp[i] != NULL && strncmp("HOME", envp[i++], 4));
  if (envp[i] != NULL)
    strcpy(home, envp[i-1]+5);

  /* if not exists, create the config directory */
  strcat(home,"/.darksnow");
  if ( !(directory = opendir(home)) ) {
    if (mkdir(home, S_IRWXU ))
      fprintf(stderr, "Error: %s cannot be created\n", home);
  }
  else
    closedir(directory);

  /* sets the location of the configuration files */
  strcpy (darksnow_cfg, home);
  strcat (darksnow_cfg, "/darksnow.cfg");
  strcpy (darkice_cfg, home);
  strcat (darkice_cfg, "/darkice.cfg");
  strcpy (darksnow_temp, home);
  strcat (darksnow_temp, "/darksnow.temp");

  /* the fifo for the debug window of darkice */
  unlink(darksnow_temp);
  if(mkfifo(darksnow_temp, S_IRWXU )) 
    fprintf(stderr, "error creating the fifo at %s\n", darksnow_temp);
  darkice_fd = open (darksnow_temp, O_RDONLY | O_NONBLOCK);
  
  /* internacionalization stuff */
  setlocale (LC_ALL, "");

  strcpy(locale, INTLPREFIX);
  strcat(locale, "/share/locale");

  bindtextdomain ("darksnow", locale);
  textdomain ("darksnow");

  /* check if darkice exists in path */
  i = 0; j = 5;
  while (envp[i] != NULL && strncmp("PATH", envp[i++], 4));

  while ( (sscanf(envp[i-1]+j,"%[^:]:",bar) != -1) && j){ 
    j = j + strlen(bar) + 1;
    fd = 0;
    while ( (bar[fd] != '=') && (bar[fd] != 0) ) ++fd;
    if (bar[fd] == '=') goto end;
    if ( (fd = open(strcat(bar,"/darkice"), O_RDONLY)) != -1) {
	j = 0;   /* 0 means darkice is found */
	close (fd);
    }
  }
  
  
  if (j){
  end:
    darkice_not_found();
    exit(-1);
  }
  
  
  /* Now the GTK+ stuff ...  */

  g_timeout_add (1000, (GSourceFunc) status_loop, 0); /* status_loop() updates the streaming status */
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  button_start = gtk_button_new_with_label ( gettext("Start Streaming") );
  button_stop = gtk_button_new_with_label ( gettext("Stop Streaming") );
  button_detail = gtk_button_new_with_label ( gettext("Show Details") );
  show_detail = 0;
  label_status = gtk_label_new ( gettext("Streaming Status: Stopped"));
  table = gtk_table_new (2, 1, FALSE);
  vbox = gtk_vbox_new (FALSE, 0); /* packs the menu_bar and the table */


  /* main window stuff */
  gtk_window_set_title (GTK_WINDOW (window), "DarkSnow");
  gtk_widget_set_size_request (GTK_WIDGET (window), SIZE_X, SIZE_Y); 
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);


  /* sets up the upper menu stuff */
  menu_bar = gtk_menu_bar_new();  

  menu_file = gtk_menu_new();
  menu_item_file = gtk_menu_item_new_with_label ( gettext("File") );
  menu_item_open = gtk_menu_item_new_with_label ( gettext("Open Configuration") );
  menu_item_save = gtk_menu_item_new_with_label ( gettext("Save Configuration") );
  menu_item_quit_nd = gtk_menu_item_new_with_label ( gettext("Quit and don't stop darkice") );
  menu_item_quit = gtk_menu_item_new_with_label ( gettext("Quit") ); 
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_file), menu_item_open);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_file), menu_item_save);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_file), menu_item_quit_nd);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_file), menu_item_quit);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item_file), menu_file);
  gtk_menu_shell_append ( GTK_MENU_SHELL (menu_bar), menu_item_file );

  menu_help = gtk_menu_new();
  menu_item_help = gtk_menu_item_new_with_label ( gettext("Help") );
  menu_item_about = gtk_menu_item_new_with_label ( gettext("About") );
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_help), menu_item_about);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item_help), menu_help);
  gtk_menu_shell_append ( GTK_MENU_SHELL (menu_bar), menu_item_help );

  gtk_box_pack_start (GTK_BOX (vbox), menu_bar, FALSE, FALSE, 0);

  
  /* sets up the about dialog */
  dialog_about = gtk_about_dialog_new ();
  const gchar *authors[] = {"Rafael Diniz", NULL};
  gtk_about_dialog_set_program_name( GTK_ABOUT_DIALOG( dialog_about), "DarkSnow");
  gtk_about_dialog_set_version( GTK_ABOUT_DIALOG( dialog_about), VERSION);
  gtk_about_dialog_set_authors( GTK_ABOUT_DIALOG(dialog_about), authors);
/*  gtk_about_dialog_set_license_type( GTK_ABOUT_DIALOG(dialog_about), GTK_LICENSE_GPL_3_0);*/
  gtk_about_dialog_set_website( GTK_ABOUT_DIALOG(dialog_about), "http://darksnow.radiolivre.org");

  /* sets up the exit window */
  dialog_darkkill = gtk_dialog_new ();
  label_darkkill = gtk_label_new (gettext("Darkice is running.\nDo you want to kill darkice?\n"));
  button_yes_darkkill = gtk_button_new_with_label (gettext("Yes"));
  button_no_darkkill = gtk_button_new_with_label (gettext("No"));

  gtk_window_set_title(GTK_WINDOW (dialog_darkkill), gettext("Darkice is running"));
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_action_area( GTK_DIALOG (dialog_darkkill))), button_yes_darkkill, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_action_area( GTK_DIALOG (dialog_darkkill))), button_no_darkkill, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area( GTK_DIALOG (dialog_darkkill))), label_darkkill, TRUE, TRUE, 0);
  
  g_signal_connect_swapped (G_OBJECT (dialog_darkkill), "delete_event",G_CALLBACK (gtk_widget_hide), dialog_darkkill);
  g_signal_connect (G_OBJECT (button_yes_darkkill), "clicked",G_CALLBACK (delete_event), NULL);
  g_signal_connect (G_OBJECT (button_no_darkkill), "clicked",G_CALLBACK (delete_event_nd), NULL);

  /* sets up the file selectors */
  file_open = gtk_file_chooser_dialog_new ( gettext("Open File"),
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                          NULL);
  file_save = gtk_file_chooser_dialog_new ( gettext("Save File"),
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_SAVE,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                          NULL);
  file_localdump = gtk_file_chooser_dialog_new ( gettext("Local Dump File"),
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                          NULL);

  gtk_file_chooser_set_local_only ( GTK_FILE_CHOOSER( file_open), FALSE);
  gtk_file_chooser_set_local_only ( GTK_FILE_CHOOSER( file_save), FALSE);
  gtk_file_chooser_set_local_only ( GTK_FILE_CHOOSER( file_localdump), FALSE);

  /* sets up the notebook stuff */
  notebook = gtk_notebook_new ();
  gtk_notebook_set_tab_pos ( GTK_NOTEBOOK (notebook), GTK_POS_TOP );

  table_server = gtk_table_new (1, 1, FALSE);
  table_audio = gtk_table_new (1, 1, FALSE);
  table_description = gtk_table_new (1, 1, FALSE);

  label_notebook_server = gtk_label_new ( gettext("Server Options"));
  label_notebook_audio = gtk_label_new ( gettext("Audio Options"));
  label_notebook_description = gtk_label_new ( gettext("Streaming Description"));
  
  gtk_notebook_insert_page ( GTK_NOTEBOOK (notebook), table_server, label_notebook_server, 1);
  gtk_notebook_insert_page ( GTK_NOTEBOOK (notebook), table_audio, label_notebook_audio, 2);
  gtk_notebook_insert_page ( GTK_NOTEBOOK (notebook), table_description, label_notebook_description, 3);

  /* Server Options Widgets */
  label_icecast = gtk_label_new ( gettext("Streaming Destination: "));
  combo_icecast = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_icecast), "Icecast 1");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_icecast), "Icecast 2");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_icecast), "Shoutcast");
  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (combo_icecast))),"Icecast 2");

  label_server = gtk_label_new ( gettext("Server: "));
  entry_server = gtk_entry_new ();
  label_port = gtk_label_new ( gettext("Port: "));
  entry_port = gtk_entry_new ();
  label_mountpoint = gtk_label_new ( gettext("Mount Point: "));
  entry_mountpoint = gtk_entry_new ();
  label_pass = gtk_label_new ( gettext("Password: "));
  entry_pass = gtk_entry_new ();
  gtk_entry_set_visibility (GTK_ENTRY(entry_pass), FALSE);
  
  label_verbosity = gtk_label_new ( gettext ("Verbosity level: "));
  combo_verbosity = gtk_combo_box_text_new_with_entry();

  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "0");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "1");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "2");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "3");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "4");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "5");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "6");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "7");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "8");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "9");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_verbosity), "10");
  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (combo_verbosity))),"0");

  label_remotedump = gtk_label_new ( gettext ("Remote Dump File: ")); 
  entry_remotedump = gtk_entry_new ();
  
  button_localdump = gtk_button_new_with_label ( gettext("Local Dump File"));
  entry_localdump = gtk_entry_new ();

  checkbutton_adddate = gtk_check_button_new_with_label ( gettext ("Add date to local dump filename? "));

  /* End Server Options Widgets */

  /* Audio Options Widgets */
  label_format = gtk_label_new ( gettext("Format: "));
  combo_format = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_format), gettext("mp3"));
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_format), gettext("ogg/vorbis"));
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_format), gettext("aac"));
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_format), gettext("mp2"));
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_format), gettext("aacp"));
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_format), gettext("ogg/opus"));
  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (combo_format))),"ogg/vorbis");

  label_bitratemode = gtk_label_new ( gettext("BitRateMode: "));
  combo_bitratemode = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitratemode), gettext("Constant"));
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitratemode), gettext("Average"));
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitratemode), gettext("Variable"));
  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (combo_bitratemode))), gettext("Constant"));

  label_bitrate = gtk_label_new ( gettext("BitRate: "));
  combo_bitrate = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "16");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "24");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "32");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "40");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "48");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "56");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "64");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "72");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "80");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "88");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "96");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "104");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "112");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "120");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "128");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "160");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "192");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "224");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitrate), "256");
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_bitrate))),"128");

  label_samplerate = gtk_label_new ( gettext("SampleRate: "));
  combo_samplerate = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "8000");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "11025");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "16000");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "22050");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "24000");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "32000");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "44100");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "48000");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_samplerate), "96000");
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_samplerate))),"48000");

  label_quality = gtk_label_new ( gettext("Quality: "));
  combo_quality = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "1.0");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.9");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.8");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.7");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.6");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.5");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.4");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.3");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.2");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_quality),  "0.1");
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_quality))),"0.8");

  label_bitspersample = gtk_label_new ( gettext("Bits per Sample: "));
  combo_bitspersample = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitspersample),  "16");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_bitspersample),  "8");
  gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (combo_bitspersample))),"16");

  label_channel = gtk_label_new ( gettext("Number of channels: "));
  combo_channel = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_channel),  "2 - Stereo");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_channel), "1 - Mono");
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_channel))),"1 - Mono");

  label_buffersize = gtk_label_new ( gettext ("Buffer Size: "));
  combo_buffersize = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "1");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "2");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "3");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "4");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "5");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "6");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "7");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "8");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "9");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "10");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "11");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "12");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "13");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "14");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "15");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "16");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "17");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "18");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "19");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_buffersize), "20");
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_buffersize))),"10");

  label_device = gtk_label_new ( gettext ("Device Input: "));
  combo_device = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "jack");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "jack_auto");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "default");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "pulseaudio");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "/dev/dsp");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "/dev/dsp0");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "/dev/dsp1");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "/dev/dsp2");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "/dev/dsp3");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "hw:0,0");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "hw:0,1");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "hw:0,2");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "hw:1,0");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "hw:1,1");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_device), "hw:1,2");
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_device))),"/dev/dsp");

  /* End Audio Options Widgets */
  
  /* configuration widgets stuff - label + entry */
  label_radioname = gtk_label_new ( gettext("Radio Name: "));
  entry_radioname = gtk_entry_new ();
  label_url = gtk_label_new ( gettext("URL: "));
  entry_url = gtk_entry_new ();
  label_description = gtk_label_new ( gettext("Description: "));
  entry_description = gtk_entry_new ();
  label_genre = gtk_label_new ( gettext("Genre: "));
  entry_genre = gtk_entry_new ();
  
  /* configuration widgets stuff - label + combo box */

  
  label_public = gtk_label_new ( gettext("Public: "));
  combo_public = gtk_combo_box_text_new_with_entry ();
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_public), "yes");
  gtk_combo_box_text_append_text ( GTK_COMBO_BOX_TEXT (combo_public), "no");
  gtk_entry_set_text ( GTK_ENTRY(gtk_bin_get_child (GTK_BIN (combo_public))),"no");

  /* text view stuff */
  tag = gtk_text_tag_new ("");
  tag_table = gtk_text_tag_table_new();
  gtk_text_tag_table_add(tag_table, tag);
  buffer = gtk_text_buffer_new(tag_table);
  foo = gettext("DarkSnow - A graphical user interface for darkice\nThe Revolution will be streamed!");
  gtk_text_buffer_set_text(buffer, foo,strlen(foo));
  text = gtk_text_view_new_with_buffer (buffer);
  gtk_text_view_set_editable(GTK_TEXT_VIEW (text), FALSE);    

  /* scroll for the textview widget */
  scroll_text = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW (scroll_text), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (scroll_text), text);


  /* the magic :-) */
  gtk_box_pack_end (GTK_BOX (vbox), table, TRUE, TRUE, 5);
  gtk_container_add (GTK_CONTAINER (window), vbox);


  /* set the widgets tooltips */
  set_all_tooltips();
  

  g_signal_connect (G_OBJECT (window), "delete_event",G_CALLBACK (main_quit), NULL);
  
  g_signal_connect_swapped (G_OBJECT (dialog_about), "response",G_CALLBACK (gtk_widget_hide), dialog_about);

  g_signal_connect (G_OBJECT (button_start), "clicked", G_CALLBACK (dark_start), NULL);
  g_signal_connect (G_OBJECT (button_stop), "clicked", G_CALLBACK (dark_stop), NULL);
  g_signal_connect (G_OBJECT (button_detail), "clicked", G_CALLBACK (dark_detail), NULL);
  
  g_signal_connect_swapped (G_OBJECT (menu_item_open), "activate", G_CALLBACK (gtk_dialog_run), file_open);
  g_signal_connect_swapped (G_OBJECT (menu_item_save), "activate", G_CALLBACK (gtk_dialog_run), file_save);
  g_signal_connect (G_OBJECT (menu_item_quit), "activate", G_CALLBACK (main_quit), NULL);
  g_signal_connect (G_OBJECT (menu_item_quit_nd), "activate", G_CALLBACK (delete_event_nd), NULL);
  g_signal_connect_swapped (G_OBJECT (menu_item_about), "activate", G_CALLBACK (gtk_dialog_run), dialog_about);
  g_signal_connect_swapped (G_OBJECT (button_localdump), "clicked", G_CALLBACK (gtk_dialog_run), file_localdump);

  g_signal_connect ( G_OBJECT (GTK_FILE_CHOOSER_DIALOG (file_open)), "response", G_CALLBACK (dark_put_in_box), NULL);
  g_signal_connect ( G_OBJECT (GTK_FILE_CHOOSER_DIALOG (file_save)), "response", G_CALLBACK (dark_write_config), NULL);
  g_signal_connect ( G_OBJECT (GTK_FILE_CHOOSER_DIALOG (file_localdump)), "response", G_CALLBACK (dark_localdump), NULL);

  
  /* pack the notebook, the buttons and textview widgets into the main table */
  gtk_table_attach (GTK_TABLE (table), notebook, 0, SIZE_X, 0, 400, (GTK_FILL | GTK_SHRINK | GTK_EXPAND), (GTK_FILL | GTK_SHRINK), 5, 5);
  gtk_table_attach (GTK_TABLE (table), button_start, 0,  SIZE_X/2,  401,  430, (GTK_SHRINK | GTK_FILL), (GTK_SHRINK | GTK_FILL), 45, 5);
  gtk_table_attach (GTK_TABLE (table), button_stop,  SIZE_X/2+1, SIZE_X, 401,  430, (GTK_SHRINK | GTK_FILL), (GTK_SHRINK | GTK_FILL), 45, 5);
  gtk_table_attach (GTK_TABLE (table), scroll_text, 0, SIZE_X, 431, 560, (GTK_FILL | GTK_SHRINK | GTK_EXPAND), (GTK_FILL | GTK_SHRINK | GTK_EXPAND), 5, 5); 
  gtk_table_attach (GTK_TABLE (table), label_status, 0, 3*SIZE_X/5, 561, 570, (GTK_FILL | GTK_SHRINK), (GTK_FILL | GTK_SHRINK ), 0, 3);  
  gtk_table_attach (GTK_TABLE (table), button_detail, 3*SIZE_X/5+1, SIZE_X, 561, 570, (GTK_FILL | GTK_SHRINK), (GTK_FILL | GTK_SHRINK), 10, 3);  


  /* main notebook tab */
  gtk_table_attach (GTK_TABLE (table_server), label_icecast, 0, SIZE_X/2, 0,  25, (GTK_FILL | GTK_SHRINK), (GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_server), combo_icecast, SIZE_X/2+1, SIZE_X, 0,  25, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_server), label_server,  0,  SIZE_X/2,   26,  50, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_server), entry_server, SIZE_X/2+1, SIZE_X,   26,  50, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_server), label_port, 0, SIZE_X/2, 51, 75, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_server), entry_port, SIZE_X/2+1, SIZE_X, 51, 75, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_server), label_mountpoint, 0, SIZE_X/2, 76, 100, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_server), entry_mountpoint, SIZE_X/2+1, SIZE_X, 76, 100, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_server), label_pass, 0, SIZE_X/2, 101,  125, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_server), entry_pass, SIZE_X/2+1, SIZE_X, 101, 125, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_server), label_verbosity, 0, SIZE_X/2, 126,  150, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_server), combo_verbosity, SIZE_X/2+1, SIZE_X, 126, 150, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_server), label_remotedump, 0, SIZE_X/2, 151,  175, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_server), entry_remotedump, SIZE_X/2+1, SIZE_X, 151, 175, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_server), button_localdump, 0, SIZE_X/2, 176,  200, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_server), entry_localdump, SIZE_X/2+1, SIZE_X, 176, 200, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_server), checkbutton_adddate, 0, SIZE_X, 201,  225, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);




  
  /* audio notebook tab */
  gtk_table_attach (GTK_TABLE (table_audio), label_format, 0, SIZE_X/2, 0,  25, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_format, SIZE_X/2+1, SIZE_X, 0,  25, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_audio), label_bitratemode, 0, SIZE_X/2, 26,  50, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_bitratemode, SIZE_X/2+1, SIZE_X, 26,  50, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_audio), label_bitrate, 0, SIZE_X/2, 51, 75, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_bitrate, SIZE_X/2+1, SIZE_X, 51, 75, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_audio), label_samplerate, 0, SIZE_X/2, 76, 100, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_samplerate, SIZE_X/2+1, SIZE_X, 76, 100, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_audio), label_quality, 0, SIZE_X/2, 101, 125, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_quality, SIZE_X/2+1, SIZE_X, 101, 125, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_audio), label_bitspersample, 0, SIZE_X/2, 126, 150, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_bitspersample, SIZE_X/2+1, SIZE_X, 126, 150, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_audio), label_channel, 0, SIZE_X/2, 151, 175, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_channel, SIZE_X/2+1, SIZE_X, 151, 175, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);

  gtk_table_attach (GTK_TABLE (table_audio), label_buffersize, 0, SIZE_X/2, 176, 200, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_buffersize, SIZE_X/2+1, SIZE_X, 176, 200, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_audio), label_device, 0, SIZE_X/2, 201, 225, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_audio), combo_device, SIZE_X/2+1, SIZE_X, 201, 225, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);


  /* description notebook tab */
  gtk_table_attach (GTK_TABLE (table_description), label_radioname, 0, SIZE_X/2, 0, 25, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_description), entry_radioname, SIZE_X/2+1, SIZE_X, 0, 25, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_description), label_description, 0, SIZE_X/2, 26, 50, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_description), entry_description, SIZE_X/2+1, SIZE_X, 26, 50, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_description), label_url, 0, SIZE_X/2, 51,  75, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_description), entry_url, SIZE_X/2+1, SIZE_X, 51,  75, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_description), label_genre, 0, SIZE_X/2, 76,  100, (GTK_FILL |  GTK_SHRINK),(GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_description), entry_genre, SIZE_X/2+1, SIZE_X, 76,  100, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  
  gtk_table_attach (GTK_TABLE (table_description), label_public, 0, SIZE_X/2, 101,  125, (GTK_FILL |  GTK_SHRINK), (GTK_FILL | GTK_SHRINK), 0, 5);
  gtk_table_attach (GTK_TABLE (table_description), combo_public, SIZE_X/2+1, SIZE_X, 101, 125, (GTK_FILL |  GTK_SHRINK | GTK_EXPAND),(GTK_FILL | GTK_SHRINK), 10, 5);
  

  
  
  /* show all widgets */ 
  gtk_widget_show (button_start);
  gtk_widget_show (button_stop);
  gtk_widget_show (button_detail);
  gtk_widget_show (label_status);

  
  gtk_widget_show (label_icecast);
  gtk_widget_show (combo_icecast);
  
  gtk_widget_show (label_server);
  gtk_widget_show (entry_server);
  
  gtk_widget_show (label_port);
  gtk_widget_show (entry_port);
  
  gtk_widget_show (label_mountpoint);
  gtk_widget_show (entry_mountpoint);

  gtk_widget_show (label_pass);
  gtk_widget_show (entry_pass);

  gtk_widget_show (label_remotedump);
  gtk_widget_show (entry_remotedump);

  gtk_widget_show (button_localdump);
  gtk_widget_show (entry_localdump);
  
  gtk_widget_show (checkbutton_adddate);
  
  gtk_widget_show (label_format);
  gtk_widget_show (combo_format);
  
  gtk_widget_show (label_bitratemode);
  gtk_widget_show (combo_bitratemode);
  
  gtk_widget_show (label_bitrate);
  gtk_widget_show (combo_bitrate);

  gtk_widget_show (label_samplerate);
  gtk_widget_show (combo_samplerate);

  gtk_widget_show (label_quality);
  gtk_widget_show (combo_quality);

  gtk_widget_show (label_bitspersample);
  gtk_widget_show (combo_bitspersample);

  gtk_widget_show (label_channel);
  gtk_widget_show (combo_channel);

  gtk_widget_show (label_buffersize);
  gtk_widget_show (combo_buffersize);

  gtk_widget_show (label_device);
  gtk_widget_show (combo_device);

  gtk_widget_show (label_verbosity);
  gtk_widget_show (combo_verbosity);
  
  gtk_widget_show (label_radioname);
  gtk_widget_show (entry_radioname);
  
  gtk_widget_show (label_description);
  gtk_widget_show (entry_description);
  
  gtk_widget_show (label_url);
  gtk_widget_show (entry_url);
  
  gtk_widget_show (label_genre);
  gtk_widget_show (entry_genre);
    
  gtk_widget_show (label_public);
  gtk_widget_show (combo_public);
 
  /*  gtk_widget_show (text);
      gtk_widget_show (scroll_text);*/

  gtk_widget_show (table_server);
  gtk_widget_show (table_audio);
  gtk_widget_show (table_description);
  
  gtk_widget_show (label_notebook_server);
  gtk_widget_show (label_notebook_audio);
  gtk_widget_show (label_notebook_description);

  gtk_widget_show (notebook);

  gtk_widget_show (menu_bar);
  gtk_widget_show (menu_item_file);
  gtk_widget_show (menu_item_open);
  gtk_widget_show (menu_item_save);
  gtk_widget_show (menu_item_quit);
  gtk_widget_show (menu_item_quit_nd);
  gtk_widget_show (menu_item_help);
  gtk_widget_show (menu_item_about);

  gtk_widget_show (table);

  gtk_widget_show (vbox);
  
  gtk_widget_show (window);
  
  gtk_main ();

  close (darkice_fd);

  return 0;
}
