#include "AudioSource.h"

#ifdef SUPPORT_COREAUDIO_DSP
// only compile this code if there is support for it

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#error need sys/types.h
#endif

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error need math.h
#endif

#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/AudioHardware.h>

#include "Util.h"
#include "Exception.h"
#include "CoreAudioDspSource.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */


/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: CoreAudioDspSource.cpp $";


/* ===============================================  local function prototypes */
static unsigned int get_device_count(void);
static const char* get_error_name( OSStatus code );

/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Tell if source id big endian
 *----------------------------------------------------------------------------*/
bool
CoreAudioDspSource :: isBigEndian ( void ) const                  throw ()
{
    //According to AudioHardware.h, the stream data will
    // always be presented in native-endian format
    CFByteOrder order = CFByteOrderGetCurrent();
    return (CFByteOrderBigEndian == order);
}


/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
CoreAudioDspSource :: init ( const char* name )           throw ( Exception )
{
    // Set defaults
    rb[0]        = NULL;        // Left Ring Buffer
    rb[1]        = NULL;        // Right Ring Buffer
    tmp_buffer   = NULL;        // Buffer big enough for one 'read' of audio
    cnv_buffer   = NULL;        // For converting.
    is_running   = false;       //
    is_opened    = false;

    // Check the sample size
    if (
#if 0
        // don't work.
        getBitsPerSample() != 8 && 
#endif
        getBitsPerSample() != 16) {
        throw Exception( __FILE__, __LINE__,
                        "CoreAudioDspSource doesn't support non 16-bit samples");
    }
    Reporter::reportEvent( 10, "CoreAudioDspSource :: init():bitsPerSample ", getBitsPerSample());

    unsigned int dev_count = get_device_count();
    if (dev_count == 0) {
        throw Exception( __FILE__, __LINE__, "no input device found");
    }

    AudioDeviceID id;
    UInt32 data_size = sizeof(AudioDeviceID);
    OSStatus result = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice,
                                               &data_size, &id);
    if (result != noErr) {
        throw Exception( __FILE__, __LINE__,
                         "faild to get DefaultInputDevice:", get_error_name(result));
    }

    // Use default device ?
    if ( Util::strEq( name, "default", 7) ) {
        device_id = id;
        char dev_name[256];
        data_size = sizeof(dev_name);
        result = AudioDeviceGetProperty(device_id, 0, true,
                                        kAudioDevicePropertyDeviceName, &data_size, &dev_name[0]);
        if (result != noErr) {
            throw Exception( __FILE__, __LINE__,
                             "faild to get device name:", get_error_name(result));
        }
        dev_name[sizeof(dev_name) - 1] = '\0';
        Reporter::reportEvent( 1, "CoreAudioDspSource :: init():name ", &dev_name[0]);
        return;
    }
    
    data_size *= dev_count;
    AudioDeviceID *device_list = (AudioDeviceID *)malloc(dev_count * sizeof(AudioDeviceID));
    if (!device_list) {
        throw Exception( __FILE__, __LINE__, "faild to allocate memory for device_list");
    }
    result = AudioHardwareGetProperty(kAudioHardwarePropertyDevices,
                                      &data_size, device_list);
    if (result != noErr) {
        free(device_list);
        device_list = NULL;
        throw Exception( __FILE__, __LINE__, "faild to get Devices:", get_error_name(result));
    }

    bool dev_found = false;
    for (unsigned int i = 0; i < dev_count; ++i) {
        id = device_list[i];
        char dev_name[256];
        data_size = sizeof(dev_name);
        result = AudioDeviceGetProperty(id, 0, false,
                                        kAudioDevicePropertyDeviceName,
                                        &data_size, &dev_name[0]);
        if (result != noErr) {
            free(device_list);
            device_list = NULL;
            throw Exception( __FILE__, __LINE__, "faild to get Device name:", get_error_name(result));
        }

        if (strcmp(&dev_name[0], name) == 0) {
            device_id = id;
            dev_found = true;

            char dev_name[256];
            data_size = sizeof(dev_name);
            result = AudioDeviceGetProperty(device_id, 0, true,
                                            kAudioDevicePropertyDeviceName, &data_size, &dev_name[0]);
            if (result != noErr) {
                throw Exception( __FILE__, __LINE__,
                                 "faild to get device name:", get_error_name(result));
            }
            dev_name[sizeof(dev_name) - 1] = '\0';
            Reporter::reportEvent( 1, "CoreAudioDspSource :: init():name ", &dev_name[0]);
            break;
        }
    }
    free(device_list);
    device_list = NULL;

    if (!dev_found) {
        throw Exception( __FILE__, __LINE__, "device is not found.");
    }
}



/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
CoreAudioDspSource :: strip ( void )                      throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }
    
    // Free the temporary buffer
    if (tmp_buffer) {
        free(tmp_buffer);
        tmp_buffer = NULL;
    }
    if (cnv_buffer) {
        free(cnv_buffer);
        cnv_buffer = NULL;
    }

}

/*------------------------------------------------------------------------------
 *  Open the audio source
 *----------------------------------------------------------------------------*/
bool
CoreAudioDspSource :: open ( void )                       throw ( Exception )
{
    size_t rb_size = 0;
    unsigned int c = 0;;
    
    if ( isOpen() ) {
        return false;
    }

    // Create a ring buffer for each channel
    rb_size = 8 * getSampleRate() * sizeof(Float32);
    for (c = 0; c < getChannel(); c++) {
        rb[c] = jack_ringbuffer_create(rb_size);
        if (!rb[c]) {
            throw Exception( __FILE__, __LINE__,
                             "Failed to create ringbuffer for channel ", c);
        }
    }

    OSStatus result = noErr;
    UInt32 data_size = 0;;

    // Set BufferFrameSize
    UInt32 buffer_size = 512;
	data_size = sizeof(UInt32);
	result = AudioDeviceSetProperty(device_id, NULL, 0, true,
                                    kAudioDevicePropertyBufferFrameSize, data_size, &buffer_size);
    if (result != noErr) {
        throw Exception( __FILE__, __LINE__,
                         "Can't set buffer frame size:", get_error_name(result));
    }

    data_size = sizeof(UInt32);
	result = AudioDeviceGetProperty(device_id, 0, true,
                                    kAudioDevicePropertyBufferFrameSize, &data_size, &buffer_size);
    if (result != noErr) {
        throw Exception( __FILE__, __LINE__,
                         "Can't get buffer frame size:", get_error_name(result));
    }
    Reporter::reportEvent( 10, "CoreAudioDspSource :: open():buffer frame size: ", buffer_size);

    // Get the stream configration.
    result = AudioDeviceGetPropertyInfo(device_id, 0, true,
                                        kAudioDevicePropertyStreamConfiguration,
                                        &data_size, NULL);
    if (data_size == 0) {
        throw Exception( __FILE__, __LINE__,
                         "Can't get stream configuration ", "data_size is zero");
    }
    if (result != noErr) {
        throw Exception( __FILE__, __LINE__,
                         "Can't get stream configuration ", get_error_name(result));
    }

    // Allocate buffer
    AudioBufferList *buffer_list = (AudioBufferList *)malloc(data_size);
    if (!buffer_list) {
        throw Exception( __FILE__, __LINE__, "Can't allocate buffer ", get_error_name(result));
    }

    result = AudioDeviceGetProperty(device_id, 0, true,
                                    kAudioDevicePropertyStreamConfiguration,
                                    &data_size, buffer_list);

    if (result != noErr) {
        free(buffer_list);
        buffer_list = NULL;
        throw Exception( __FILE__, __LINE__,
                         "Can't get stream configuration ", get_error_name(result));
    }

    UInt32 dev_channels = 0;
    for (UInt32 i = 0; i < buffer_list->mNumberBuffers; i++) {
        dev_channels += buffer_list->mBuffers[i].mNumberChannels;
    }
    if (getChannel() > dev_channels) {
        free(buffer_list);
        buffer_list = NULL;
        throw Exception( __FILE__, __LINE__,
                         "Invalid number of channels", getChannel());
    }

    free(buffer_list);
    buffer_list = NULL;
    
    // set stream format.
    Float64 sample_rate = getSampleRate();
    data_size = sizeof(sample_rate);
    result = AudioDeviceSetProperty(device_id, 0, true, 0,
                                    kAudioDevicePropertyNominalSampleRate, data_size, &sample_rate);
    if (result != noErr) {
        throw Exception( __FILE__, __LINE__, "Can't set stream format:", get_error_name(result));
    }

    AudioStreamBasicDescription description;
    data_size = sizeof(description);
	result = AudioDeviceGetProperty(device_id, 0, true,
                                    kAudioDevicePropertyStreamFormat, &data_size, &description);
    if (result != noErr) {
        throw Exception( __FILE__, __LINE__, "Can't get stream format:", get_error_name(result));
    }
#if 0
    fprintf(stderr, "stream format:sr=%f\nfid='%c%c%c%c'\nflags=0x%x\ncpf=%u\nbpf=%u\nbpp=%u\n",
            description.mSampleRate,
            (description.mFormatID >> 24) & 0x00ff, (description.mFormatID >> 16) & 0x00ff,
            (description.mFormatID >> 8) & 0x00ff, description.mFormatID & 0x00ff,
            description.mFormatFlags,
            description.mChannelsPerFrame,
            description.mBytesPerFrame,
            description.mBytesPerPacket);
#endif
    if (description.mChannelsPerFrame != 1 && description.mChannelsPerFrame != 2) {
        throw Exception( __FILE__, __LINE__, "not supported device channels ", description.mChannelsPerFrame);
    }
    stream_description = description;
    is_opened = true;
    return true;
}


/*------------------------------------------------------------------------------
 *  Check wether read() would return anything
 *----------------------------------------------------------------------------*/
bool
CoreAudioDspSource :: canRead ( unsigned int   sec,
                           unsigned int   usec )    throw ( Exception )
{
    const unsigned int max_wait_time  = sec * 1000000;
    const unsigned int wait_increment = 10000;
    unsigned int       cur_wait       = 0;

    if ( !isOpen() ) {
        return false;
    }

    if (!is_running) {
        startDevice();
    }

    while (max_wait_time > cur_wait) {
        bool canRead = true;

        for (unsigned int c = 0 ; c < getChannel() ; c++) {
            if (jack_ringbuffer_read_space(rb[c]) <= 0) {
                canRead = false;
            }
        }

        if (canRead) {
            return true;
        }

        cur_wait += wait_increment;
        usleep ( wait_increment );
    }

    usleep( usec );
    for (unsigned int c = 0 ; c < getChannel() ; c++) {
        if (jack_ringbuffer_read_space(rb[c]) <= 0) {
            return false;
        }
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Read from the audio source
 *----------------------------------------------------------------------------*/
unsigned int
CoreAudioDspSource :: read (   void          * buf,
                          unsigned int    len )     throw ( Exception )
{
    unsigned int samples         = len / (getBitsPerSample() >> 3) / getChannel();
    unsigned int samples_read[2] = {0,0};
    unsigned int c, n;

    if ( !isOpen() ) {
        return 0;
    }

    if (!is_running) {
        startDevice();
    }

    // Ensure the temporary buffer is big enough
    tmp_buffer = (char *)realloc(tmp_buffer, samples * sizeof(Float32));
    if (!tmp_buffer) {
        throw Exception( __FILE__, __LINE__, "realloc on tmp_buffer failed");
    }

    // We must be sure to fetch as many data on both channels
    int minBytesAvailable = samples * sizeof(Float32);

    for (c = 0; c < getChannel(); c++) {
        int readable = jack_ringbuffer_read_space(rb[c]);
        if (readable < minBytesAvailable) {
            minBytesAvailable = readable;
        }
    }

    for (c = 0; c < getChannel(); c++) {    
        // Copy frames from ring buffer to temporary buffer
        // and then convert samples to output buffer
        int bytes_read = jack_ringbuffer_read(rb[c],
                                             (char*)tmp_buffer,
                                              minBytesAvailable);
        samples_read[c] = bytes_read / sizeof(Float32);
        
        // Convert samples from float to char/short and put in output buffer
        if (getBitsPerSample() == 8) {
            char *output8 = (char*)buf;
            Float32 *in = (Float32 *)tmp_buffer;
            for (n = 0; n < samples_read[c]; n++) {
                int tmp = lrintf(in[n] * 128.0f);
                if (tmp > CHAR_MAX) {
                    output8[n * getChannel() + c] = CHAR_MAX;
                } else if (tmp < CHAR_MIN) {
                    output8[n * getChannel() + c] = CHAR_MIN;
                } else {
                    output8[n * getChannel() + c] = (char)tmp;
                }
            }

        } else { // 16
            short *output16 = (short*)buf;
            Float32 *in = (Float32 *)tmp_buffer;
            for (n = 0; n < samples_read[c]; n++) {
                int tmp = lrintf(in[n] * 32768.0f);
                if (tmp > SHRT_MAX) {
                    output16[n * getChannel() + c] = SHRT_MAX;
                } else if (tmp < SHRT_MIN) {
                    output16[n * getChannel() + c] = SHRT_MIN;
                } else {
                    output16[n * getChannel() + c] = (short)tmp;
                }
            }
        }
    }

    // Didn't get as many samples as we wanted ?
    if (getChannel() == 2 && samples_read[0] != samples_read[1]) {
        Reporter::reportEvent( 2,
                              "Warning: Read a different number of samples "
                              "for left and right channels");
    }

    // Return the number of bytes put in the output buffer
    return samples_read[0] * (getBitsPerSample() >> 3) * getChannel();
}


/*------------------------------------------------------------------------------
 *  Close the audio source
 *----------------------------------------------------------------------------*/
void
CoreAudioDspSource :: close ( void )                  throw ( Exception )
{
    unsigned int i;

    if ( !isOpen() ) {
        return;
    }

    if (is_running) {
        stopDevice();
    }

    for (i = 0; i < getChannel(); i++) {
        if (rb[i]) {
            jack_ringbuffer_free(rb[i]);
            rb[i] = NULL;
        }
    }
    is_opened = false;
}


/*------------------------------------------------------------------------------
 *  Callback called by CoreAudio when audio is available
 *
 *  Don't do anything too expensive here
 *      - just shove audio samples in ring buffer
 *----------------------------------------------------------------------------*/
OSStatus
CoreAudioDspSource :: callback_handler( AudioDeviceID inDevice,
                                        const AudioTimeStamp *inNow,
                                        const AudioBufferList *inInputData,
                                        const AudioTimeStamp *inInputTime,
                                        AudioBufferList *outOutputData,
                                        const AudioTimeStamp *inOutputTime, 
                                        void *infoPointer )
{
    CoreAudioDspSource *self = (CoreAudioDspSource *)infoPointer;
    bool err = false;

    if (self->stream_description.mFormatFlags & kAudioFormatFlagIsNonInterleaved) {
#if 1
        Reporter::reportEvent( 1, "not supported.");
        err = true;
#else
        unsigned int c;
        //XXX not tested.
        /* copy data to ringbuffer; one per channel */
        if ((self->getChannel() == 2 && self->stream_description.mChannelsPerFrame == 2)
            || (self->getChannel() == 1 && self->stream_description.mChannelsPerFrame == 1)) {
            for (c = 0; c < self->getChannel(); c++) {    
                char *buf  = (char*)inInputData->mBuffers[c].mData;
                size_t to_write = inInputData->mBuffers[c].mDataByteSize;
                size_t len = jack_ringbuffer_write(self->rb[c], buf, to_write);
                if (len < to_write) {
                    Reporter::reportEvent( 1, "failed to write to ring buffer");
                    err = true;
                }
            }
        } else if (self->getChannel() == 1 && self->stream_description.mChannelsPerFrame == 2) {
            Float32 *dataL  = (Float32 *)inInputData->mBuffers[0].mData;
            Float32 *dataR  = (Float32 *)inInputData->mBuffers[1].mData;
            size_t to_write = inInputData->mBuffers[0].mDataByteSize;
            size_t sz = to_write / sizeof(Float32);
            for (unsigned i = 0; i < sz; i++) {
                dataL[i] = (dataL[i] + dataR[i]) / 2.0;
            }
            size_t len = jack_ringbuffer_write(self->rb[0], (char *)dataL, to_write);
            if (len < to_write) {
                Reporter::reportEvent( 1, "failed to write to ring buffer");
                err = true;
            }

        } else if (self->getChannel() == 2 && self->stream_description.mChannelsPerFrame == 1) {
            for (c = 0; c < self->getChannel(); c++) {    
                char *buf  = (char*)inInputData->mBuffers[0].mData;
                size_t to_write = inInputData->mBuffers[0].mDataByteSize;
                size_t len = jack_ringbuffer_write(self->rb[c], buf, to_write);
                if (len < to_write) {
                    Reporter::reportEvent( 1, "failed to write to ring buffer");
                    err = true;
                }
            }
        } else {
            Reporter::reportEvent( 1, "not reach here.");
            err = true;
        }
#endif
    } else {

        Float32 *data = (Float32 *)inInputData->mBuffers[0].mData;
        if (self->getChannel() == 2 && self->stream_description.mChannelsPerFrame == 2) {
            // Ensure the temporary buffer is big enough
            size_t sz = inInputData->mBuffers[0].mDataByteSize;
            self->cnv_buffer = (char *)realloc(self->cnv_buffer, sz);
            if (!self->cnv_buffer) {
                Reporter::reportEvent( 1, "realloc on cnv_buffer failed");
                return kAudioHardwareUnspecifiedError;
            }
            
            size_t to_write = sz / 2;
            Float32 *dataL = (Float32 *)self->cnv_buffer;
            Float32 *dataR = (Float32 *)(self->cnv_buffer + to_write);
            
            sz /= sizeof(Float32);
            for (unsigned int i = 0; i < sz; i += 2) {
                dataL[i / 2] = data[i];
                dataR[i / 2] = data[i + 1];
            }
            
            size_t len = jack_ringbuffer_write(self->rb[0], (char *)dataL, to_write);
            if (len < to_write) {
                Reporter::reportEvent( 1, "failed to write to ring buffer");
                err = true;
            }

            len = jack_ringbuffer_write(self->rb[1], (char *)dataR, to_write);
            if (len < to_write) {
                Reporter::reportEvent( 1, "failed to write to ring buffer");
                err = true;
            }
        } else if (self->getChannel() == 1 && self->stream_description.mChannelsPerFrame == 2) {
            // Ensure the temporary buffer is big enough
            size_t sz = inInputData->mBuffers[0].mDataByteSize;
            self->cnv_buffer = (char *)realloc(self->cnv_buffer, sz / 2);
            if (!self->cnv_buffer) {
                Reporter::reportEvent( 1, "realloc on cnv_buffer failed");
                return kAudioHardwareUnspecifiedError;
            }
            
            size_t to_write = sz / 2;
            Float32 *dataM = (Float32 *)self->cnv_buffer;
            
            sz /= sizeof(Float32);
            for (unsigned int i = 0; i < sz; i += 2) {
                dataM[i / 2] = ((data[i] + data[i + 1]) / 2.0);
            }
            
            size_t len = jack_ringbuffer_write(self->rb[0], (char *)dataM, to_write);
            if (len < to_write) {
                Reporter::reportEvent( 1, "failed to write to ring buffer");
                err = true;
            }

        } else if (self->getChannel() == 2 && self->stream_description.mChannelsPerFrame == 1) {
#if 1
            Reporter::reportEvent( 1, "not supported.");
            err = true;
#else
            //XXX not tested.
            // Ensure the temporary buffer is big enough
            size_t sz = inInputData->mBuffers[0].mDataByteSize;
            self->cnv_buffer = (char *)realloc(self->cnv_buffer, sz * 2);
            if (!self->cnv_buffer) {
                Reporter::reportEvent( 1, "realloc on cnv_buffer failed");
                return kAudioHardwareUnspecifiedError;
            }
            
            size_t to_write = sz;
            Float32 *dataL = (Float32 *)self->cnv_buffer;
            Float32 *dataR = (Float32 *)(self->cnv_buffer + to_write);
            
            sz /= sizeof(Float32);
            for (unsigned int i = 0; i < sz; i++) {
                dataL[i] = data[i];
                dataR[i] = data[i];
            }
            
            size_t len = jack_ringbuffer_write(self->rb[0], (char *)dataL, to_write);
            if (len < to_write) {
                Reporter::reportEvent( 1, "failed to write to ring buffer");
                err = true;
            }

            len = jack_ringbuffer_write(self->rb[1], (char *)dataR, to_write);
            if (len < to_write) {
                Reporter::reportEvent( 1, "failed to write to ring buffer");
                err = true;
            }
#endif
        } else if (self->getChannel() == 1 && self->stream_description.mChannelsPerFrame == 1) {
#if 1
            Reporter::reportEvent( 1, "not supported.");
            err = true;
#else
            //XXX not tested.
            size_t to_write = inInputData->mBuffers[0].mDataByteSize;
            size_t len = jack_ringbuffer_write(self->rb[0], (char *)data, to_write);
            if (len < to_write) {
                Reporter::reportEvent( 1, "failed to write to ring buffer");
                err = true;
            }
#endif
        } else {
            Reporter::reportEvent( 1, "not reach here.");
            err = true;
        }
    }

    if (err) {
        return kAudioHardwareUnspecifiedError;
    }
    // Success
    return kAudioHardwareNoError;
}

/*
 */
OSStatus
CoreAudioDspSource :: device_listener( AudioDeviceID inDevice,
                                       UInt32 channel,
                                       Boolean isInput,
                                       AudioDevicePropertyID propertyID,
                                       void* handlePointer)
{
#if 0
    CoreAudioDspSource* self =  (CoreAudioDspSource*)handlePointer;
    if ( propertyID == kAudioDeviceProcessorOverload ) {
        Reporter::reportEvent( 1, "CoreAudioDspSource :: device_listener xrun");
        // xrun!
    }
#endif
    return kAudioHardwareNoError;
}

/*
 */
static unsigned int get_device_count(void)
{
    unsigned int count = 0;
    UInt32 data_size = 0;
    OSStatus result = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &data_size, NULL);
    if (result != noErr) {
        Reporter::reportEvent( 1, "CoreAudioDspSource ::  get_device_count():error:",
                               get_error_name(result));
        return 0;
    }

    count = data_size / sizeof(AudioDeviceID);

    return count;
}

/*
 */
bool CoreAudioDspSource :: startDevice()
{
    if (is_running) {
        return true;
    }

    OSStatus result = AudioDeviceAddIOProc(device_id, callback_handler, this);
    if (result != noErr) {
        Reporter::reportEvent( 1, "CoreAudioDspSource ::  startDevice():error:AudioDeviceAddIOProc():",
                               get_error_name(result));
        return false;
    }

    result = AudioDeviceStart(device_id, callback_handler);
    if (result != noErr) {
        Reporter::reportEvent( 1, "CoreAudioDspSource ::  startDevice():error:AudioDeviceStart():",
                               get_error_name(result));
        return false;
    }
    is_running = true;
    return true;
}

/*
 */
bool CoreAudioDspSource :: stopDevice()
{
    if (!is_running) {
        return true;
    }

    OSStatus result = AudioDeviceRemoveIOProc(device_id, callback_handler);
    if (result != noErr) {
#if 0
        Reporter::reportEvent( 1, "CoreAudioDspSource ::  stopDevice():error:AudioDeviceRemoveIOProc():",
                               get_error_name(result));
        return false;
#endif
    }

    result = AudioDeviceStop(device_id, callback_handler);
    if (result != noErr) {
#if 0
        Reporter::reportEvent( 1, "CoreAudioDspSource ::  stopDevice():error:AudioDeviceStop():",
                               get_error_name(result));
        return false;
#endif
    }
    is_running = false;
    return true;
}


static const char* get_error_name( OSStatus code )
{
  switch( code ) {

  case kAudioHardwareNotRunningError:
    return "kAudioHardwareNotRunningError";

  case kAudioHardwareUnspecifiedError:
    return "kAudioHardwareUnspecifiedError";

  case kAudioHardwareUnknownPropertyError:
    return "kAudioHardwareUnknownPropertyError";

  case kAudioHardwareBadPropertySizeError:
    return "kAudioHardwareBadPropertySizeError";

  case kAudioHardwareIllegalOperationError:
    return "kAudioHardwareIllegalOperationError";
#if 0
  case kAudioHardwareBadObjectError:
    return "kAudioHardwareBadObjectError";
#endif
  case kAudioHardwareBadDeviceError:
    return "kAudioHardwareBadDeviceError";

  case kAudioHardwareBadStreamError:
    return "kAudioHardwareBadStreamError";

  case kAudioHardwareUnsupportedOperationError:
    return "kAudioHardwareUnsupportedOperationError";

  case kAudioDeviceUnsupportedFormatError:
    return "kAudioDeviceUnsupportedFormatError";

  case kAudioDevicePermissionsError:
    return "kAudioDevicePermissionsError";

  default:
    return "CoreAudio unknown error";
  }
}

#endif // SUPPORT_COREAUDIO_DSP

