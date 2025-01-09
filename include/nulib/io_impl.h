#ifndef NU_IO_IMPL_H
#define NU_IO_IMPL_H

#include <nulib/io.h>

#ifdef NU_PLATFORM_UNIX
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#endif

nu_bool_t
mkpath_p (nu_sv_t path)
{
#ifdef NU_PLATFORM_UNIX
    nu_char_t file_path[NU_PATH_MAX];
    nu_sv_to_cstr(path, file_path, NU_PATH_MAX);
    for (char *p = strchr(file_path + 1, '/'); p; p = strchr(p + 1, '/'))
    {
        *p = '\0';
        if (mkdir(file_path, 0755) == -1)
        {
            if (errno != EEXIST)
            {
                *p = '/';
                return NU_FALSE;
            }
        }
        *p = '/';
    }
    return NU_TRUE;
#endif
}
nu_bool_t
nu_load_bytes (nu_sv_t filename, nu_byte_t *data, nu_size_t *size)
{
    nu_char_t buf[NU_PATH_MAX];
    nu_sv_to_cstr(filename, buf, sizeof(buf));
    FILE *f = fopen(buf, "rb");
    if (!f)
    {
        printf("Failed to open file %s\n", buf);
        return NU_NULL;
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
    return NU_TRUE;
}
nu_bool_t
nu_save_bytes (nu_sv_t filename, const nu_byte_t *data, nu_size_t size)
{
    nu_char_t buf[NU_PATH_MAX]; // TODO: support PATH_MAX
    nu_sv_to_cstr(filename, buf, sizeof(buf));
    if (!mkpath_p(filename))
    {
        printf("Failed to create directory for %s\n", buf);
        return NU_FALSE;
    }
    FILE *f = fopen(buf, "wb");
    if (!f)
    {
        printf("Failed to open file %s\n", buf);
        return NU_FALSE;
    }
    nu_int_t n = fwrite(data, size, 1, f);
    fclose(f);
    return n == 1;
}

#endif
