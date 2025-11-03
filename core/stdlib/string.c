#include "stdlib.h"

nux_u32_t
nux_strnlen (const nux_c8_t *s, nux_u32_t n)
{
    nux_u32_t i = 0;
    for (; (i < n) && s[i]; ++i)
        ;
    return i;
}
void
nux_strncpy (nux_c8_t *dst, const nux_c8_t *src, nux_u32_t n)
{
    nux_u32_t i = 0;
    while (i++ != n && (*dst++ = *src++))
        ;
}
nux_i32_t
nux_strncmp (const nux_c8_t *a, const nux_c8_t *b, nux_u32_t n)
{
    while (n && *a && (*a == *b))
    {
        ++a;
        ++b;
        --n;
    }
    if (n == 0)
    {
        return 0;
    }
    else
    {
        return (*(unsigned char *)a - *(unsigned char *)b);
    }
}
nux_f32_t
nux_strtof (const nux_c8_t *s, nux_c8_t **end)
{
#ifdef NUX_BUILD_STDLIB
    return strtof(s, end);
#endif
}
nux_c8_t *
nux_strdup (nux_arena_t *a, const nux_c8_t *s)
{
    if (!s)
    {
        return NUX_NULL;
    }
    nux_u32_t len = nux_strnlen(s, NUX_PATH_MAX);
    if (!len)
    {
        return NUX_NULL;
    }
    nux_c8_t *p = nux_arena_malloc(a, len + 1);
    NUX_CHECK(p, return NUX_NULL);
    nux_memcpy(p, s, len + 1); // include '\0'
    return p;
}
nux_u32_t
nux_snprintf (nux_c8_t *buf, nux_u32_t n, const nux_c8_t *format, ...)
{
#ifdef NUX_BUILD_STDLIB
    va_list args;
    va_start(args, format);
    nux_u32_t cn = nux_vsnprintf(buf, n, format, args);
    va_end(args);
    return cn;
#endif
}
nux_u32_t
nux_vsnprintf (nux_c8_t *buf, nux_u32_t n, const nux_c8_t *format, va_list args)
{
#ifdef NUX_BUILD_STDLIB
    return vsnprintf(buf, n, format, args);
#endif
}

nux_b32_t
nux_path_isdir (const nux_c8_t *path)
{
    nux_u32_t len = nux_strnlen(path, NUX_PATH_MAX);
    return len == 0 || path[len - 1] == '/';
}
nux_status_t
nux_path_concat (nux_c8_t *dst, const nux_c8_t *a, const nux_c8_t *b)
{
    if (!nux_path_isdir(a))
    {
        return NUX_FAILURE;
    }
    nux_u32_t a_len = nux_strnlen(a, NUX_PATH_MAX);
    nux_u32_t b_len = nux_strnlen(b, NUX_PATH_MAX);
    if (a_len + b_len + 1 > NUX_PATH_MAX)
    {
        return NUX_FAILURE;
    }
    nux_memcpy(dst, a, a_len);
    dst[a_len] = '/';
    nux_memcpy(dst + a_len + 1, b, b_len);
    dst[a_len + b_len + 1] = '\0';
    return NUX_SUCCESS;
}
nux_u32_t
nux_path_basename (nux_c8_t *dst, const nux_c8_t *path)
{
    nux_u32_t len = nux_strnlen(path, NUX_PATH_MAX);
    for (nux_u32_t n = len; n; --n)
    {
        if (path[n - 1] == '/')
        {
            nux_u32_t basename_len = len - n;
            nux_strncpy(dst, path + n, basename_len);
            return basename_len;
        }
    }
    nux_strncpy(dst, path, len);
    return len;
}
nux_u32_t
nux_path_normalize (nux_c8_t *dst, const nux_c8_t *path)
{
    nux_u32_t       len = nux_strnlen(path, NUX_PATH_MAX);
    nux_u32_t       i   = 0;
    const nux_c8_t *p   = path;
    while (*p)
    {
        if (*p == '/')
        {
            if (i == 0 || dst[i - 1] == '/')
            {
                ++p;
                continue;
            }
        }
        else if (*p == '.')
        {
            if (*(p + 1) == '/' || *(p + 1) == '.' || (i == len - 1))
            {
                ++p;
                continue;
            }
        }
        else if (*p == '\\' || *p == ' ' || *p == ':' || *p == '~' || *p == '<'
                 || *p == '>' || *p == '?' || *p == '*'
                 || *p == '|') // forbidden characters
        {
            ++p;
            continue;
        }
        dst[i] = *p;
        ++i;
        ++p;
    }
    dst[i] = '\0';
    return i;
}
nux_b32_t
nux_path_endswith (const nux_c8_t *path, const nux_c8_t *end)
{
    nux_u32_t path_length = nux_strnlen(path, NUX_PATH_MAX);
    nux_u32_t end_length  = nux_strnlen(end, NUX_PATH_MAX);
    if (end_length > path_length)
    {
        return NUX_FALSE;
    }
    for (nux_u32_t i = 0; i < end_length; ++i)
    {
        if (path[path_length - i] != end[end_length - i])
        {
            return NUX_FALSE;
        }
    }
    return NUX_TRUE;
}
nux_status_t
nux_path_set_extension (nux_c8_t *path, const nux_c8_t *extension)
{
    if (nux_path_isdir(path))
    {
        return NUX_FAILURE;
    }
    nux_u32_t path_length      = nux_strnlen(path, NUX_PATH_MAX);
    nux_u32_t extension_length = nux_strnlen(extension, NUX_PATH_MAX);
    for (nux_u32_t n = path_length; n; --n)
    {
        if (path[n] == '.')
        {
            if (n + extension_length + 1 > NUX_PATH_MAX)
            {
                return NUX_FAILURE;
            }
            nux_memcpy(path + n + 1, extension, extension_length);
            path[n + extension_length + 1] = '\0';
            return NUX_SUCCESS;
        }
    }
    if (path_length) // no extension, simply concatenate extension
    {
        if (path_length + extension_length + 1 > NUX_PATH_MAX)
        {
            return NUX_FAILURE;
        }
        path[path_length] = '.';
        nux_memcpy(path + path_length + 1, extension, extension_length);
        path[path_length + extension_length + 1] = '\0';
        return NUX_SUCCESS;
    }
    return NUX_FAILURE;
}
void
nux_path_copy (nux_c8_t *dst, const nux_c8_t *src)
{
    nux_memcpy(dst, src, NUX_PATH_BUF_SIZE);
}
