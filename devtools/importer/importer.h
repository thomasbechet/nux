#ifndef IMPORTER_H
#define IMPORTER_H

#include <nulib.h>

NU_API nu_byte_t *importer_load_image(nu_sv_t path, nu_v2u_t *size);
NU_API nu_f32_t  *importer_load_gltf(nu_sv_t path, nu_size_t *size);

#endif
