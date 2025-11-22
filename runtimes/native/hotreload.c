#include "internal.h"

#if defined(NUX_PLATFORM_UNIX)
#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

#define WATCH_MASK IN_MODIFY | IN_DONT_FOLLOW | IN_ATTRIB
#define EVENT_SIZE sizeof(struct inotify_event)
#define BUF_LEN    (1024 * (EVENT_SIZE + 16))
#endif

static struct
{
    int fd;
    struct
    {
        int             wd;
        const nux_c8_t *path;
        nux_id_t        id;
    } entries[256];
    nux_u32_t entries_count;
} hotreload;

nux_status_t
hotreload_init (void)
{
#if defined(NUX_PLATFORM_UNIX)
    hotreload.fd = inotify_init1(IN_NONBLOCK);
    nux_check(hotreload.fd >= 0, return NUX_FAILURE);
    inotify_add_watch(hotreload.fd, ".", IN_MODIFY | IN_CREATE | IN_DELETE);
    hotreload.entries_count = 0;
#endif
    return NUX_SUCCESS;
}
void
hotreload_free (void)
{
#if defined(NUX_PLATFORM_UNIX)
    close(hotreload.fd);
#endif
}

void
nux_os_hotreload_add (const nux_c8_t *path, nux_id_t id)
{
#if defined(NUX_PLATFORM_UNIX)
    nux_c8_t copypath[NUX_PATH_BUF_SIZE];
    memcpy(copypath, path, NUX_PATH_BUF_SIZE);
    const nux_c8_t *dir = dirname((char *)copypath);
    int             wd  = inotify_add_watch(
        hotreload.fd, dir, IN_MODIFY | IN_CREATE | IN_DELETE);
    if (hotreload.entries_count >= nux_array_size(hotreload.entries))
    {
        fprintf(stderr, "out of hotreload resources");
        return;
    }
    hotreload.entries[hotreload.entries_count].path = path;
    hotreload.entries[hotreload.entries_count].wd   = wd;
    hotreload.entries[hotreload.entries_count].id   = id;
    ++hotreload.entries_count;
#endif
}
void
nux_os_hotreload_remove (nux_id_t id)
{
#if defined(NUX_PLATFORM_UNIX)
    for (nux_u32_t i = 0; i < hotreload.entries_count; ++i)
    {
        if (hotreload.entries[i].id == id)
        {
            // swap remove
            hotreload.entries[i]
                = hotreload.entries[hotreload.entries_count - 1];
            --hotreload.entries_count;
            return;
        }
    }
#endif
}
void
nux_os_hotreload_pull (nux_id_t *ids, nux_u32_t *count)
{
#if defined(NUX_PLATFORM_UNIX)
    char                        buf[BUF_LEN];
    const struct inotify_event *event;
    *count  = 0;
    int len = read(hotreload.fd, buf, BUF_LEN);
    if (len == -1 && errno != EAGAIN)
    {
        fprintf(stderr, "inotify read error\n");
        return;
    }
    int i = 0;
    while (i < len)
    {
        struct inotify_event *event = (struct inotify_event *)&buf[i];
        if (event->len)
        {
            if (event->mask & IN_MODIFY && !(event->mask & IN_ISDIR))
            {
                for (nux_u32_t j = 0; j < hotreload.entries_count; ++j)
                {
                    if (hotreload.entries[j].wd == event->wd)
                    {
                        nux_c8_t copy[NUX_PATH_BUF_SIZE];
                        memcpy(copy, hotreload.entries[j].path, sizeof(copy));
                        const nux_c8_t *bn = basename(copy);
                        if (strcmp(bn, event->name) == 0)
                        {
                            ids[*count] = hotreload.entries[j].id;
                            ++(*count);
                        }
                    }
                }
            }
        }
        i += EVENT_SIZE + event->len;
    }
#else
    *count = 0;
#endif
}
