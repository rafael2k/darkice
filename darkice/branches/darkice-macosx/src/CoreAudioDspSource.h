#ifndef COREAUDIO_DSP_SOURCE_H
#define COREAUDIO_DSP_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */


#include "Reporter.h"
#include "AudioSource.h"

#if defined( HAVE_COREAUDIO_LIB )
#include <jack/ringbuffer.h>
#include <CoreAudio/CoreAudio.h>
#else
#error configure for CoreAudio
#endif


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An audio input based on CoreAudio
 *
 *  @author  $Author:  $
 *  @version $Revision:  $
 */
class CoreAudioDspSource : public AudioSource, public virtual Reporter
{
    private:
        /**
         *  The device ID
         */
        AudioDeviceID                device_id;

        /**
         *  The ring buffer.
         */
        jack_ringbuffer_t            * rb[2];

        /**
         *  The audio sample buffer.
         */
        char                         * tmp_buffer;

        /**
         *  The audio format convarting buffer.
         */
        char                         * cnv_buffer;
        
         /**
         *  Device status whether running or not.
         */
        bool                is_running;

         /**
         *  Device status whether be opened or not.
         */
        bool                is_opened;

         /**
         *  Stream description.
         */
        AudioStreamBasicDescription stream_description;

    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        CoreAudioDspSource ( void )                       throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Initialize the object
         *
         *  @exception Exception
         */
        void
        init ( const char* name )                   throw ( Exception );

        /**
         *  De-initialize the object
         *
         *  @exception Exception
         */
        void
        strip ( void )                              throw ( Exception );

        static OSStatus
        callback_handler( AudioDeviceID inDevice,
                          const AudioTimeStamp *inNow,
                          const AudioBufferList *inInputData,
                          const AudioTimeStamp *inInputTime,
                          AudioBufferList *outOutputData,
                          const AudioTimeStamp *inOutputTime, 
                          void *infoPointer );
        
        static OSStatus
        device_listener( AudioDeviceID inDevice,
                         UInt32 channel,
                         Boolean isInput,
                         AudioDevicePropertyID propertyID,
                         void* handlePointer);
        
    private:
        /**
         */
        bool
        startDevice(void);

        /**
         */
        bool
        stopDevice(void);

    public:

        /**
         *  Constructor.
         *
         *  @param name the name of the device
         *  @param sampleRate samples per second (e.g. 44100 for 44.1kHz).
         *  @param bitsPerSample bits per sample (e.g. 16 bits).
         *  @param channels number of channels of the audio source
         *                 (e.g. 1 for mono, 2 for stereo, etc.).
         *  @exception Exception
         */
        inline
        CoreAudioDspSource ( const char    * name,
                             int             sampleRate    = 44100,
                             int             bitsPerSample = 16,
                             int             channels      = 2 )
                                                           throw ( Exception )

                    : AudioSource( sampleRate, bitsPerSample, channels )
        {
            init( name );
        }

        /**
         *  Copy Constructor.
         *
         *  @param cds the object to copy.
         *  @exception Exception
         */
        inline
        CoreAudioDspSource (  const CoreAudioDspSource &    cds )   throw ( Exception )
                    : AudioSource( cds )
        {
            throw Exception( __FILE__, __LINE__, "CoreAudioDspSource doesn't copy");
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~CoreAudioDspSource ( void )                          throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param ds the object to assign to this one.
         *  @return a reference to this object.
         *  @exception Exception
         */
        inline virtual CoreAudioDspSource &
        operator= (     const CoreAudioDspSource &     ds )   throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__, "CoreAudioDspSource doesn't assign");
        }

        /**
         *  Tell if the data from this source comes in big or little endian.
         *
         *  @return true if the source is big endian, false otherwise
         */
        virtual bool
        isBigEndian ( void ) const           throw ();

        /**
         *  Open the CoreAudioDspSource.
         *  This does not put the device into recording mode.
         *  To start getting samples, call either canRead() or read().
         *
         *  @return true if opening was successful, false otherwise
         *  @exception Exception
         *  
         *  @see #canRead
         *  @see #read
         */
        virtual bool
        open ( void )                                   throw ( Exception );

        /**
         *  Check if the CoreAudioDspSource is registered
         *
         *  @return true if device is setup
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return is_opened;
        }

        /**
         *  Check if the CoreAudioDspSource can be read from.
         *  Blocks until the specified time for data to be available.
         *  Puts the CoreAudio device into recording mode.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the CoreAudioDspSource is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  throw ( Exception );

        /**
         *  Read from the CoreAudioDspSource.
         *  Puts the CoreAudio device into recording mode.
         *
         *  @param buf the buffer to read into.
         *  @param len the number of bytes to read into buf
         *  @return the number of bytes read (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        read (                  void          * buf,
                                unsigned int    len )   throw ( Exception );

        /**
         *  Close the CoreAudioDspSource.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif /* COREAUDIO_DSP_SOURCE_H */
