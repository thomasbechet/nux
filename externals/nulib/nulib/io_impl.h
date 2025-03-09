#ifndef NU_IO_IMPL_H
#define NU_IO_IMPL_H

#include "io.h"
#include "nulib/nulib/string.h"

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
nu_size_t
nu_list_files (nu_sv_t path, nu_char_t (*files)[NU_PATH_MAX], nu_size_t capa)
{
    if (!nu_isdir(path))
    {
        return 0;
    }
    nu_size_t count = 0;
    nu_char_t s[NU_PATH_MAX];
    nu_sv_to_cstr(path, s, NU_PATH_MAX);
    DIR           *d;
    struct dirent *dir;
    d = opendir(s);
    if (d)
    {
        for (nu_size_t i = 0; i < capa; ++i)
        {
            dir = readdir(d);
            if (dir != NU_NULL)
            {
                if (!nu_sv_eq(nu_sv_cstr(dir->d_name), NU_SV("."))
                    && !nu_sv_eq(nu_sv_cstr(dir->d_name), NU_SV("..")))
                {
                    nu_char_t realpath_buf[NU_PATH_MAX];
                    nu_sv_t   realpath = nu_path_concat(realpath_buf,
                                                      sizeof(realpath_buf),
                                                      path,
                                                      nu_sv_cstr(dir->d_name));

                    nu_sv_to_cstr(realpath, files[count], NU_PATH_MAX);
                    ++count;
                }
            }
            else
            {
                break;
            }
        }
        closedir(d);
    }
    return count;
}
nu_sv_t
nu_getcwd (nu_char_t *buf, nu_size_t n)
{
    if (getcwd(buf, n) != NU_NULL)
    {
        return nu_sv_cstr(buf);
    }
    else
    {
        return nu_sv_null();
    }
}

nu_sv_t
nu_path_basename (nu_sv_t path)
{
    for (nu_size_t n = path.size; n; --n)
    {
        if (path.data[n - 1] == '/')
        {
            return nu_sv(path.data + n, path.size - n);
        }
    }
    return path;
}
nu_sv_t
nu_path_dirname (nu_sv_t path)
{
    for (nu_size_t n = path.size; n; --n)
    {
        if (path.data[n - 1] == '/')
        {
            return nu_sv(path.data, n);
        }
    }
    return path;
}
nu_sv_t
nu_path_parent (nu_sv_t path)
{
    // Remive trailing '/' for folder
    if (path.size && path.data[path.size - 1] == '/')
    {
        --path.size;
    }
    if (!path.size)
    {
        return nu_sv_null();
    }
    for (nu_size_t n = path.size; n; --n)
    {
        if (path.data[n - 1] == '/')
        {
            return nu_sv(path.data, n - 1);
        }
    }
    return path;
}
nu_sv_t
nu_path_concat (nu_char_t *buf, nu_size_t n, nu_sv_t p1, nu_sv_t p2)
{
    return nu_sv_fmt(
        buf, n, NU_SV_FMT "/" NU_SV_FMT, NU_SV_ARGS(p1), NU_SV_ARGS(p2));
}

#endif
