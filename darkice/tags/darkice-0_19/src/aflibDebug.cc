    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "aflibDebug.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static int aflib_debug_level = ::aflibDebug::lInfo;
static bool aflib_debug_abort = false;
static const char *aflib_debug_prefix = "";
static char *messageAppName = 0;


/*
static char* status_strs[] =
{
   "Success",
   "Error Open",
   "Unsupported",
   "AFLIB_ERROR_INITIALIZATION_FAILURE",
   "AFLIB_NOT_FOUND",
   "AFLIB_END_OF_FILE",
   "AFLIB_NO_DATA",
	0
};


static char* data_size_strs[] =   
{
   "UNDEFINED",
   "8 bit signed",
   "8 bit unsigned",
   "16 bit signed",
   "16 bit unsigned",
   "32 bit signed",
   "32 bit unsigned",
	0
};

static char* data_endian_strs[] =
{
   "UNDEFINED",
   "ENDIAN_LITTLE",
   "ENDIAN_BIG",
	0
};
*/

/*
 * Call the graphical application to display a message, if
 * defined. Otherwise, send to standard error. Debug messages are
 * always sent to standard error because they tend to be very verbose.
 * Note that the external application is run in the background to
 * avoid blocking the sound server.
 */
void output_message(::aflibDebug::Level level, const char *msg) {
   char buff[1024];

   /* default to text output if no message app is defined or if it is a debug message. */
	if (messageAppName == 0 || !strcmp(messageAppName, "") || (level == ::aflibDebug::lDebug))
	{
		fprintf(stderr, "%s\n", msg);
		return;
	}

	switch (level) {
		case ::aflibDebug::lFatal:
		  sprintf(buff, "%s -e \"aflib fatal error:\n\n%s\" &", messageAppName, msg);
		  break;
		case ::aflibDebug::lWarning:
		  sprintf(buff, "%s -w \"aflib warning message:\n\n%s\" &", messageAppName, msg);
		  break;
		case ::aflibDebug::lInfo:
		  sprintf(buff, "%s -i \"aflib informational message:\n\n%s\" &", messageAppName, msg);
		  break;
	  default:
		  break; // avoid compile warning
	}
	system(buff);
}

/*
 * Display a message using output_message. If the message is the same
 * as the previous one, just increment a count but don't display
 * it. This prevents flooding the user with duplicate warnings. If the
 * message is not the same as the previous one, then we report the
 * previously repeated message (if any) and reset the last message and
 * count.
 */
void display_message(::aflibDebug::Level level, const char *msg) {
  static char lastMsg[1024];
  static ::aflibDebug::Level lastLevel;
  static int msgCount = 0;

	if (!strncmp(msg, lastMsg, 1024))
	{
		msgCount++;
	} else {
		if (msgCount > 0)
		{
			char buff[1024];
			sprintf(buff, "%s\n(The previous message was repeated %d times.)", lastMsg, msgCount);
			output_message(lastLevel, buff);
		}
	
		strncpy(lastMsg, msg, 1024);
		lastLevel = level;
		msgCount = 0;
		output_message(level, msg);
	}

}

static class DebugInitFromEnv {

   public:
   DebugInitFromEnv() {
		const char *env = getenv("AFLIB_DEBUG");
      if(env)
      {
         if(strcmp(env,"debug") == 0)
	    		aflib_debug_level = ::aflibDebug::lDebug;
         else if(strcmp(env,"info") == 0)
            aflib_debug_level = ::aflibDebug::lInfo;
         else if(strcmp(env,"warning") == 0)
            aflib_debug_level = ::aflibDebug::lWarning;
         else if(strcmp(env,"quiet") == 0)
            aflib_debug_level = ::aflibDebug::lFatal;
         else
         {
            fprintf(stderr,
	       "AFLIB_DEBUG must be one of debug,info,warning,quiet\n");
         }
      }

      env = getenv("AFLIB_DEBUG_ABORT");
      if(env)
         aflib_debug_abort = true;
   }
} 

debugInitFromEnv;


void aflibDebug::init(const char *prefix, Level level)
{
   aflib_debug_level = level;
   aflib_debug_prefix = prefix;
}

void aflibDebug::fatal(const char *fmt, ...)
{
   char buff[1024];
   va_list ap;
   va_start(ap, fmt);
   vsprintf(buff, fmt, ap);
   va_end(ap);
   display_message(::aflibDebug::lFatal, buff);

   if(aflib_debug_abort) abort();
   exit(1);
}

void aflibDebug::warning(const char *fmt, ...)
{
   if(lWarning >= aflib_debug_level)
   {
      char buff[1024];
      va_list ap;
      va_start(ap, fmt);
      vsprintf(buff, fmt, ap);
      va_end(ap);
      display_message(::aflibDebug::lWarning, buff);
   }
}

void aflibDebug::info(const char *fmt, ...)
{
   if(lInfo >= aflib_debug_level)
   {
      char buff[1024];
      va_list ap;
      va_start(ap, fmt);
      vsprintf(buff, fmt, ap);
      va_end(ap);
      display_message(::aflibDebug::lInfo, buff);
   }
}

void aflibDebug::debug(const char *fmt, ...)
{
   if(lDebug >= aflib_debug_level)
   {
      char buff[1024];
      va_list ap;
      va_start(ap, fmt);
      vsprintf(buff, fmt, ap);
      va_end(ap);
      display_message(::aflibDebug::lDebug, buff);
   }
}

void aflibDebug::messageApp(const char *appName)
{
   messageAppName = (char*) realloc(messageAppName, strlen(appName)+1);
   strcpy(messageAppName, appName);
}
