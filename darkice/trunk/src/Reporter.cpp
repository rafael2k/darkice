/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell Reporter

   File     : Reporter.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Copyright notice:

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License  
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#include <iostream>

#include "Reporter.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  Initial values for static members of the class
 *----------------------------------------------------------------------------*/
unsigned int    Reporter::verbosity = 1;
std::ostream  * Reporter::os        = &std::cout;


/* ===============================================  local function prototypes */


/* =============================================================  module code */

    

/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2002/05/28 12:35:41  darkeye
  code cleanup: compiles under gcc-c++ 3.1, using -pedantic option

  Revision 1.1  2000/11/16 08:48:43  darkeye
  added multiple verbosity-level event reporting and verbosity command
  line option


  
------------------------------------------------------------------------------*/

