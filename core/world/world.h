#ifndef NUX_WORLD_H
#define NUX_WORLD_H

#include <serde/serde.h>

typedef nux_u32_t          nux_oid_t;
typedef struct nux_world_t nux_world_t;

typedef nux_u32_t (*nux_wcomponent_add_callback_t)(nux_oid_t oid);
typedef void (*nux_wcomponent_remove_callback_t)(nux_oid_t oid,
                                                 nux_u32_t handle);
typedef nux_status_t (*nux_wcomponent_read_callback_t)(nux_serde_reader_t *s,
                                                       void *data);
typedef nux_status_t (*nux_wcomponent_write_callback_t)(nux_serde_writer_t *s,
                                                        const void *data);

typedef struct
{
    const nux_c8_t                  *name;
    nux_wcomponent_add_callback_t    add;
    nux_wcomponent_remove_callback_t remove;
    nux_wcomponent_read_callback_t   read;
    nux_wcomponent_write_callback_t  write;
} nux_wcomponent_info_t;

nux_world_t *nux_world_new(nux_arena_t *a);
void         nux_world_set_active(nux_world_t *world);

void nux_wcomponent_register(nux_u32_t index, nux_wcomponent_info_t info);

nux_oid_t nux_object_create(void);
void      nux_object_delete(nux_oid_t oid);
void      nux_object_add(nux_oid_t oid, nux_u32_t type);
void      nux_object_remove(nux_oid_t oid, nux_u32_t type);
void      nux_object_attach(nux_oid_t oid, nux_oid_t parent);
nux_b32_t nux_object_has(nux_oid_t oid, nux_u32_t type);
nux_u32_t nux_object_get(nux_oid_t oid, nux_u32_t type);
void      nux_object_set(nux_oid_t oid, nux_u32_t type, nux_u32_t handle);

#endif
