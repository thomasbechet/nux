#ifndef SDK_TEMPLATES_DATA_H
#define SDK_TEMPLATES_DATA_H
#include <nulib/nulib.h>
typedef struct
{const nu_char_t *path; const nu_byte_t *data; nu_size_t size; } sdk_template_file_t;
static const nu_byte_t template_c__gitignore_data[] = {
  0x62, 0x75, 0x69, 0x6c, 0x64, 0x2f, 0x2a, 0x2a, 0x0a, 0x63, 0x61, 0x72,
  0x74, 0x2e, 0x62, 0x69, 0x6e, 0x0a};
static const nu_byte_t template_c_Makefile_data[] = {
  0x69, 0x66, 0x6e, 0x64, 0x65, 0x66, 0x20, 0x57, 0x41, 0x53, 0x49, 0x5f,
  0x53, 0x44, 0x4b, 0x5f, 0x50, 0x41, 0x54, 0x48, 0x0a, 0x24, 0x28, 0x65,
  0x72, 0x72, 0x6f, 0x72, 0x20, 0x57, 0x41, 0x53, 0x49, 0x20, 0x53, 0x44,
  0x4b, 0x20, 0x72, 0x65, 0x71, 0x75, 0x69, 0x72, 0x65, 0x64, 0x3a, 0x20,
  0x49, 0x6e, 0x73, 0x74, 0x61, 0x6c, 0x6c, 0x20, 0x68, 0x74, 0x74, 0x70,
  0x73, 0x3a, 0x2f, 0x2f, 0x67, 0x69, 0x74, 0x68, 0x75, 0x62, 0x2e, 0x63,
  0x6f, 0x6d, 0x2f, 0x57, 0x65, 0x62, 0x41, 0x73, 0x73, 0x65, 0x6d, 0x62,
  0x6c, 0x79, 0x2f, 0x77, 0x61, 0x73, 0x69, 0x2d, 0x73, 0x64, 0x6b, 0x20,
  0x61, 0x6e, 0x64, 0x20, 0x73, 0x65, 0x74, 0x20, 0x57, 0x41, 0x53, 0x49,
  0x5f, 0x53, 0x44, 0x4b, 0x5f, 0x50, 0x41, 0x54, 0x48, 0x29, 0x0a, 0x65,
  0x6e, 0x64, 0x69, 0x66, 0x0a, 0x0a, 0x44, 0x45, 0x42, 0x55, 0x47, 0x20,
  0x3d, 0x20, 0x30, 0x0a, 0x0a, 0x43, 0x43, 0x20, 0x3d, 0x20, 0x22, 0x24,
  0x28, 0x57, 0x41, 0x53, 0x49, 0x5f, 0x53, 0x44, 0x4b, 0x5f, 0x50, 0x41,
  0x54, 0x48, 0x29, 0x2f, 0x62, 0x69, 0x6e, 0x2f, 0x63, 0x6c, 0x61, 0x6e,
  0x67, 0x22, 0x20, 0x2d, 0x2d, 0x73, 0x79, 0x73, 0x72, 0x6f, 0x6f, 0x74,
  0x3d, 0x22, 0x24, 0x28, 0x57, 0x41, 0x53, 0x49, 0x5f, 0x53, 0x44, 0x4b,
  0x5f, 0x50, 0x41, 0x54, 0x48, 0x29, 0x2f, 0x73, 0x68, 0x61, 0x72, 0x65,
  0x2f, 0x77, 0x61, 0x73, 0x69, 0x2d, 0x73, 0x79, 0x73, 0x72, 0x6f, 0x6f,
  0x74, 0x22, 0x0a, 0x0a, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x4f, 0x50, 0x54,
  0x20, 0x3d, 0x20, 0x77, 0x61, 0x73, 0x6d, 0x2d, 0x6f, 0x70, 0x74, 0x0a,
  0x57, 0x41, 0x53, 0x4d, 0x5f, 0x4f, 0x50, 0x54, 0x5f, 0x46, 0x4c, 0x41,
  0x47, 0x53, 0x20, 0x3d, 0x20, 0x2d, 0x4f, 0x7a, 0x20, 0x2d, 0x2d, 0x7a,
  0x65, 0x72, 0x6f, 0x2d, 0x66, 0x69, 0x6c, 0x6c, 0x65, 0x64, 0x2d, 0x6d,
  0x65, 0x6d, 0x6f, 0x72, 0x79, 0x20, 0x2d, 0x2d, 0x73, 0x74, 0x72, 0x69,
  0x70, 0x2d, 0x70, 0x72, 0x6f, 0x64, 0x75, 0x63, 0x65, 0x72, 0x73, 0x20,
  0x2d, 0x2d, 0x65, 0x6e, 0x61, 0x62, 0x6c, 0x65, 0x2d, 0x62, 0x75, 0x6c,
  0x6b, 0x2d, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x0a, 0x0a, 0x43, 0x46,
  0x4c, 0x41, 0x47, 0x53, 0x20, 0x3d, 0x20, 0x2d, 0x57, 0x20, 0x2d, 0x57,
  0x61, 0x6c, 0x6c, 0x20, 0x2d, 0x57, 0x65, 0x78, 0x74, 0x72, 0x61, 0x20,
  0x2d, 0x57, 0x6e, 0x6f, 0x2d, 0x75, 0x6e, 0x75, 0x73, 0x65, 0x64, 0x20,
  0x2d, 0x4d, 0x4d, 0x44, 0x20, 0x2d, 0x4d, 0x50, 0x20, 0x2d, 0x66, 0x6e,
  0x6f, 0x2d, 0x65, 0x78, 0x63, 0x65, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x73,
  0x20, 0x2d, 0x6d, 0x62, 0x75, 0x6c, 0x6b, 0x2d, 0x6d, 0x65, 0x6d, 0x6f,
  0x72, 0x79, 0x20, 0x2d, 0x49, 0x73, 0x72, 0x63, 0x0a, 0x69, 0x66, 0x65,
  0x71, 0x20, 0x28, 0x24, 0x28, 0x44, 0x45, 0x42, 0x55, 0x47, 0x29, 0x2c,
  0x20, 0x31, 0x29, 0x0a, 0x09, 0x43, 0x46, 0x4c, 0x41, 0x47, 0x53, 0x20,
  0x2b, 0x3d, 0x20, 0x2d, 0x44, 0x44, 0x45, 0x42, 0x55, 0x47, 0x20, 0x2d,
  0x4f, 0x30, 0x20, 0x2d, 0x67, 0x0a, 0x65, 0x6c, 0x73, 0x65, 0x0a, 0x09,
  0x43, 0x46, 0x4c, 0x41, 0x47, 0x53, 0x20, 0x2b, 0x3d, 0x20, 0x2d, 0x44,
  0x4e, 0x44, 0x45, 0x42, 0x55, 0x47, 0x20, 0x2d, 0x4f, 0x7a, 0x20, 0x2d,
  0x66, 0x6c, 0x74, 0x6f, 0x0a, 0x65, 0x6e, 0x64, 0x69, 0x66, 0x0a, 0x0a,
  0x4c, 0x44, 0x46, 0x4c, 0x41, 0x47, 0x53, 0x20, 0x3d, 0x20, 0x2d, 0x57,
  0x6c, 0x2c, 0x2d, 0x2d, 0x6e, 0x6f, 0x2d, 0x65, 0x6e, 0x74, 0x72, 0x79,
  0x2c, 0x2d, 0x2d, 0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x3d, 0x73, 0x74,
  0x61, 0x72, 0x74, 0x2c, 0x2d, 0x2d, 0x65, 0x78, 0x70, 0x6f, 0x72, 0x74,
  0x3d, 0x75, 0x70, 0x64, 0x61, 0x74, 0x65, 0x2c, 0x2d, 0x2d, 0x61, 0x6c,
  0x6c, 0x6f, 0x77, 0x2d, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65,
  0x64, 0x0a, 0x69, 0x66, 0x65, 0x71, 0x20, 0x28, 0x24, 0x28, 0x44, 0x45,
  0x42, 0x55, 0x47, 0x29, 0x2c, 0x20, 0x31, 0x29, 0x0a, 0x09, 0x4c, 0x44,
  0x46, 0x4c, 0x41, 0x47, 0x53, 0x20, 0x2b, 0x3d, 0x20, 0x2d, 0x57, 0x6c,
  0x2c, 0x2d, 0x2d, 0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x2d, 0x61, 0x6c,
  0x6c, 0x2c, 0x2d, 0x2d, 0x6e, 0x6f, 0x2d, 0x67, 0x63, 0x2d, 0x73, 0x65,
  0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x0a, 0x65, 0x6c, 0x73, 0x65, 0x0a,
  0x09, 0x4c, 0x44, 0x46, 0x4c, 0x41, 0x47, 0x53, 0x20, 0x2b, 0x3d, 0x20,
  0x2d, 0x57, 0x6c, 0x2c, 0x2d, 0x2d, 0x73, 0x74, 0x72, 0x69, 0x70, 0x2d,
  0x61, 0x6c, 0x6c, 0x2c, 0x2d, 0x2d, 0x67, 0x63, 0x2d, 0x73, 0x65, 0x63,
  0x74, 0x69, 0x6f, 0x6e, 0x73, 0x2c, 0x2d, 0x2d, 0x6c, 0x74, 0x6f, 0x2d,
  0x4f, 0x33, 0x20, 0x2d, 0x4f, 0x7a, 0x0a, 0x65, 0x6e, 0x64, 0x69, 0x66,
  0x0a, 0x0a, 0x4f, 0x42, 0x4a, 0x45, 0x43, 0x54, 0x53, 0x20, 0x3d, 0x20,
  0x24, 0x28, 0x70, 0x61, 0x74, 0x73, 0x75, 0x62, 0x73, 0x74, 0x20, 0x73,
  0x72, 0x63, 0x2f, 0x25, 0x2e, 0x63, 0x2c, 0x20, 0x62, 0x75, 0x69, 0x6c,
  0x64, 0x2f, 0x25, 0x2e, 0x6f, 0x2c, 0x20, 0x24, 0x28, 0x77, 0x69, 0x6c,
  0x64, 0x63, 0x61, 0x72, 0x64, 0x20, 0x73, 0x72, 0x63, 0x2f, 0x2a, 0x2e,
  0x63, 0x29, 0x29, 0x0a, 0x44, 0x45, 0x50, 0x53, 0x20, 0x3d, 0x20, 0x24,
  0x28, 0x4f, 0x42, 0x4a, 0x45, 0x43, 0x54, 0x53, 0x3a, 0x2e, 0x6f, 0x3d,
  0x2e, 0x64, 0x29, 0x0a, 0x0a, 0x69, 0x66, 0x65, 0x71, 0x20, 0x27, 0x24,
  0x28, 0x66, 0x69, 0x6e, 0x64, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x20,
  0x3b, 0x2c, 0x24, 0x28, 0x50, 0x41, 0x54, 0x48, 0x29, 0x29, 0x27, 0x20,
  0x27, 0x3b, 0x27, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x44, 0x45, 0x54, 0x45,
  0x43, 0x54, 0x45, 0x44, 0x5f, 0x4f, 0x53, 0x20, 0x3a, 0x3d, 0x20, 0x57,
  0x69, 0x6e, 0x64, 0x6f, 0x77, 0x73, 0x0a, 0x65, 0x6c, 0x73, 0x65, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x44, 0x45, 0x54, 0x45, 0x43, 0x54, 0x45, 0x44,
  0x5f, 0x4f, 0x53, 0x20, 0x3a, 0x3d, 0x20, 0x24, 0x28, 0x73, 0x68, 0x65,
  0x6c, 0x6c, 0x20, 0x75, 0x6e, 0x61, 0x6d, 0x65, 0x20, 0x32, 0x3e, 0x2f,
  0x64, 0x65, 0x76, 0x2f, 0x6e, 0x75, 0x6c, 0x6c, 0x20, 0x7c, 0x7c, 0x20,
  0x65, 0x63, 0x68, 0x6f, 0x20, 0x55, 0x6e, 0x6b, 0x6e, 0x6f, 0x77, 0x6e,
  0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x44, 0x45, 0x54, 0x45, 0x43, 0x54,
  0x45, 0x44, 0x5f, 0x4f, 0x53, 0x20, 0x3a, 0x3d, 0x20, 0x24, 0x28, 0x70,
  0x61, 0x74, 0x73, 0x75, 0x62, 0x73, 0x74, 0x20, 0x43, 0x59, 0x47, 0x57,
  0x49, 0x4e, 0x25, 0x2c, 0x43, 0x79, 0x67, 0x77, 0x69, 0x6e, 0x2c, 0x24,
  0x28, 0x44, 0x45, 0x54, 0x45, 0x43, 0x54, 0x45, 0x44, 0x5f, 0x4f, 0x53,
  0x29, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x44, 0x45, 0x54, 0x45, 0x43,
  0x54, 0x45, 0x44, 0x5f, 0x4f, 0x53, 0x20, 0x3a, 0x3d, 0x20, 0x24, 0x28,
  0x70, 0x61, 0x74, 0x73, 0x75, 0x62, 0x73, 0x74, 0x20, 0x4d, 0x53, 0x59,
  0x53, 0x25, 0x2c, 0x4d, 0x53, 0x59, 0x53, 0x2c, 0x24, 0x28, 0x44, 0x45,
  0x54, 0x45, 0x43, 0x54, 0x45, 0x44, 0x5f, 0x4f, 0x53, 0x29, 0x29, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x44, 0x45, 0x54, 0x45, 0x43, 0x54, 0x45, 0x44,
  0x5f, 0x4f, 0x53, 0x20, 0x3a, 0x3d, 0x20, 0x24, 0x28, 0x70, 0x61, 0x74,
  0x73, 0x75, 0x62, 0x73, 0x74, 0x20, 0x4d, 0x49, 0x4e, 0x47, 0x57, 0x25,
  0x2c, 0x4d, 0x53, 0x59, 0x53, 0x2c, 0x24, 0x28, 0x44, 0x45, 0x54, 0x45,
  0x43, 0x54, 0x45, 0x44, 0x5f, 0x4f, 0x53, 0x29, 0x29, 0x0a, 0x65, 0x6e,
  0x64, 0x69, 0x66, 0x0a, 0x0a, 0x69, 0x66, 0x65, 0x71, 0x20, 0x28, 0x24,
  0x28, 0x44, 0x45, 0x54, 0x45, 0x43, 0x54, 0x45, 0x44, 0x5f, 0x4f, 0x53,
  0x29, 0x2c, 0x20, 0x57, 0x69, 0x6e, 0x64, 0x6f, 0x77, 0x73, 0x29, 0x0a,
  0x09, 0x4d, 0x4b, 0x44, 0x49, 0x52, 0x5f, 0x42, 0x55, 0x49, 0x4c, 0x44,
  0x20, 0x3d, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78,
  0x69, 0x73, 0x74, 0x20, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x20, 0x6d, 0x64,
  0x20, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x0a, 0x09, 0x52, 0x4d, 0x44, 0x49,
  0x52, 0x20, 0x3d, 0x20, 0x72, 0x64, 0x20, 0x2f, 0x73, 0x20, 0x2f, 0x71,
  0x0a, 0x65, 0x6c, 0x73, 0x65, 0x0a, 0x09, 0x4d, 0x4b, 0x44, 0x49, 0x52,
  0x5f, 0x42, 0x55, 0x49, 0x4c, 0x44, 0x20, 0x3d, 0x20, 0x6d, 0x6b, 0x64,
  0x69, 0x72, 0x20, 0x2d, 0x70, 0x20, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x0a,
  0x09, 0x52, 0x4d, 0x44, 0x49, 0x52, 0x20, 0x3d, 0x20, 0x72, 0x6d, 0x20,
  0x2d, 0x72, 0x66, 0x0a, 0x65, 0x6e, 0x64, 0x69, 0x66, 0x0a, 0x0a, 0x61,
  0x6c, 0x6c, 0x3a, 0x20, 0x63, 0x6c, 0x65, 0x61, 0x6e, 0x20, 0x62, 0x75,
  0x69, 0x6c, 0x64, 0x2f, 0x63, 0x61, 0x72, 0x74, 0x2e, 0x77, 0x61, 0x73,
  0x6d, 0x0a, 0x0a, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x2f, 0x63, 0x61, 0x72,
  0x74, 0x2e, 0x77, 0x61, 0x73, 0x6d, 0x3a, 0x20, 0x24, 0x28, 0x4f, 0x42,
  0x4a, 0x45, 0x43, 0x54, 0x53, 0x29, 0x0a, 0x09, 0x24, 0x28, 0x43, 0x43,
  0x29, 0x20, 0x2d, 0x6f, 0x20, 0x24, 0x40, 0x20, 0x24, 0x28, 0x4f, 0x42,
  0x4a, 0x45, 0x43, 0x54, 0x53, 0x29, 0x20, 0x24, 0x28, 0x4c, 0x44, 0x46,
  0x4c, 0x41, 0x47, 0x53, 0x29, 0x0a, 0x69, 0x66, 0x6e, 0x65, 0x71, 0x20,
  0x28, 0x24, 0x28, 0x44, 0x45, 0x42, 0x55, 0x47, 0x29, 0x2c, 0x20, 0x31,
  0x29, 0x0a, 0x69, 0x66, 0x65, 0x71, 0x20, 0x28, 0x2c, 0x20, 0x24, 0x28,
  0x73, 0x68, 0x65, 0x6c, 0x6c, 0x20, 0x63, 0x6f, 0x6d, 0x6d, 0x61, 0x6e,
  0x64, 0x20, 0x2d, 0x76, 0x20, 0x24, 0x28, 0x57, 0x41, 0x53, 0x4d, 0x5f,
  0x4f, 0x50, 0x54, 0x29, 0x29, 0x29, 0x0a, 0x09, 0x40, 0x65, 0x63, 0x68,
  0x6f, 0x20, 0x54, 0x69, 0x70, 0x3a, 0x20, 0x24, 0x28, 0x57, 0x41, 0x53,
  0x4d, 0x5f, 0x4f, 0x50, 0x54, 0x29, 0x20, 0x77, 0x61, 0x73, 0x20, 0x6e,
  0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x2e, 0x20, 0x49, 0x6e,
  0x73, 0x74, 0x61, 0x6c, 0x6c, 0x20, 0x69, 0x74, 0x20, 0x66, 0x72, 0x6f,
  0x6d, 0x20, 0x62, 0x69, 0x6e, 0x61, 0x72, 0x79, 0x65, 0x6e, 0x20, 0x66,
  0x6f, 0x72, 0x20, 0x73, 0x6d, 0x61, 0x6c, 0x6c, 0x65, 0x72, 0x20, 0x62,
  0x75, 0x69, 0x6c, 0x64, 0x73, 0x21, 0x0a, 0x65, 0x6c, 0x73, 0x65, 0x0a,
  0x09, 0x24, 0x28, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x4f, 0x50, 0x54, 0x29,
  0x20, 0x24, 0x28, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x4f, 0x50, 0x54, 0x5f,
  0x46, 0x4c, 0x41, 0x47, 0x53, 0x29, 0x20, 0x24, 0x40, 0x20, 0x2d, 0x6f,
  0x20, 0x24, 0x40, 0x0a, 0x65, 0x6e, 0x64, 0x69, 0x66, 0x0a, 0x65, 0x6e,
  0x64, 0x69, 0x66, 0x0a, 0x0a, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x2f, 0x25,
  0x2e, 0x6f, 0x3a, 0x20, 0x73, 0x72, 0x63, 0x2f, 0x25, 0x2e, 0x63, 0x0a,
  0x09, 0x40, 0x24, 0x28, 0x4d, 0x4b, 0x44, 0x49, 0x52, 0x5f, 0x42, 0x55,
  0x49, 0x4c, 0x44, 0x29, 0x0a, 0x09, 0x24, 0x28, 0x43, 0x43, 0x29, 0x20,
  0x2d, 0x63, 0x20, 0x24, 0x3c, 0x20, 0x2d, 0x6f, 0x20, 0x24, 0x40, 0x20,
  0x24, 0x28, 0x43, 0x46, 0x4c, 0x41, 0x47, 0x53, 0x29, 0x0a, 0x0a, 0x2e,
  0x50, 0x48, 0x4f, 0x4e, 0x59, 0x3a, 0x20, 0x63, 0x6c, 0x65, 0x61, 0x6e,
  0x0a, 0x63, 0x6c, 0x65, 0x61, 0x6e, 0x3a, 0x0a, 0x09, 0x24, 0x28, 0x52,
  0x4d, 0x44, 0x49, 0x52, 0x29, 0x20, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x0a,
  0x0a, 0x2d, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x24, 0x28,
  0x44, 0x45, 0x50, 0x53, 0x29, 0x0a};
static const nu_byte_t template_c_nux_json_data[] = {
  0x7b, 0x0a, 0x09, 0x22, 0x74, 0x61, 0x72, 0x67, 0x65, 0x74, 0x22, 0x3a,
  0x20, 0x22, 0x63, 0x61, 0x72, 0x74, 0x2e, 0x62, 0x69, 0x6e, 0x22, 0x2c,
  0x0a, 0x09, 0x22, 0x70, 0x72, 0x65, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x22,
  0x3a, 0x20, 0x22, 0x6d, 0x61, 0x6b, 0x65, 0x22, 0x2c, 0x0a, 0x09, 0x22,
  0x63, 0x68, 0x75, 0x6e, 0x6b, 0x73, 0x22, 0x3a, 0x20, 0x5b, 0x0a, 0x09,
  0x09, 0x7b, 0x0a, 0x09, 0x09, 0x09, 0x22, 0x74, 0x79, 0x70, 0x65, 0x22,
  0x3a, 0x20, 0x22, 0x77, 0x61, 0x73, 0x6d, 0x22, 0x2c, 0x0a, 0x09, 0x09,
  0x09, 0x22, 0x73, 0x6f, 0x75, 0x72, 0x63, 0x65, 0x22, 0x3a, 0x20, 0x22,
  0x62, 0x75, 0x69, 0x6c, 0x64, 0x2f, 0x63, 0x61, 0x72, 0x74, 0x2e, 0x77,
  0x61, 0x73, 0x6d, 0x22, 0x0a, 0x09, 0x09, 0x7d, 0x0a, 0x09, 0x5d, 0x0a,
  0x7d, 0x0a};
static const nu_byte_t template_c_main_c_data[] = {
  0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x22, 0x6e, 0x75,
  0x78, 0x2e, 0x68, 0x22, 0x0a, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x0a, 0x73,
  0x74, 0x61, 0x72, 0x74, 0x20, 0x28, 0x76, 0x6f, 0x69, 0x64, 0x29, 0x0a,
  0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x74, 0x72, 0x61, 0x63, 0x65, 0x28,
  0x22, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64,
  0x20, 0x21, 0x22, 0x2c, 0x20, 0x31, 0x33, 0x29, 0x3b, 0x0a, 0x7d, 0x0a,
  0x0a, 0x76, 0x6f, 0x69, 0x64, 0x0a, 0x75, 0x70, 0x64, 0x61, 0x74, 0x65,
  0x20, 0x28, 0x76, 0x6f, 0x69, 0x64, 0x29, 0x0a, 0x7b, 0x0a, 0x7d, 0x0a};
static const nu_byte_t template_c_nux_h_data[] = {
  0x23, 0x69, 0x66, 0x6e, 0x64, 0x65, 0x66, 0x20, 0x4e, 0x55, 0x58, 0x5f,
  0x48, 0x0a, 0x23, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x20, 0x4e, 0x55,
  0x58, 0x5f, 0x48, 0x0a, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x43, 0x6f, 0x72, 0x65, 0x20, 0x54, 0x79, 0x70,
  0x65, 0x73, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x0a, 0x0a, 0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20,
  0x3c, 0x73, 0x74, 0x64, 0x69, 0x6e, 0x74, 0x2e, 0x68, 0x3e, 0x0a, 0x0a,
  0x74, 0x79, 0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x69, 0x6e, 0x74, 0x33,
  0x32, 0x5f, 0x74, 0x20, 0x20, 0x69, 0x33, 0x32, 0x3b, 0x0a, 0x74, 0x79,
  0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x75, 0x69, 0x6e, 0x74, 0x33, 0x32,
  0x5f, 0x74, 0x20, 0x75, 0x33, 0x32, 0x3b, 0x0a, 0x74, 0x79, 0x70, 0x65,
  0x64, 0x65, 0x66, 0x20, 0x69, 0x6e, 0x74, 0x36, 0x34, 0x5f, 0x74, 0x20,
  0x20, 0x69, 0x36, 0x34, 0x3b, 0x0a, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65,
  0x66, 0x20, 0x75, 0x69, 0x6e, 0x74, 0x36, 0x34, 0x5f, 0x74, 0x20, 0x75,
  0x36, 0x34, 0x3b, 0x0a, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65, 0x66, 0x20,
  0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x20, 0x20, 0x20, 0x66, 0x33, 0x32,
  0x3b, 0x0a, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x64, 0x6f,
  0x75, 0x62, 0x6c, 0x65, 0x20, 0x20, 0x20, 0x66, 0x36, 0x34, 0x3b, 0x0a,
  0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x45, 0x78,
  0x70, 0x6f, 0x72, 0x74, 0x20, 0x4d, 0x61, 0x63, 0x72, 0x6f, 0x73, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x0a, 0x23,
  0x69, 0x66, 0x20, 0x28, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x28,
  0x5f, 0x5f, 0x57, 0x49, 0x4e, 0x33, 0x32, 0x5f, 0x5f, 0x29, 0x20, 0x7c,
  0x7c, 0x20, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x28, 0x57, 0x49,
  0x4e, 0x33, 0x32, 0x29, 0x20, 0x7c, 0x7c, 0x20, 0x64, 0x65, 0x66, 0x69,
  0x6e, 0x65, 0x64, 0x28, 0x5f, 0x5f, 0x4d, 0x49, 0x4e, 0x47, 0x57, 0x33,
  0x32, 0x5f, 0x5f, 0x29, 0x20, 0x5c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x7c, 0x7c, 0x20, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x28, 0x5f,
  0x57, 0x49, 0x4e, 0x33, 0x32, 0x29, 0x29, 0x0a, 0x23, 0x64, 0x65, 0x66,
  0x69, 0x6e, 0x65, 0x20, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x45, 0x58, 0x50,
  0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x20, 0x5f, 0x5f,
  0x64, 0x65, 0x63, 0x6c, 0x73, 0x70, 0x65, 0x63, 0x28, 0x64, 0x6c, 0x6c,
  0x65, 0x78, 0x70, 0x6f, 0x72, 0x74, 0x29, 0x0a, 0x23, 0x64, 0x65, 0x66,
  0x69, 0x6e, 0x65, 0x20, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x49, 0x4d, 0x50,
  0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x0a, 0x23, 0x65,
  0x6c, 0x69, 0x66, 0x20, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x28,
  0x5f, 0x5f, 0x6c, 0x69, 0x6e, 0x75, 0x78, 0x5f, 0x5f, 0x29, 0x20, 0x7c,
  0x7c, 0x20, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x28, 0x5f, 0x5f,
  0x75, 0x6e, 0x69, 0x78, 0x5f, 0x5f, 0x29, 0x20, 0x7c, 0x7c, 0x20, 0x64,
  0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x28, 0x5f, 0x5f, 0x6c, 0x69, 0x6e,
  0x75, 0x78, 0x29, 0x0a, 0x23, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x20,
  0x57, 0x41, 0x53, 0x4d, 0x5f, 0x45, 0x58, 0x50, 0x4f, 0x52, 0x54, 0x28,
  0x6e, 0x61, 0x6d, 0x65, 0x29, 0x20, 0x5f, 0x5f, 0x61, 0x74, 0x74, 0x72,
  0x69, 0x62, 0x75, 0x74, 0x65, 0x5f, 0x5f, 0x28, 0x28, 0x76, 0x69, 0x73,
  0x69, 0x62, 0x69, 0x6c, 0x69, 0x74, 0x79, 0x28, 0x22, 0x64, 0x65, 0x66,
  0x61, 0x75, 0x6c, 0x74, 0x22, 0x29, 0x29, 0x29, 0x0a, 0x23, 0x64, 0x65,
  0x66, 0x69, 0x6e, 0x65, 0x20, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x49, 0x4d,
  0x50, 0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x0a, 0x23,
  0x65, 0x6c, 0x69, 0x66, 0x20, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64,
  0x28, 0x5f, 0x5f, 0x41, 0x50, 0x50, 0x4c, 0x45, 0x5f, 0x5f, 0x29, 0x0a,
  0x23, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x20, 0x57, 0x41, 0x53, 0x4d,
  0x5f, 0x45, 0x58, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61, 0x6d, 0x65,
  0x29, 0x0a, 0x23, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x20, 0x57, 0x41,
  0x53, 0x4d, 0x5f, 0x49, 0x4d, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61,
  0x6d, 0x65, 0x29, 0x0a, 0x23, 0x65, 0x6c, 0x69, 0x66, 0x20, 0x64, 0x65,
  0x66, 0x69, 0x6e, 0x65, 0x64, 0x28, 0x5f, 0x50, 0x4f, 0x53, 0x49, 0x58,
  0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x29, 0x0a, 0x23, 0x64,
  0x65, 0x66, 0x69, 0x6e, 0x65, 0x20, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x45,
  0x58, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x0a,
  0x23, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x20, 0x57, 0x41, 0x53, 0x4d,
  0x5f, 0x49, 0x4d, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61, 0x6d, 0x65,
  0x29, 0x0a, 0x23, 0x65, 0x6c, 0x73, 0x65, 0x0a, 0x23, 0x64, 0x65, 0x66,
  0x69, 0x6e, 0x65, 0x20, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x45, 0x58, 0x50,
  0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x0a, 0x23, 0x64,
  0x65, 0x66, 0x69, 0x6e, 0x65, 0x20, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x49,
  0x4d, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x0a,
  0x23, 0x65, 0x6e, 0x64, 0x69, 0x66, 0x0a, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x4d, 0x4f, 0x44,
  0x55, 0x4c, 0x45, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x0a, 0x2f, 0x2f, 0x20, 0x4d, 0x6f, 0x64,
  0x75, 0x6c, 0x65, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74, 0x20, 0x63, 0x61,
  0x6c, 0x6c, 0x62, 0x61, 0x63, 0x6b, 0x0a, 0x57, 0x41, 0x53, 0x4d, 0x5f,
  0x45, 0x58, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x22, 0x73, 0x74, 0x61, 0x72,
  0x74, 0x22, 0x29, 0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x73, 0x74, 0x61,
  0x72, 0x74, 0x28, 0x29, 0x3b, 0x0a, 0x2f, 0x2f, 0x20, 0x4d, 0x6f, 0x64,
  0x75, 0x6c, 0x65, 0x20, 0x75, 0x70, 0x64, 0x61, 0x74, 0x65, 0x20, 0x63,
  0x61, 0x6c, 0x6c, 0x62, 0x61, 0x63, 0x6b, 0x0a, 0x57, 0x41, 0x53, 0x4d,
  0x5f, 0x45, 0x58, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x22, 0x75, 0x70, 0x64,
  0x61, 0x74, 0x65, 0x22, 0x29, 0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x75,
  0x70, 0x64, 0x61, 0x74, 0x65, 0x28, 0x29, 0x3b, 0x0a, 0x0a, 0x2f, 0x2f,
  0x20, 0x54, 0x72, 0x61, 0x63, 0x65, 0x20, 0x61, 0x20, 0x6d, 0x65, 0x73,
  0x73, 0x61, 0x67, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x74, 0x68, 0x65, 0x20,
  0x72, 0x75, 0x6e, 0x74, 0x69, 0x6d, 0x65, 0x20, 0x28, 0x55, 0x54, 0x46,
  0x2d, 0x38, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x29, 0x0a, 0x57,
  0x41, 0x53, 0x4d, 0x5f, 0x49, 0x4d, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x22,
  0x74, 0x72, 0x61, 0x63, 0x65, 0x22, 0x29, 0x20, 0x76, 0x6f, 0x69, 0x64,
  0x20, 0x74, 0x72, 0x61, 0x63, 0x65, 0x28, 0x63, 0x6f, 0x6e, 0x73, 0x74,
  0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x2a, 0x73, 0x74, 0x72, 0x2c, 0x20,
  0x75, 0x33, 0x32, 0x20, 0x6e, 0x29, 0x3b, 0x0a, 0x0a, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x47, 0x50, 0x55, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x0a, 0x2f, 0x2f, 0x20, 0x52, 0x65,
  0x73, 0x6f, 0x75, 0x72, 0x63, 0x65, 0x73, 0x0a, 0x57, 0x41, 0x53, 0x4d,
  0x5f, 0x49, 0x4d, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x22, 0x77, 0x72, 0x69,
  0x74, 0x65, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x22, 0x29,
  0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x77, 0x72, 0x69, 0x74, 0x65, 0x5f,
  0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x28, 0x75, 0x33, 0x32, 0x20,
  0x73, 0x6c, 0x6f, 0x74, 0x2c, 0x20, 0x75, 0x33, 0x32, 0x20, 0x78, 0x2c,
  0x20, 0x75, 0x33, 0x32, 0x20, 0x79, 0x2c, 0x20, 0x75, 0x33, 0x32, 0x20,
  0x77, 0x2c, 0x20, 0x75, 0x33, 0x32, 0x20, 0x68, 0x2c, 0x20, 0x63, 0x6f,
  0x6e, 0x73, 0x74, 0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x2a, 0x70, 0x29,
  0x3b, 0x0a, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x49, 0x4d, 0x50, 0x4f, 0x52,
  0x54, 0x28, 0x22, 0x77, 0x72, 0x69, 0x74, 0x65, 0x5f, 0x76, 0x65, 0x72,
  0x74, 0x65, 0x78, 0x22, 0x29, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x77,
  0x72, 0x69, 0x74, 0x65, 0x5f, 0x76, 0x65, 0x72, 0x74, 0x65, 0x78, 0x28,
  0x75, 0x33, 0x32, 0x20, 0x66, 0x69, 0x72, 0x73, 0x74, 0x2c, 0x20, 0x75,
  0x33, 0x32, 0x20, 0x63, 0x6f, 0x75, 0x6e, 0x74, 0x2c, 0x20, 0x63, 0x6f,
  0x6e, 0x73, 0x74, 0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x2a, 0x70, 0x29,
  0x3b, 0x0a, 0x0a, 0x2f, 0x2f, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65, 0x0a,
  0x57, 0x41, 0x53, 0x4d, 0x5f, 0x49, 0x4d, 0x50, 0x4f, 0x52, 0x54, 0x28,
  0x22, 0x62, 0x69, 0x6e, 0x64, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72,
  0x65, 0x22, 0x29, 0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x62, 0x69, 0x6e,
  0x64, 0x5f, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x28, 0x75, 0x33,
  0x32, 0x20, 0x73, 0x6c, 0x6f, 0x74, 0x29, 0x3b, 0x0a, 0x0a, 0x2f, 0x2f,
  0x20, 0x43, 0x6f, 0x6d, 0x6d, 0x61, 0x6e, 0x64, 0x73, 0x0a, 0x57, 0x41,
  0x53, 0x4d, 0x5f, 0x49, 0x4d, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x22, 0x64,
  0x72, 0x61, 0x77, 0x22, 0x29, 0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x64,
  0x72, 0x61, 0x77, 0x28, 0x75, 0x33, 0x32, 0x20, 0x66, 0x69, 0x72, 0x73,
  0x74, 0x2c, 0x20, 0x75, 0x33, 0x32, 0x20, 0x63, 0x6f, 0x75, 0x6e, 0x74,
  0x29, 0x3b, 0x0a, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x49, 0x4d, 0x50, 0x4f,
  0x52, 0x54, 0x28, 0x22, 0x62, 0x6c, 0x69, 0x74, 0x22, 0x29, 0x20, 0x76,
  0x6f, 0x69, 0x64, 0x20, 0x62, 0x6c, 0x69, 0x74, 0x28, 0x69, 0x33, 0x32,
  0x20, 0x78, 0x2c, 0x20, 0x69, 0x33, 0x32, 0x20, 0x79, 0x2c, 0x20, 0x69,
  0x33, 0x32, 0x20, 0x64, 0x78, 0x2c, 0x20, 0x69, 0x33, 0x32, 0x20, 0x64,
  0x79, 0x2c, 0x20, 0x75, 0x33, 0x32, 0x20, 0x68, 0x2c, 0x20, 0x75, 0x33,
  0x32, 0x20, 0x77, 0x29, 0x3b, 0x0a, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x43, 0x41, 0x52, 0x54,
  0x52, 0x49, 0x44, 0x47, 0x45, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x0a, 0x0a, 0x2f, 0x2f, 0x20, 0x4c, 0x6f, 0x61, 0x64,
  0x20, 0x72, 0x61, 0x77, 0x20, 0x64, 0x61, 0x74, 0x61, 0x20, 0x66, 0x72,
  0x6f, 0x6d, 0x20, 0x63, 0x61, 0x72, 0x74, 0x0a, 0x57, 0x41, 0x53, 0x4d,
  0x5f, 0x49, 0x4d, 0x50, 0x4f, 0x52, 0x54, 0x28, 0x22, 0x6c, 0x6f, 0x61,
  0x64, 0x22, 0x29, 0x20, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6c, 0x6f, 0x61,
  0x64, 0x28, 0x75, 0x33, 0x32, 0x20, 0x63, 0x68, 0x75, 0x6e, 0x6b, 0x29,
  0x3b, 0x0a, 0x2f, 0x2f, 0x20, 0x4c, 0x6f, 0x61, 0x64, 0x20, 0x72, 0x61,
  0x77, 0x20, 0x64, 0x61, 0x74, 0x61, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20,
  0x63, 0x61, 0x72, 0x74, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x6f, 0x76,
  0x65, 0x72, 0x72, 0x69, 0x64, 0x65, 0x64, 0x20, 0x64, 0x65, 0x73, 0x74,
  0x69, 0x6e, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x0a, 0x2f, 0x2f, 0x20, 0x40,
  0x63, 0x68, 0x75, 0x6e, 0x6b, 0x20, 0x63, 0x68, 0x75, 0x6e, 0x6b, 0x20,
  0x69, 0x6e, 0x64, 0x65, 0x78, 0x0a, 0x2f, 0x2f, 0x20, 0x40, 0x64, 0x73,
  0x74, 0x20, 0x20, 0x20, 0x64, 0x65, 0x73, 0x74, 0x69, 0x6e, 0x61, 0x74,
  0x69, 0x6f, 0x6e, 0x20, 0x28, 0x73, 0x6c, 0x6f, 0x74, 0x20, 0x66, 0x6f,
  0x72, 0x20, 0x67, 0x70, 0x75, 0x2c, 0x20, 0x61, 0x64, 0x64, 0x72, 0x20,
  0x66, 0x6f, 0x72, 0x20, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x2e, 0x2e,
  0x2e, 0x29, 0x0a, 0x57, 0x41, 0x53, 0x4d, 0x5f, 0x49, 0x4d, 0x50, 0x4f,
  0x52, 0x54, 0x28, 0x22, 0x6c, 0x6f, 0x61, 0x64, 0x64, 0x22, 0x29, 0x20,
  0x76, 0x6f, 0x69, 0x64, 0x20, 0x6c, 0x6f, 0x61, 0x64, 0x64, 0x28, 0x75,
  0x33, 0x32, 0x20, 0x63, 0x68, 0x75, 0x6e, 0x6b, 0x2c, 0x20, 0x75, 0x33,
  0x32, 0x20, 0x64, 0x73, 0x74, 0x29, 0x3b, 0x0a, 0x0a, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x43, 0x4f,
  0x4e, 0x54, 0x52, 0x4f, 0x4c, 0x4c, 0x45, 0x52, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x0a, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
  0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x0a, 0x0a, 0x23, 0x65, 0x6e, 0x64, 0x69,
  0x66, 0x0a};
static sdk_template_file_t template_c_files[] = {
{ .path=".gitignore", .data=template_c__gitignore_data, .size=sizeof(template_c__gitignore_data) },
{ .path="Makefile", .data=template_c_Makefile_data, .size=sizeof(template_c_Makefile_data) },
{ .path="nux.json", .data=template_c_nux_json_data, .size=sizeof(template_c_nux_json_data) },
{ .path="src/main.c", .data=template_c_main_c_data, .size=sizeof(template_c_main_c_data) },
{ .path="src/nux.h", .data=template_c_nux_h_data, .size=sizeof(template_c_nux_h_data) },
{ .path=NU_NULL, .data=NU_NULL, .size=0 }
};
static const nu_byte_t template_rust_Cargo_toml_data[] = {
  0x5b, 0x70, 0x61, 0x63, 0x6b, 0x61, 0x67, 0x65, 0x5d, 0x0a, 0x6e, 0x61,
  0x6d, 0x65, 0x20, 0x3d, 0x20, 0x22, 0x63, 0x61, 0x72, 0x74, 0x22, 0x0a,
  0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x22, 0x30,
  0x2e, 0x31, 0x2e, 0x30, 0x22, 0x0a, 0x65, 0x64, 0x69, 0x74, 0x69, 0x6f,
  0x6e, 0x20, 0x3d, 0x20, 0x22, 0x32, 0x30, 0x32, 0x31, 0x22, 0x0a, 0x0a,
  0x5b, 0x6c, 0x69, 0x62, 0x5d, 0x0a, 0x63, 0x72, 0x61, 0x74, 0x65, 0x2d,
  0x74, 0x79, 0x70, 0x65, 0x20, 0x3d, 0x20, 0x5b, 0x22, 0x63, 0x64, 0x79,
  0x6c, 0x69, 0x62, 0x22, 0x5d, 0x0a, 0x0a, 0x5b, 0x64, 0x65, 0x70, 0x65,
  0x6e, 0x64, 0x65, 0x6e, 0x63, 0x69, 0x65, 0x73, 0x5d, 0x0a, 0x0a, 0x5b,
  0x70, 0x72, 0x6f, 0x66, 0x69, 0x6c, 0x65, 0x2e, 0x72, 0x65, 0x6c, 0x65,
  0x61, 0x73, 0x65, 0x5d, 0x0a, 0x6f, 0x70, 0x74, 0x2d, 0x6c, 0x65, 0x76,
  0x65, 0x6c, 0x20, 0x3d, 0x20, 0x22, 0x7a, 0x22, 0x0a, 0x6c, 0x74, 0x6f,
  0x20, 0x3d, 0x20, 0x74, 0x72, 0x75, 0x65, 0x0a};
static const nu_byte_t template_rust__gitignore_data[] = {
  0x64, 0x65, 0x62, 0x75, 0x67, 0x2f, 0x0a, 0x74, 0x61, 0x72, 0x67, 0x65,
  0x74, 0x2f, 0x0a, 0x2a, 0x2a, 0x2f, 0x2a, 0x2e, 0x72, 0x73, 0x2e, 0x62,
  0x6b, 0x0a, 0x2a, 0x2e, 0x70, 0x64, 0x62, 0x0a, 0x43, 0x61, 0x72, 0x67,
  0x6f, 0x2e, 0x6c, 0x6f, 0x63, 0x6b, 0x0a};
static const nu_byte_t template_rust_nux_json_data[] = {
  0x7b, 0x0a, 0x09, 0x22, 0x74, 0x61, 0x72, 0x67, 0x65, 0x74, 0x22, 0x3a,
  0x20, 0x22, 0x63, 0x61, 0x72, 0x74, 0x2e, 0x62, 0x69, 0x6e, 0x22, 0x2c,
  0x0a, 0x09, 0x22, 0x70, 0x72, 0x65, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x22,
  0x3a, 0x20, 0x22, 0x63, 0x61, 0x72, 0x67, 0x6f, 0x20, 0x62, 0x75, 0x69,
  0x6c, 0x64, 0x20, 0x2d, 0x2d, 0x72, 0x65, 0x6c, 0x65, 0x61, 0x73, 0x65,
  0x22, 0x2c, 0x0a, 0x09, 0x22, 0x63, 0x68, 0x75, 0x6e, 0x6b, 0x73, 0x22,
  0x3a, 0x20, 0x5b, 0x0a, 0x09, 0x09, 0x7b, 0x0a, 0x09, 0x09, 0x09, 0x22,
  0x74, 0x79, 0x70, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x77, 0x61, 0x73, 0x6d,
  0x22, 0x2c, 0x0a, 0x09, 0x09, 0x09, 0x22, 0x73, 0x6f, 0x75, 0x72, 0x63,
  0x65, 0x22, 0x3a, 0x20, 0x22, 0x74, 0x61, 0x72, 0x67, 0x65, 0x74, 0x2f,
  0x77, 0x61, 0x73, 0x6d, 0x33, 0x32, 0x2d, 0x75, 0x6e, 0x6b, 0x6e, 0x6f,
  0x77, 0x6e, 0x2d, 0x75, 0x6e, 0x6b, 0x6e, 0x6f, 0x77, 0x6e, 0x2f, 0x72,
  0x65, 0x6c, 0x65, 0x61, 0x73, 0x65, 0x2f, 0x63, 0x61, 0x72, 0x74, 0x2e,
  0x77, 0x61, 0x73, 0x6d, 0x22, 0x0a, 0x09, 0x09, 0x7d, 0x0a, 0x09, 0x5d,
  0x0a, 0x7d, 0x0a};
static const nu_byte_t template_rust_config_toml_data[] = {
  0x5b, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x5d, 0x0a, 0x74, 0x61, 0x72, 0x67,
  0x65, 0x74, 0x20, 0x3d, 0x20, 0x22, 0x77, 0x61, 0x73, 0x6d, 0x33, 0x32,
  0x2d, 0x75, 0x6e, 0x6b, 0x6e, 0x6f, 0x77, 0x6e, 0x2d, 0x75, 0x6e, 0x6b,
  0x6e, 0x6f, 0x77, 0x6e, 0x22, 0x0a, 0x0a, 0x5b, 0x74, 0x61, 0x72, 0x67,
  0x65, 0x74, 0x2e, 0x77, 0x61, 0x73, 0x6d, 0x33, 0x32, 0x2d, 0x75, 0x6e,
  0x6b, 0x6e, 0x6f, 0x77, 0x6e, 0x2d, 0x75, 0x6e, 0x6b, 0x6e, 0x6f, 0x77,
  0x6e, 0x5d, 0x0a, 0x72, 0x75, 0x73, 0x74, 0x66, 0x6c, 0x61, 0x67, 0x73,
  0x20, 0x3d, 0x20, 0x5b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x23, 0x20, 0x22,
  0x2d, 0x43, 0x22, 0x2c, 0x20, 0x22, 0x6c, 0x69, 0x6e, 0x6b, 0x2d, 0x61,
  0x72, 0x67, 0x3d, 0x2d, 0x2d, 0x69, 0x6d, 0x70, 0x6f, 0x72, 0x74, 0x2d,
  0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x23, 0x20, 0x22, 0x2d, 0x43, 0x22, 0x2c, 0x20, 0x22, 0x6c, 0x69,
  0x6e, 0x6b, 0x2d, 0x61, 0x72, 0x67, 0x3d, 0x2d, 0x2d, 0x69, 0x6e, 0x69,
  0x74, 0x69, 0x61, 0x6c, 0x2d, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x3d,
  0x36, 0x35, 0x35, 0x33, 0x36, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x23, 0x20, 0x22, 0x2d, 0x43, 0x22, 0x2c, 0x20, 0x22, 0x6c, 0x69, 0x6e,
  0x6b, 0x2d, 0x61, 0x72, 0x67, 0x3d, 0x2d, 0x2d, 0x6d, 0x61, 0x78, 0x2d,
  0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x3d, 0x36, 0x35, 0x35, 0x33, 0x36,
  0x22, 0x2c, 0x0a, 0x5d, 0x0a};
static const nu_byte_t template_rust_lib_rs_data[] = {
  0x23, 0x5b, 0x6e, 0x6f, 0x5f, 0x6d, 0x61, 0x6e, 0x67, 0x6c, 0x65, 0x5d,
  0x0a, 0x66, 0x6e, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74, 0x28, 0x29, 0x20,
  0x7b, 0x7d, 0x0a, 0x0a, 0x23, 0x5b, 0x6e, 0x6f, 0x5f, 0x6d, 0x61, 0x6e,
  0x67, 0x6c, 0x65, 0x5d, 0x0a, 0x66, 0x6e, 0x20, 0x75, 0x70, 0x64, 0x61,
  0x74, 0x65, 0x28, 0x29, 0x20, 0x7b, 0x7d, 0x0a};
static sdk_template_file_t template_rust_files[] = {
{ .path="Cargo.toml", .data=template_rust_Cargo_toml_data, .size=sizeof(template_rust_Cargo_toml_data) },
{ .path=".gitignore", .data=template_rust__gitignore_data, .size=sizeof(template_rust__gitignore_data) },
{ .path="nux.json", .data=template_rust_nux_json_data, .size=sizeof(template_rust_nux_json_data) },
{ .path=".cargo/config.toml", .data=template_rust_config_toml_data, .size=sizeof(template_rust_config_toml_data) },
{ .path="src/lib.rs", .data=template_rust_lib_rs_data, .size=sizeof(template_rust_lib_rs_data) },
{ .path=NU_NULL, .data=NU_NULL, .size=0 }
};
#endif
