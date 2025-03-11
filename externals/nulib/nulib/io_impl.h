#ifndef NU_IO_IMPL_H
#define NU_IO_IMPL_H

#include "io.h"
#include "assert.h"

#ifdef NU_PLATFORM_UNIX
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
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

nu_sv_t
nu_path_getcwd (nu_char_t *buf, nu_size_t n)
{
    if (getcwd(buf, n) != NU_NULL)
    {
        return nu_sv(buf, n);
    }
    else
    {
        return nu_sv_empty();
    }
}
nu_bool_t
nu_path_isdir (nu_sv_t path)
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
nu_status_t
nu_path_list_files (nu_sv_t path,
                    nu_char_t (*files)[NU_PATH_MAX],
                    nu_size_t  capa,
                    nu_size_t *count)
{
    NU_ASSERT(path.len);
    *count = 0;
    nu_char_t s[NU_PATH_MAX];
    nu_sv_to_cstr(path, s, NU_PATH_MAX);
    DIR *d;
    d = opendir(s);
    if (!d)
    {
        return NU_FAILURE;
    }
    struct dirent *dir;
    while ((dir = readdir(d)))
    {
        // Apply filter
        if (!nu_strneq(dir->d_name, ".", NU_PATH_MAX)
            && !nu_strneq(dir->d_name, "..", NU_PATH_MAX)
            && !(nu_strnlen(dir->d_name, NU_PATH_MAX) && dir->d_name[0] == '.'))
        {
            if (*count < capa)
            {
                nu_char_t realpath_buf[NU_PATH_MAX];
                nu_sv_t   realpath
                    = nu_path_concat(realpath_buf,
                                     NU_PATH_MAX,
                                     path,
                                     nu_sv(dir->d_name, NU_PATH_MAX));
                nu_sv_to_cstr(realpath, files[*count], NU_PATH_MAX);
            }
            ++(*count);
        }
    }
    closedir(d);
    return NU_SUCCESS;
}
nu_sv_t
nu_path_basename (nu_sv_t path)
{
    for (nu_size_t n = path.len; n; --n)
    {
        if (path.ptr[n - 1] == '/')
        {
            return nu_sv_slice(path.ptr + n, path.len - n);
        }
    }
    return path;
}
nu_sv_t
nu_path_dirname (nu_sv_t path)
{
    for (nu_size_t n = path.len; n; --n)
    {
        if (path.ptr[n - 1] == '/')
        {
            return nu_sv_slice(path.ptr, n);
        }
    }
    return path;
}
nu_sv_t
nu_path_parent (nu_sv_t path)
{
    NU_ASSERT(path.len);
    // Remove trailing '/' for folder
    if (path.ptr[path.len - 1] == '/')
    {
        --path.len;
    }
    // Remove the basename
    for (nu_size_t n = path.len; n; --n)
    {
        if (path.ptr[n - 1] == '/')
        {
            if (n - 1 == 0) // Special case for root dir
            {
                ++n;
            }
            return nu_sv_slice(path.ptr, n - 1);
        }
    }
    return nu_sv_empty();
}
nu_sv_t
nu_path_concat (nu_char_t *buf, nu_size_t n, nu_sv_t p1, nu_sv_t p2)
{
    if (!p1.len)
    {
        return nu_sv_to_cstr(p2, buf, n);
    }
    if (!p2.len)
    {
        return nu_sv_to_cstr(p1, buf, n);
    }

    const nu_char_t *fmt;
    if (p1.ptr[p1.len - 1] == '/')
    {
        fmt = NU_SV_FMT NU_SV_FMT;
    }
    else
    {
        fmt = NU_SV_FMT "/" NU_SV_FMT;
    }
    return nu_snprintf(buf, n, fmt, NU_SV_ARGS(p1), NU_SV_ARGS(p2));
}

#endif
