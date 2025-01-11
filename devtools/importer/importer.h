#ifndef IMPORTER_H
#define IMPORTER_H

#include <nulib.h>

nu_byte_t *importer_load_image(nu_sv_t path, nu_v2u_t *size);
nu_f32_t  *importer_load_gltf(const nu_byte_t *path, nu_size_t *size);

#endif
