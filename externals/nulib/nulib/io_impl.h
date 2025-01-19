#ifndef NU_IO_IMPL_H
#define NU_IO_IMPL_H

#include "io.h"

#ifdef NU_PLATFORM_UNIX
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#endif

static nu_status_t
mkpath_p (nu_sv_t path)
{
#ifdef NU_PLATFORM_UNIX
    nu_char_t file_path[NU_PATH_MAX];
    nu_sv_to_cstr(path, file_path, NU_PATH_MAX);
    for (nu_char_t *p = strchr(file_path + 1, '/'); p; p = strchr(p + 1, '/'))
    {
        *p = '\0';
        if (mkdir(file_path, 0755) == -1)
        {
            if (errno != EEXIST)
            {
                *p = '/';
                return NU_FAILURE;
            }
        }
        *p = '/';
    }
    return NU_SUCCESS;
#else
    (void)path;
    return NU_FAILURE;
#endif
}
nu_status_t
nu_load_bytes (nu_sv_t filename, nu_byte_t *data, nu_size_t *size)
{
    nu_char_t buf[NU_PATH_MAX];
    nu_sv_to_cstr(filename, buf, sizeof(buf));
    FILE *f = fopen(buf, "rb");
    if (!f)
    {
        return NU_FAILURE;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size)
    {
        *size = fsize;
    }

    if (data)
    {
        fread(data, fsize, 1, f);
    }

    fclose(f);
    return NU_SUCCESS;
}
nu_status_t
nu_save_bytes (nu_sv_t filename, const nu_byte_t *data, nu_size_t size)
{
    nu_char_t buf[NU_PATH_MAX]; // TODO: support PATH_MAX
    nu_sv_to_cstr(filename, buf, sizeof(buf));
    if (!mkpath_p(filename))
    {
        return NU_FAILURE;
    }
    FILE *f = fopen(buf, "wb");
    if (!f)
    {
        return NU_FAILURE;
    }
    nu_int_t n = fwrite(data, size, 1, f);
    fclose(f);
    return n == 1 ? NU_SUCCESS : NU_FAILURE;
}
nu_bool_t
nu_isdir (nu_sv_t path)
{
#ifdef NU_PLATFORM_UNIX
    nu_char_t s[NU_PATH_MAX];
    nu_sv_to_cstr(path, s, NU_PATH_MAX);
    struct stat statbuf;
    if (stat(s, &statbuf) != 0)
    {
        return NU_FALSE;
    }
    return S_ISDIR(statbuf.st_mode);
#else
    (void)path;
    return NU_FALSE;
#endif
}

#endif
