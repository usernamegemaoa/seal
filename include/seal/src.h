/*
 * src.h is part of the Scorched End Audio Library (SEAL) and is licensed
 * under the terms of the GNU Lesser General Public License. See COPYING
 * attached with the library.
 *
 * src.h wraps up the abstract data type `seal_src_t'. Sources are abstract
 * representations of sound sources which emit sound in Euclidean space.
 */

#ifndef _SEAL_SRC_H_
#define _SEAL_SRC_H_

#include <stddef.h>
#include "buf.h"
#include "stream.h"

/*
 * A just-initialized source is of the `SEAL_UNDETERMINED' type. A source that
 * attaches a buffer through `seal_set_src_buf' will become the `SEAL_STATIC'
 * type. A source that streams audio from file through `seal_set_src_stream'
 * will become the `SEAL_STREAMING' type.
 */
enum seal_src_type_t
{
    SEAL_UNDETERMINED,
    SEAL_STATIC,
    SEAL_STREAMING
};

/*
 * A just-initialized source is in the `SEAL_INITIAL' state. After a call to
 * `seal_play_src', the source will enter the `SEAL_PLAYING' state. After a
 * call to `seal_pause_src', the source will enter the `SEAL_PAUSED' state.
 * After a call to `seal_stop_src', the source will enter the `SEAL_STOPPED'
 * state.
 */
enum seal_src_state_t
{
    SEAL_INITIAL,
    SEAL_PLAYING,
    SEAL_PAUSED,
    SEAL_STOPPED
};

typedef struct seal_src_t seal_src_t;
typedef enum seal_src_type_t seal_src_type_t;
typedef enum seal_src_state_t seal_src_state_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Allocates a new source.
 *
 * @return      the newly allocated source if successful or otherwise 0
 */
seal_src_t* seal_alloc_src(void);

/*
 * Deallocates a source. Will not free the associated buffer or stream. Can
 * be applied on sources in any state and will automatically stop any
 * playback.
 *
 * @param src   the source to deallocate
 */
void seal_free_src(seal_src_t*);

/*
 * Starts to play a source. Applying to a `SEAL_PLAYING' source will restart
 * the playback from the beginning thus reset the sampling offset too. If the
 * source is a streaming source, restarting the playback will automatically
 * rewind the stream to the beginning. Applying to a `SEAL_INITIAL' or
 * `SEAL_STOPPED' source will start start playing and change its state to
 * `SEAL_PLAYING'. Applying to a `SEAL_PAUSED' source will resume playing and
 * change its state to `SEAL_PLAYING'.
 *
 * @param src   the source to play
 */
void seal_play_src(seal_src_t*);

/*
 * Pauses the playing of a source. Applying to a `SEAL_PLAYING' source will
 * change its state to `SEAL_PAUSED'. Applying to a `SEAL_INITIAL',
 * `SEAL_PAUSED' or `SEAL_STOPPED' source has no effect.
 *
 * @param src   the source to pause
 */
void seal_pause_src(seal_src_t*);

/*
 * Stops the playing of a source. Applying to a `SEAL_PLAYING' or
 * `SEAL_PAUSED' source will change its state to SEAL_STOPPED. Applying to a
 * `SEAL_INITIAL' or `SEAL_STOPPED' source has no effect. Resets the sampling
 * offset.
 *
 * @param src   the source to stop
 */
void seal_stop_src(seal_src_t*);

/*
 * Rewinds a source to the beginning. Applying to a `SEAL_PLAYING',
 * `SEAL_PAUSED' or `SEAL_STOPPED' source will change its state to
 * `SEAL_INITIAL'. Applying to a `SEAL_INITIAL' source has no effect. The
 * sampling offset will be reset to the beginning. Other attributes are
 * preserved.
 *
 * @param src   the source to rewind
 */
void seal_rewind_src(seal_src_t*);

/*
 * Associates a buffer with a source so that the source is ready to play the
 * audio contained in the buffer. Can be applied only to sources in the
 * `SEAL_INITIAL' or `SEAL_STOPPED' states and that are not of the
 * `SEAL_STREAMING' type. If successful, the source will become or remain the
 * `SEAL_STATIC' type.
 *
 * @param src   the source to associate the buffer `buf' with
 * @param buf   the buffer to associate the source `src' with
 * @return      nonzero if successful or otherwise 0
 */
int seal_set_src_buf(seal_src_t*, seal_buf_t*);

/*
 * Associates an opened stream with a source so that audio data can be
 * continuously fetched from a file rather than loading everything to memory
 * at once. Can be applied to sources in any playing state but not on the
 * `SEAL_STATIC' type of source. When replacing an attached stream, the new
 * stream must have the same audio format as the old one. Also be aware of the
 * fact that in this case there could still be some chunks of the old stream
 * at the front of the streaming queue waiting to be played. If successful,
 * the source will become or remain the `SEAL_STREAMING' type. The streaming
 * queue will be filled after this call returns; after the queue starts to be
 * played, `seal_update_src' should be called to refill the queue.
 *
 * @param src       the source to associate the stream `stream' with
 * @param stream    the stream to associate the source `src' with
 * @return          nonzero if successful or otherwise 0
 */
int seal_set_src_stream(seal_src_t*, seal_stream_t*);

/*
 * Updates a streaming source. If the source is not up-to-date, the playback
 * will end before the end of the stream is reached. Does nothing if the
 * passed-in source is not a streaming source.
 *
 * @param src       the source to update
 * @return          nonzero if successfully updated, 0 if end of stream is
 *                  reached and the source is not looping; -1 if an error
 *                  occurs
 */
int seal_update_src(seal_src_t*);

/*
 * Releases the current buffer or stream from a source (hence empties the
 * queue for streaming sources). Will reset the source type to
 * `SEAL_UNDETERMINED' and the source state to `SEAL_STOPPED'. Will not free
 * the associated buffer or stream.
 *
 * @param src       the source to detach the audio of
 * @return          nonzero if successful or otherwise 0
 */
void seal_detach_src_audio(seal_src_t*);

/*
 * Sets the size of the streaming queue internally used by a source. The queue
 * maintains a multiple buffering mechanism when streaming the audio data.
 * Multiple bufferring allows buffers in the queue to be processed while the
 * one at the front of the queue is still being played. A queue of size 2
 * (double buffering) may still be inefficient in CPU-, and I/O-bound
 * situations while triple, or even quad buffering generally produces better
 * sound quality in non-memory-bound situations.
 *
 * @param src   the source to set the queue size of
 * @param size  the queue size in the interval [2, 127]
 * @return      nonzero if successful or otherwise 0
 */
int seal_set_src_queue_size(seal_src_t*, size_t);

/*
 * Sets the maximum size, in byte, of the audio chunk which buffers the audio
 * data constantly fetched from an audio stream. Using small chunks may cause
 * playback to occur before the required audio chunk is ready, which in turn
 * causes unexpected stop of playback.
 *
 * @param src   the source to set the chunk size of
 * @param size  the chunk size in the interval [9216, 16773120]; must be a
 *              multiple of 9216
 * @return      nonzero if successful or otherwise 0
 */
int seal_set_src_chunk_size(seal_src_t*, size_t);

/*
 * Sets the position of a source in a right-handed Cartesian coordinate
 * system. Use of NaN and infinity is undefined.
 *
 * @param src   the source to set the position of
 * @param x     the x position to set
 * @param y     the y position to set
 * @param z     the z position to set
 */
void seal_set_src_pos(seal_src_t*, float /*x*/, float /*y*/, float /*z*/);

/*
 * Sets the velocity of a source in a right-handed Cartesian coordinate
 * system. The velocity of the source does not affect its position but is a
 * factor used during the Doppler effect emulation. Use of NaN is undefined.
 *
 * @param src   the source to set the velocity of
 * @param x     the x velocity to set
 * @param y     the y velocity to set
 * @param z     the z velocity to set
 */
void seal_set_src_vel(seal_src_t*, float /*x*/, float /*y*/, float /*z*/);

/*
 * Sets the pitch shift multiplier of a source. 1.0f means identity; each
 * reduction by 1/2 means a pitch shift of -12 semitones; each doubling means
 * a pitch shift of 12 semitones. Use of 0.0f is undefined.
 *
 * @param src   the source to set the pitch of
 * @param pitch the pitch multiplier in the interval (0.0f, +inf.)
 * @return      nonzero if successful or otherwise 0
 */
int seal_set_src_pitch(seal_src_t*, float /*pitch*/);

/*
 * Sets the scalar amplitude multiplier of a source. 1.0f means that the sound
 * is unattenuated; 0.5f means an attenuation of 6 dB; 0.0f means silence.
 *
 * @param src   the source to set the gain of
 * @param gain  the scalar amplitude multiplier in the interval [0.0f, +inf.)
 * @return      nonzero if successful or otherwise 0
 */
int seal_set_src_gain(seal_src_t*, float /*gain*/);

/*
 * Sets whether a source's position, velocity, cone and direction are all
 * relative to the listener position.
 *
 * @param src       the source to set the relative property of
 * @param relative  1 to set it relative to the listener or otherwise 0
 * @return          nonzero if successful or otherwise 0
 */
int seal_set_src_relative(seal_src_t*, int /*relative*/);

/*
 * Sets whether the playback of a source is looping. A looping source will
 * never enter the `SEAL_STOPPED' state; it will immediate enter
 * `SEAL_INITIAL' and then SEAL_PLAYING after it reaches the end of the last
 * buffer.
 *
 * @param src       the source to set the looping property of
 * @param looping   1 to set it looping or otherwise 0
 * @return          nonzero if successful or otherwise 0
 */
int seal_set_src_looping(seal_src_t*, int /*looping*/);

/*
 * Gets the buffer of a source. The default is 0 (null pointer).
 *
 * @see         seal_set_src_buffer
 * @param src   the source to get the buffer of
 * @return      the buffer
 */
seal_buf_t* seal_get_src_buf(seal_src_t*);

/*
 * Gets the stream of a source. The default is 0 (null pointer).
 *
 * @see         seal_set_src_stream
 * @param src   the source to get the stream of
 * @return      the stream
 */
seal_stream_t* seal_get_src_stream(seal_src_t*);

/*
 * Gets the size, in byte, of an source's streaming queue. The default is 3.
 *
 * @see         seal_set_src_queue_size
 * @param src   the source to get the queue size of
 * @return      the queue size
 */
size_t seal_get_src_queue_size(seal_src_t*);

/*
 * Gets the size, in byte, of an source's streaming chunk. The default is
 * 36864.
 *
 * @see         seal_set_src_chunk_size
 * @param src   the source to get the chunk size of
 * @return      the chunk size
 */
size_t seal_get_src_chunk_size(seal_src_t*);

/*
 * Gets the position of a source. The default is ( 0.0f, 0.0f, 0.0f ).
 *
 * @see         seal_set_src_pos
 * @param src   the source to get the position of
 * @param x     receives the x position
 * @param y     receives the y position
 * @param z     receives the z position
 */
void seal_get_src_pos(seal_src_t*, float* /*x*/, float* /*y*/, float* /*z*/);

/*
 * Gets the velocity of a source. The default is ( 0.0f, 0.0f, 0.0f ).
 *
 * @see         seal_set_src_vel
 * @param src   the source to get the velocity of
 * @param x     receives the x velocity
 * @param y     receives the y velocity
 * @param z     receives the z velocity
 */
void seal_get_src_vel(seal_src_t*, float* /*x*/, float* /*y*/, float* /*z*/);

/*
 * Gets the pitch of a source. The default is 1.0f.
 *
 * @see         seal_set_src_pitch
 * @param src   the source to get the pitch of
 * @return      the pitch
 */
float seal_get_src_pitch(seal_src_t*);

/*
 * Gets the gain of a source. The default is 1.0f.
 *
 * @see         seal_set_src_gain
 * @param src   the source to get the gain of
 * @return      the gain
 */
float seal_get_src_gain(seal_src_t*);

/*
 * Gets the relative property of a source. The default is 0.
 *
 * @see         seal_set_src_relative
 * @param src   the source to get the relative property of
 * @return      nonzero if the source is relative to the listener or otherwise
 *              0
 */
int seal_is_src_relative(seal_src_t*);

/*
 * Gets the looping property of a source. The default is 0.
 *
 * @see         seal_set_src_looping
 * @param src   the source to get the looping property of
 * @return      nonzero if the source is looping or otherwise 0
 */
int seal_is_src_looping(seal_src_t*);

/*
 * Gets the type of a source.
 *
 * @see         enum seal_src_type_t
 * @param src   the source to get the source type of
 * @return      the source type
 */
seal_src_type_t seal_get_src_type(seal_src_t*);

/*
 * Gets the state of a source.
 * @see         enum seal_src_state_t
 * @param src   the source to get the state of
 * @return      the source state
 */
seal_src_state_t seal_get_src_state(seal_src_t*);

#ifdef __cplusplus
}
#endif

#endif /* _SEAL_SRC_H_ */
