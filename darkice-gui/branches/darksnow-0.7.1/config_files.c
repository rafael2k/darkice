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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif


#ifndef __GLOBAL_H__
#include "global.h"
#endif

#ifndef __CONSTANTS_H__
#include "constants.h"
#endif

#ifndef __CONFIG_FILES_H__
#include "config_files.h"
#endif

gboolean darksnow_config_store(char opt, char *save_path) {
  int i;
  char mp[128];
  char *server;
  char *port;
  char *mountpoint;
  char *pass;
  char *remotedump;
  char *localdump;
  char *radioname;
  char adddate;
  char *description;
  char *url;
  char *genre;
  char *icecast;
  char *format;
  char *bitrate;
  char *bitratemode;
  char *samplerate;
  char *quality;
  char *bitspersample;
  char *channel;
  char *buffersize;
  char *device;
  char *public;
  char *verbosity;

  FILE *f_darksnow_cfg;
  
  /* Entry inputs */
  server = (char *) gtk_entry_get_text ( (GtkEntry *) entry_server);
  port = (char *) gtk_entry_get_text (( GtkEntry *) entry_port);
  mountpoint = (char *) gtk_entry_get_text ( (GtkEntry *) entry_mountpoint);
  pass = (char *) gtk_entry_get_text ( (GtkEntry *) entry_pass);
  radioname = (char *) gtk_entry_get_text ( (GtkEntry *) entry_radioname);
  description = (char *) gtk_entry_get_text ( (GtkEntry *) entry_description);
  url = (char *) gtk_entry_get_text ( (GtkEntry *) entry_url);
  genre = (char *) gtk_entry_get_text ( (GtkEntry *) entry_genre);
  remotedump = (char *) gtk_entry_get_text ( (GtkEntry *) entry_remotedump);
  localdump = (char *) gtk_entry_get_text ( (GtkEntry *) entry_localdump);
  
  adddate = (char) (gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON (checkbutton_adddate) ))? '1': '0';
  
  /* Combo box inputs */
  icecast = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_icecast));
  format = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_format));
  bitrate = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_bitrate));
  bitratemode = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_bitratemode));
  samplerate = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_samplerate));
  quality = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_quality));
  bitspersample = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_bitspersample));
  channel = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_channel));
  buffersize = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_buffersize));
  device = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_device));
  public = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_public));
  verbosity = gtk_combo_box_text_get_active_text  ( GTK_COMBO_BOX_TEXT(combo_verbosity));
  

  if (!( f_darksnow_cfg = (opt != 0)? fopen(darksnow_cfg, "w") : fopen(save_path, "w"))) {
      printf("Error: Cannot open %s\n", (opt != 0)? darksnow_cfg: save_path);
      return FALSE;
  }

  /* for compatibility issues, mountpoint can start with slash */
  i = 0;
  while( mountpoint[i] == ' ' || mountpoint[i] == '/' ) i++;
  strcpy (mp, mountpoint+i);
  

  /* Entry writes */
  fprintf (f_darksnow_cfg, "server=%s\n", server);
  fprintf (f_darksnow_cfg, "port=%s\n", port);
  fprintf (f_darksnow_cfg, "mountpoint=%s\n", mp);
  fprintf (f_darksnow_cfg, "pass=%s\n", pass);
  fprintf (f_darksnow_cfg, "radioname=%s\n", radioname);
  fprintf (f_darksnow_cfg, "description=%s\n", description);
  fprintf (f_darksnow_cfg, "url=%s\n", url);
  fprintf (f_darksnow_cfg, "genre=%s\n", genre);
  fprintf (f_darksnow_cfg, "icecast=%s\n", icecast);
  fprintf (f_darksnow_cfg, "format=%s\n", format);
  fprintf (f_darksnow_cfg, "bitrate=%s\n", bitrate);
  fprintf (f_darksnow_cfg, "bitratemode=%s\n", bitratemode);
  fprintf (f_darksnow_cfg, "samplerate=%s\n", samplerate);
  fprintf (f_darksnow_cfg, "quality=%s\n", quality);
  fprintf (f_darksnow_cfg, "bitspersample=%s\n", bitspersample);
  fprintf (f_darksnow_cfg, "channel=%s\n", channel);
  fprintf (f_darksnow_cfg, "buffersize=%s\n", buffersize);
  fprintf (f_darksnow_cfg, "device=%s\n", device);
  fprintf (f_darksnow_cfg, "public=%s\n", public);
  fprintf (f_darksnow_cfg, "verbosity=%s\n", verbosity);
  fprintf (f_darksnow_cfg, "remotedump=%s\n", remotedump);
  fprintf (f_darksnow_cfg, "localdump=%s\n", localdump);
  fprintf (f_darksnow_cfg, "adddate=%c\n", adddate);

  fclose (f_darksnow_cfg);
  
  return TRUE;
}

gboolean darksnow2darkice_cfg() {
  FILE *f_darksnow_cfg;
  FILE *f_darkice_cfg;
  char destination = 0;
  int i;

  /* strings section */
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

  if (!(f_darksnow_cfg = fopen(darksnow_cfg, "r"))) {
    printf("Error: Cannot open %s\n", darksnow_cfg);
    exit (-1);
  }

  if (!(f_darkice_cfg = fopen(darkice_cfg, "w"))) {
    printf("Error: Cannot open %s\n", darkice_cfg);
    exit (-1);
  }
  
  /* read de configuration from darksnow config file */
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
  fscanf(f_darksnow_cfg, "%*[^=]=%*[^\n]\n"); /* the verbosity level is not needed */ 
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, remotedump);
  fscanf(f_darksnow_cfg, "%[^=]=%[^\n]\n", foo, localdump);
  fscanf(f_darksnow_cfg, "%[^=]=%c\n", foo, &adddate);

  /* set destination flag - 0-> icecast1, 1->icecast2, 2->shoutcast, 3->file */
  if (!strcmp("Icecast 1", icecast)) 
    destination = 0;
  else if (!strcmp("Icecast 2", icecast))
    destination = 1;
  else if (!strcmp("Shoutcast", icecast))
    destination = 2;
  /* File output support not yet codded */
  else if (!strcmp("File", icecast))
    destination = 3;

  /* write the config file for darkice */

  /* the reader of the config file */
  fprintf(f_darkice_cfg, "[general]");
  fprintf(f_darkice_cfg, "\nduration = ");  
  fprintf(f_darkice_cfg, "0");        /*  duration of encoding, in seconds. 0 means forever */
  fprintf(f_darkice_cfg, "\nbufferSecs = ");
  fprintf(f_darkice_cfg, "%s", buffersize);        /* size of internal slip buffer, in seconds */
  fprintf(f_darkice_cfg, "\n\n[input]");
  fprintf(f_darkice_cfg, "\ndevice = ");
  fprintf(f_darkice_cfg, "%s", device); /* OSS, ALSA, Pulse or JACK */
  if (!strcmp(device, "pulseaudio")) {
      fprintf(f_darkice_cfg, "\npaSourceName = default");
  }
  fprintf(f_darkice_cfg, "\nsampleRate = "); 
  fprintf(f_darkice_cfg, "%s", samplerate); /* sample rate in Hz. try 11025, 22050 or 44100 */
  fprintf(f_darkice_cfg, "\nbitsPerSample = ");
  fprintf(f_darkice_cfg, "%s", bitspersample); /* bits per sample. try 16 */

  fprintf(f_darkice_cfg, "\nchannel = "); /* channels. 1 = mono, 2 = stereo */
  if (!strcmp(channel, "1 - Mono"))
    fprintf(f_darkice_cfg, "1"); 
  else
    fprintf(f_darkice_cfg, "2");

  /* destionation type */
  switch (destination) {
  case 0:
    fprintf(f_darkice_cfg, "\n\n[icecast-0]"); /* icecast 1 version server */
    break;
  case 1:
    fprintf(f_darkice_cfg, "\n\n[icecast2-0]"); /* icecast 2 version server */
    break;
  case 2:
    fprintf(f_darkice_cfg, "\n\n[shoutcast-0]"); /* shoutcast server */ /* FIXME - not yet implemented */
    break;
  case 3:
    fprintf(f_darkice_cfg, "\n\n[file-0]"); /* file output */ /* FIXME - not yet implemented */
    break;
  }

  /* write bitratemode */
  fprintf(f_darkice_cfg, "\nbitrateMode = ");
  if (!strcmp(gettext("Constant"), bitratemode))
    fprintf(f_darkice_cfg, "cbr"); /* Constant bitrate mode */
  else 
    if (!strcmp(gettext("Average"), bitratemode))
      fprintf(f_darkice_cfg, "abr"); /* average bitrate mode */
  else 
    if (!strcmp(gettext("Variable"), bitratemode)) 
      fprintf(f_darkice_cfg, "vbr"); /* variable bitrate mode */
  

  /* write format */
  if (destination == 1) {
    fprintf(f_darkice_cfg, "\nformat = ");
    if (!strcmp( gettext("mp3"), format))
      fprintf(f_darkice_cfg, "mp3"); 
    else if (!strcmp( gettext("ogg/vorbis"), format))
      fprintf(f_darkice_cfg, "vorbis"); 
    else if (!strcmp( gettext("ogg/opus"), format))
      fprintf(f_darkice_cfg, "opus"); 
    else if (!strcmp( gettext("aac"), format))
      fprintf(f_darkice_cfg, "aac");
    else if (!strcmp( gettext("mp2"), format))
      fprintf(f_darkice_cfg, "mp2");
    else if (!strcmp( gettext("aacp"), format))
      fprintf(f_darkice_cfg, "aacp");
  }
  
  fprintf(f_darkice_cfg, "\nbitrate = ");
  fprintf(f_darkice_cfg, "%s", bitrate); /* bitrate of the stream sent to the server */
  fprintf(f_darkice_cfg, "\nquality = ");
  fprintf(f_darkice_cfg, "%s", quality); /*  encoding quality */ 
  fprintf(f_darkice_cfg, "\nserver = ");
  fprintf(f_darkice_cfg, "%s", server);
  fprintf(f_darkice_cfg, "\nport = ");
  fprintf(f_darkice_cfg, "%s", port); /* port of the IceCast2 server, usually 8000 */
  fprintf(f_darkice_cfg, "\npassword = ");
  fprintf(f_darkice_cfg, "%s", pass); /* source password to the IceCast2 server */
  fprintf(f_darkice_cfg, "\nmountPoint = "); 
  fprintf(f_darkice_cfg, "%s", mountpoint);  /* mount point of this stream on the IceCast2 server */
  fprintf(f_darkice_cfg, "\nname = "); 
  fprintf(f_darkice_cfg, "%s", radioname); /* name of the stream */
  fprintf(f_darkice_cfg, "\ndescription = "); 
  fprintf(f_darkice_cfg, "%s", description); /* description of the stream */
  fprintf(f_darkice_cfg, "\nurl = ");
  fprintf(f_darkice_cfg, "%s", url); /* URL related to the stream */
  fprintf(f_darkice_cfg, "\ngenre = ");
  fprintf(f_darkice_cfg, "%s", genre); /* genre of the stream */
  fprintf(f_darkice_cfg, "\npublic = ");
  if (!strcmp( gettext("yes"), public))
    fprintf(f_darkice_cfg, "yes"); /* list this stream */
  else 
    fprintf(f_darkice_cfg, "no"); /* do not list this stream */

  for (i = 0; remotedump[i] != 0 && remotedump[i] == ' '; i++); /* set the remotedumpfile name */
  if (remotedump[i] != 0) {
    fprintf(f_darkice_cfg, "\nremoteDumpFile  = ");
    fprintf(f_darkice_cfg, "%s", remotedump+i);
  }

  for (i = 0; localdump[i] != 0 && localdump[i] == ' '; i++); /* set the localdumpfile name */
  if (localdump[i] != 0) {
    fprintf(f_darkice_cfg, "\nlocalDumpFile  = ");
    fprintf(f_darkice_cfg, "%s", localdump+i);
    fprintf(f_darkice_cfg, "\nfileAddDate  = ");
    fprintf(f_darkice_cfg, "%s", (adddate == '1')? "yes":"no");
  }  

  fprintf(f_darkice_cfg, "\n");

  fclose(f_darksnow_cfg);
  fclose(f_darkice_cfg);


  return TRUE;
}
