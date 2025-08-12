#include "internal.h"

#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

#define WATCH_MASK IN_MODIFY | IN_DONT_FOLLOW | IN_ATTRIB
#define EVENT_SIZE sizeof(struct inotify_event)
#define BUF_LEN    (1024 * (EVENT_SIZE + 16))

static struct
{
    int fd;
    struct
    {
        int             wd;
        const nux_c8_t *path;
        nux_res_t       handle;
    } entries[256];
    nux_u32_t entries_count;
} hotreload;

nux_status_t
hotreload_init (void)
{
    hotreload.fd = inotify_init1(IN_NONBLOCK);
    CHECK(hotreload.fd >= 0, return NUX_FAILURE);
    inotify_add_watch(hotreload.fd, ".", IN_MODIFY | IN_CREATE | IN_DELETE);
    hotreload.entries_count = 0;
    return NUX_SUCCESS;
}
void
hotreload_free (void)
{
    close(hotreload.fd);
}

void
nux_os_hotreload_add (void *userdata, const nux_c8_t *path, nux_res_t handle)
{
    const nux_c8_t *dir = dirname((char *)path);
    int             wd  = inotify_add_watch(
        hotreload.fd, dir, IN_MODIFY | IN_CREATE | IN_DELETE);
    if (hotreload.entries_count >= ARRAY_LEN(hotreload.entries))
    {
        fprintf(stderr, "out of hotreload resources");
        return;
    }
    hotreload.entries[hotreload.entries_count].path   = path;
    hotreload.entries[hotreload.entries_count].wd     = wd;
    hotreload.entries[hotreload.entries_count].handle = handle;
    ++hotreload.entries_count;
}
void
nux_os_hotreload_remove (void *userdata, nux_res_t handle)
{
    for (nux_u32_t i = 0; i < hotreload.entries_count; ++i)
    {
        if (hotreload.entries[i].handle == handle)
        {
            // swap remove
            hotreload.entries[i]
                = hotreload.entries[hotreload.entries_count - 1];
            return;
        }
    }
}
void
nux_os_hotreload_pull (void *userdata, nux_res_t *handles, nux_u32_t *count)
{
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
            if (event->mask & IN_MODIFY)
            {
                if (!(event->mask & IN_ISDIR))
                {
                    for (nux_u32_t j = 0; j < hotreload.entries_count; ++j)
                    {
                        if (hotreload.entries[j].wd == event->wd)
                        {
                            const nux_c8_t *bn
                                = basename((char *)hotreload.entries[j].path);
                            if (strcmp(bn, event->name) == 0)
                            {
                                handles[*count] = hotreload.entries[j].handle;
                                ++(*count);
                            }
                        }
                    }
                }
            }
        }
        i += EVENT_SIZE + event->len;
    }
}
