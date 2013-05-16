/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell Reporter

   File     : Reporter.h
   Version  : $Revision: 474 $
   Author   : $Author: rafael@riseup.net $
   Location : $HeadURL$
   
   Copyright notice:

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License  
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

------------------------------------------------------------------------------*/
#ifndef REPORTER_H
#define REPORTER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#else
#error need time.h
#endif


#include <iostream>


#include "Exception.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Class for reporting events. All objects of this class share
 *  the same verbosity level. Typical usage is to inherit this class
 *  and use the report() function to report events. Only reports
 *  which are of suffucient importance are really reported.
 *
 *  The default verbosity is 1, and the default ostream is cout.
 * 
 *  Known problems: this class is not thread-safe.
 * 
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class Reporter
{

    private:

        /**
         *  Verbosity level shared among all Reporter objects
         */
        static unsigned int     verbosity;

        /**
         *  The output stream to report to.
         */
        static std::ostream    * os;

        /**
         *  Print timestamp for every report only if verbosity level
         *  is above this value.
         */
        static const unsigned int   prefixVerbosity = 3;

        /**
         *  Print a prefix to each report.
         */
        static void
        printPrefix( void )                         throw ()
        {
            if ( verbosity > prefixVerbosity ) {
                char    str[32];
                time_t  now;

                now = time(NULL);
                strftime( str, 32, "%d-%b-%Y %H:%M:%S ", localtime(&now) );
                (*(Reporter::os)) << str;
            }
        }


    protected:


    public:

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~Reporter ( void )                                  throw ( Exception )
        {
            (Reporter::os)->flush();
        }

        /**
         *  Set the verbosity level. This sets the verbosity for all
         *  Reporter objects.
         *
         *  @param verbosity the new verbosity level.
         */
        static inline void
        setReportVerbosity ( unsigned int     verbosity )   throw ()
        {
            Reporter::verbosity = verbosity;
        }

        /**
         *  Get the verbosity level.
         *
         *  @return the current verbosity level.
         */
        static inline unsigned int
        getReportVerbosity ( void )                         throw ()
        {
            return Reporter::verbosity;
        }

        /**
         *  Set the output stream to report to. This setting effects all
         *  Reporter objects.
         *
         *  @param os the output stream
         */
        static inline void
        setReportOutputStream ( std::ostream  & os )        throw ()
        {
            Reporter::os = &os;
        }

        /**
         *  Get the output stream to report to.
         *
         *  @return the output stream
         */
        static inline std::ostream &
        getReportOutputStream ( void )                      throw ()
        {
            return *(Reporter::os);
        }

        /**
         *  Report an event with a given verbosity.
         *
         *  @param verbosity the importance of the event, with 0 being
         *                   the most important.
         *  @param t the object to report. Must have an
         *           <code>ostream & operator<<( ostream&, const T)</code>
         *           operator overload.
         */
        template<class T>
        static inline void
        reportEvent ( unsigned int  verbosity,
                      const T       t )                     throw ()
        {
            if ( Reporter::verbosity >= verbosity ) {
                printPrefix();
                (*(Reporter::os)) << t <<  std::endl;
            }
        }

        /**
         *  Report an event with a given verbosity.
         *
         *  @param verbosity the importance of the event, with 0 being
         *                   the most important.
         *  @param t the object 1 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const T)</code>
         *           operator overload.
         *  @param u the object 2 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const U)</code>
         *           operator overload.
         */
        template<class T, class U>
        inline void
        static reportEvent ( unsigned int  verbosity,
                      const T       t,
                      const U       u )                     throw ()
        {
            if ( Reporter::verbosity >= verbosity ) {
                printPrefix();
                (*(Reporter::os)) << t << " "
                                  << u <<  std::endl;
            }
        }

        /**
         *  Report an event with a given verbosity.
         *
         *  @param verbosity the importance of the event, with 0 being
         *                   the most important.
         *  @param t the object 1 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const T)</code>
         *           operator overload.
         *  @param u the object 2 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const U)</code>
         *           operator overload.
         *  @param v the object 3 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const V)</code>
         *           operator overload.
         */
        template<class T, class U, class V>
        static inline void
        reportEvent ( unsigned int  verbosity,
                      const T       t,
                      const U       u,
                      const V       v )                     throw ()
        {
            if ( Reporter::verbosity >= verbosity ) {
                printPrefix();
                (*(Reporter::os)) << t << " "
                                  << u << " "
                                  << v <<  std::endl;
            }
        }

        /**
         *  Report an event with a given verbosity.
         *
         *  @param verbosity the importance of the event, with 0 being
         *                   the most important.
         *  @param t the object 1 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const T)</code>
         *           operator overload.
         *  @param u the object 2 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const U)</code>
         *           operator overload.
         *  @param v the object 3 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const V)</code>
         *           operator overload.
         *  @param w the object 4 to report. Must have an
         *           <code>ostream & operator<<( ostream&, const W)</code>
         *           operator overload.
         */
        template<class T, class U, class V, class W>
        static inline void
        reportEvent ( unsigned int  verbosity,
                      const T       t,
                      const U       u,
                      const V       v,
                      const W       w )                     throw ()
        {
            if ( Reporter::verbosity >= verbosity ) {
                printPrefix();
                (*(Reporter::os)) << t << " "
                                  << u << " "
                                  << v << " "
                                  << w <<  std::endl;
            }
        }
};


    

/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* REPORTER_H */

