#include <al/al.h>
#include <seal/listener.h>
#include <seal/err.h>

static
seal_err_t
setf(int key, float val)
{
    alListenerf(key, val);

    return _seal_get_openal_err();
}

static
seal_err_t
set3f(int key, float x, float y, float z)
{
    alListener3f(key, x, y, z);

    return _seal_get_openal_err();
}

static
seal_err_t
setfv(int key, float* vector)
{
    alListenerfv(key, vector);

    return _seal_get_openal_err();
}

static
seal_err_t
getf(int key, float* pval)
{
    alGetListenerf(key, pval);

    return _seal_get_openal_err();
}

static
seal_err_t
get3f(int key, float* px, float* py, float* pz)
{
    alGetListener3f(key, px, py, pz);

    return _seal_get_openal_err();
}

static
seal_err_t
getfv(int key, float* vector)
{
    alGetListenerfv(key, vector);

    return _seal_get_openal_err();
}

seal_err_t
SEAL_API
seal_move_listener(void)
{
    seal_err_t err;
    float pos[3], vel[3];

    if ((err = seal_get_listener_pos(pos, pos + 1, pos + 2)) != SEAL_OK)
        return err;
    if ((err = seal_get_listener_vel(vel, vel + 1, vel + 2)) != SEAL_OK)
        return err;
    err = seal_set_listener_pos(
        pos[0] + vel[0],
        pos[1] + vel[1],
        pos[2] + vel[2]
    );
    if (err != SEAL_OK)
        return err;

    return SEAL_OK;
}

seal_err_t
SEAL_API
seal_set_listener_gain(float gain)
{
    return setf(AL_GAIN, gain);
}

seal_err_t
SEAL_API
seal_set_listener_pos(float x, float y, float z)
{
    return set3f(AL_POSITION, x, y, z);
}

seal_err_t
SEAL_API
seal_set_listener_vel(float x, float y, float z)
{
    return set3f(AL_VELOCITY, x, y, z);
}

seal_err_t
SEAL_API
seal_set_listener_orien(float* orien)
{
    return setfv(AL_ORIENTATION, orien);
}

seal_err_t
SEAL_API
seal_get_listener_gain(float* pgain)
{
    return getf(AL_GAIN, pgain);
}

seal_err_t
SEAL_API
seal_get_listener_pos(float* px, float* py, float* pz)
{
    return get3f(AL_POSITION, px, py, pz);
}

seal_err_t
SEAL_API
seal_get_listener_vel(float* px, float* py, float* pz)
{
    return get3f(AL_VELOCITY, px, py, pz);
}

seal_err_t
SEAL_API
seal_get_listener_orien(float* orien)
{
    return getfv(AL_ORIENTATION, orien);
}
