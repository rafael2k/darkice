/*
 * Copyright: (C) 2000 Julius O. Smith
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 *   Julius O. Smith  jos@ccrma.stanford.edu
 *
 */
/* This code was modified by Bruce Forsberg (forsberg@adnc.com) to make it
   into a C++ class
*/


#ifndef _AFLIBCONVERTER_H_
#define _AFLIBCONVERTER_H_

#ifndef MAX
#define MAX(x,y) ((x)>(y) ?(x):(y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y) ?(x):(y))
#endif

#define MAX_HWORD (32767)
#define MIN_HWORD (-32768)

typedef char           BOOL;
typedef short          HWORD;
typedef unsigned short UHWORD;
typedef int            WORD;
typedef unsigned int   UWORD;
#define IBUFFSIZE 4096                         /* Input buffer size */

/*! \class aflibConverter
    \brief Provides sample rate conversion.

 This class will perform audio resampling. With the constructor you can choose the
 type of resampling to be done. Simple linear interpolation can be done by setting
 linear_interpolation to be TRUE in the constructor. The other two flags are 
 ignored if this is set. If linear_interpolation is FALSE then some form of filtering
 will be done. IF high_quality is FALSE then a small filter will be performed. 
 If high_quality is TRUE then a large filter (higher quality) will be performed. For
 both the small and large filters another parameter can be specified, filter_interpolation.
 With filter_interpolation set then the filter coefficients used for both the small and
 large filtering will be interpolated as well.

 This class was designed to stream audio data. It also expects audio data as 16 bit values.
 Each time a new stream is started some initialization needs to be done. Thus the function
 initialize should be called to initialize everything. This class will work on any
 number of channels. Once everything is specified then resample should be called as many
 times as is necessary to process all the data. The value inCount will be returned
 indicating how many inArray samples were actually used to produce the output. This
 value can be used to indicate where the next block of inArray data should start. The
 resample function is driven by the outCount value specified. The inArray should
 contain at least:
    outCount / factor + extra_samples.
 extra_samples depends on the type of filtering done. As a rule of thumb 50 should be
 adequate for any type of filter.
*/


class aflibConverter {

public:

   // Available contructors and destructors
   aflibConverter (
      bool  high_quality,
      bool  linear_interpolation,
      bool  filter_interpolation);

   ~aflibConverter();

   void
   initialize(
      double factor,   /* factor = Sndout/Sndin */
      int    channels);/* number of sound channels */

   int
   resample(           /* number of output samples returned */
      int& inCount,    /* number of input samples to convert */
      int outCount,    /* number of output samples to compute */
      HWORD inArray[], /* input array data (length inCount * nChans) */
      HWORD outArray[]);/* output array data (length outCount * nChans) */
 
 
private:

   aflibConverter();

   aflibConverter(const aflibConverter& op);

   const aflibConverter&
   operator=(const aflibConverter& op);

   int
   err_ret(char *s);

   void
   deleteMemory();

   int
   readData(
      int   inCount,       /* _total_ number of frames in input file */
      HWORD inArray[],     /* input data */
      HWORD *outPtr[],     /* array receiving chan samps */
      int   dataArraySize, /* size of these arrays */
      int   Xoff,          /* read into input array starting at this index */
      bool  init_count);


   inline HWORD
   WordToHword(WORD v, int scl)
   {
       HWORD out;
       WORD llsb = (1<<(scl-1));
       v += llsb;          /* round */
       v >>= scl;
       if (v>MAX_HWORD) {
#ifdef DEBUG
           if (pof == 0)
             fprintf(stderr, "*** resample: sound sample overflow\n");
           else if ((pof % 10000) == 0)
             fprintf(stderr, "*** resample: another ten thousand overflows\n");
           pof++;
#endif
           v = MAX_HWORD;
       } else if (v < MIN_HWORD) {
#ifdef DEBUG
           if (nof == 0)
             fprintf(stderr, "*** resample: sound sample (-) overflow\n");
           else if ((nof % 1000) == 0)
             fprintf(stderr, "*** resample: another thousand (-) overflows\n");
           nof++;
#endif
           v = MIN_HWORD;
       }
       out = (HWORD) v;
       return out;
   };

   int
   SrcLinear(
      HWORD X[],
      HWORD Y[],
      double factor,
      UWORD *Time,
      UHWORD& Nx,
      UHWORD Nout);

   int
   SrcUp(
      HWORD X[],
      HWORD Y[],
      double factor,
      UWORD *Time,
      UHWORD& Nx,
      UHWORD Nout,
      UHWORD Nwing,
      UHWORD LpScl,
      HWORD Imp[],
      HWORD ImpD[],
      BOOL Interp);

   int
   SrcUD(
      HWORD X[],
      HWORD Y[],
      double factor,
      UWORD *Time,
      UHWORD& Nx,
      UHWORD Nout,
      UHWORD Nwing,
      UHWORD LpScl,
      HWORD Imp[],
      HWORD ImpD[],
      BOOL Interp);

   WORD
   FilterUp(
      HWORD Imp[],
      HWORD ImpD[],
      UHWORD Nwing,
      BOOL Interp,
      HWORD *Xp,
      HWORD Ph,
      HWORD Inc);

   WORD
   FilterUD(
      HWORD Imp[],
      HWORD ImpD[],
      UHWORD Nwing,
      BOOL Interp,
      HWORD *Xp,
      HWORD Ph,
      HWORD Inc,
      UHWORD dhb);

   int
   resampleFast(  /* number of output samples returned */
      int& inCount,     /* number of input samples to convert */
      int outCount,    /* number of output samples to compute */
      HWORD inArray[], /* input array data (length inCount * nChans) */
      HWORD outArray[]);/* output array data (length outCount * nChans) */
 
   int
   resampleWithFilter(  /* number of output samples returned */
      int& inCount,      /* number of input samples to convert */
      int outCount,     /* number of output samples to compute */
      HWORD inArray[],  /* input array data (length inCount * nChans) */
      HWORD outArray[], /* output array data (length outCount * nChans) */
      HWORD Imp[], HWORD ImpD[],
      UHWORD LpScl, UHWORD Nmult, UHWORD Nwing);


static HWORD SMALL_FILTER_IMP[];
static HWORD LARGE_FILTER_IMP[];

bool    interpFilt;
bool    largeFilter;
bool    linearInterp;
HWORD  ** X;
HWORD  ** Y;
UWORD   Time;
double  factor;
int     nChans;
bool    initial;

};


#endif
