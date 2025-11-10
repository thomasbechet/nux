#include "internal.h"

#define JSMN_STRICT
#define JSMN_PARENT_LINKS
#include <externals/jsmn/jsmn.h>

static void
json_append (nux_json_writer_t *j, const nux_c8_t *s, nux_u32_t n)
{
    nux_write_file(j->file, s, n);
}
static nux_b32_t
json_equals (const nux_c8_t *json, jsmntok_t *tok, const nux_c8_t *s)
{
    return tok->type == JSMN_STRING
           && ((nux_i32_t)nux_strnlen(s, 256) == tok->end - tok->start)
           && (nux_strncmp(json + tok->start, s, tok->end - tok->start) == 0);
}
static void
json_nested_skip (const jsmntok_t *toks, nux_i32_t num_tokens, nux_i32_t *i)
{
    for (nux_i32_t char_end = toks[*i].end;
         *i < num_tokens && toks[*i].start < char_end;
         (*i)++)
        ;
}
static jsmntok_t *
json_next (const nux_json_reader_t *j, const jsmntok_t *current)
{
    jsmntok_t *toks   = j->tokens;
    nux_i32_t  it     = current - toks;
    nux_i32_t  parent = toks[it].parent;
    json_nested_skip(j->tokens, j->tokens_count, &it);
    if (toks[it].parent == parent)
    {
        return toks + it;
    }
    return NUX_NULL;
}
static jsmntok_t *
json_find (const nux_json_reader_t *j,
           const jsmntok_t         *obj,
           const nux_c8_t          *key)
{
    jsmntok_t *toks = j->tokens;
    NUX_ASSERT(key);
    nux_i32_t it = (obj - toks) + 1; // first key
    for (nux_i32_t i = 0; i < obj->size; ++i)
    {
        if (json_equals(j->json, &toks[it], key))
        {
            return &toks[it + 1]; // return value
        }
        ++it;                                         // skip key
        json_nested_skip(toks, j->tokens_count, &it); // skip value
    }
    return NUX_NULL;
}
static nux_status_t
json_writer (void *userdata, const nux_serde_value_t *v)
{
    nux_json_writer_t *j = userdata;
    nux_c8_t           buf[256];
    nux_u32_t          n;
    if (j->has_previous_value && v->type != NUX_SERDE_END)
    {
        json_append(j, ",", 1);
    }
    if (j->depth != 0) // skip first line
    {
        json_append(j, "\n", 1);
        for (nux_u32_t i = 0; i < j->depth; ++i)
        {
            json_append(j, "    ", 4);
        }
    }
    if (v->key && j->stack[j->depth] == NUX_SERDE_OBJECT)
    {
        n = nux_snprintf(buf, sizeof(buf), "\"%s\": ", v->key);
        json_append(j, buf, n);
    }
    switch (v->type)
    {
        case NUX_SERDE_OBJECT:
            json_append(j, "{", 1);
            j->has_previous_value  = NUX_FALSE;
            j->stack[j->depth + 1] = v->type;
            ++j->depth;
            break;
        case NUX_SERDE_ARRAY:
            json_append(j, "[", 1);
            j->has_previous_value  = NUX_FALSE;
            j->stack[j->depth + 1] = v->type;
            ++j->depth;
            break;
        case NUX_SERDE_END:
            --j->depth;
            if (j->stack[j->depth + 1] == NUX_SERDE_OBJECT)
            {
                json_append(j, "}", 1);
            }
            else if (j->stack[j->depth + 1] == NUX_SERDE_ARRAY)
            {
                json_append(j, "]", 1);
            }
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_RID:
        case NUX_SERDE_NID:
        case NUX_SERDE_U32:
            n = nux_snprintf(buf, sizeof(buf), "%d", *v->u32);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_F32:
            n = nux_snprintf(buf, sizeof(buf), "%lf", *v->f32);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_STRING:
            n = nux_snprintf(buf, sizeof(buf), "\"%s\"", *v->str.s);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_V3:
            n = nux_snprintf(buf,
                             sizeof(buf),
                             "[%lf, %lf, %lf]",
                             v->v3->x,
                             v->v3->y,
                             v->v3->z);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_Q4:
            n = nux_snprintf(buf,
                             sizeof(buf),
                             "[%lf, %lf, %lf, %lf]",
                             v->q4->x,
                             v->q4->y,
                             v->q4->z,
                             v->q4->w);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_BYTES:
            break;
    }
    return NUX_SUCCESS;
}
static nux_status_t
json_reader (void *userdata, nux_serde_value_t *v)
{
    nux_json_reader_t *j    = userdata;
    jsmntok_t         *toks = j->tokens;

    if (v->type == NUX_SERDE_END)
    {
        --j->depth;
        j->it = toks + j->it->parent;
        return NUX_SUCCESS;
    }

    jsmntok_t *tok = NUX_NULL;
    if (v->key) // find field in object
    {
        NUX_ASSERT(j->it->type == JSMN_OBJECT);
        tok = json_find(j, j->it, v->key);
    }
    else if (j->iters[j->depth]) // in iterator
    {
        tok                = j->iters[j->depth];
        j->iters[j->depth] = json_next(j, tok);
    }
    NUX_ASSERT(tok);
    NUX_CHECK(tok, return NUX_FAILURE);

    switch (v->type)
    {
        case NUX_SERDE_OBJECT:
            NUX_CHECK(tok->type == JSMN_OBJECT, return NUX_FAILURE);
            ++j->depth;
            j->it = tok;
            break;
        case NUX_SERDE_ARRAY:
            NUX_CHECK(tok->type == JSMN_ARRAY, return NUX_FAILURE);
            *v->size = tok->size;
            j->it    = tok;
            ++j->depth;
            if (tok->size)
            {
                j->iters[j->depth] = tok + 1; // first item
            }
            else
            {
                j->iters[j->depth] = NUX_NULL;
            }
            break;
        case NUX_SERDE_END:
            --j->depth;
            j->it = toks + j->it->parent;
            break;
        case NUX_SERDE_RID:
        case NUX_SERDE_NID:
        case NUX_SERDE_U32:
            NUX_CHECK(tok->type == JSMN_PRIMITIVE, return NUX_FAILURE);
            *v->u32 = nux_strtof(j->json + tok->start, NUX_NULL);
            break;
        case NUX_SERDE_F32:
            NUX_CHECK(tok->type == JSMN_PRIMITIVE, return NUX_FAILURE);
            *v->f32 = nux_strtof(j->json + tok->start, NUX_NULL);
            break;
        case NUX_SERDE_STRING:
            NUX_CHECK(tok->type == JSMN_STRING, return NUX_FAILURE);
            *v->str.s = j->json + tok->start;
            *v->str.n = tok->end - tok->start;
            break;
        case NUX_SERDE_V3:
            NUX_CHECK(tok->type == JSMN_ARRAY && tok->size == NUX_V3_SIZE,
                      return NUX_FAILURE);
            for (nux_u32_t i = 0; i < NUX_V3_SIZE; ++i)
            {
                jsmntok_t *p = tok + i + 1;
                NUX_CHECK(p->type == JSMN_PRIMITIVE, return NUX_FAILURE);
                v->v3->data[i] = nux_strtof(j->json + p->start, NUX_NULL);
            }
            break;
        case NUX_SERDE_Q4:
            NUX_CHECK(tok->type == JSMN_ARRAY && tok->size == NUX_Q4_SIZE,
                      return NUX_FAILURE);
            for (nux_u32_t i = 0; i < NUX_Q4_SIZE; ++i)
            {
                jsmntok_t *p = tok + i + 1;
                NUX_CHECK(p->type == JSMN_PRIMITIVE, return NUX_FAILURE);
                v->q4->data[i] = nux_strtof(j->json + p->start, NUX_NULL);
            }
            break;
        case NUX_SERDE_BYTES:
            break;
    }

    return NUX_SUCCESS;
}

nux_status_t
nux_json_writer_init (nux_json_writer_t *j, const nux_c8_t *path)
{
    j->depth              = 0;
    j->has_previous_value = NUX_FALSE;
    nux_serde_writer_init(&j->writer, j, json_writer);
    j->file = nux_open_file(nux_frame_arena(), path, NUX_IO_READ_WRITE);
    NUX_CHECK(j->file, return NUX_FAILURE);
    j->stack[0] = NUX_SERDE_OBJECT;
    nux_serde_write_object(&j->writer, NUX_NULL);
    return NUX_SUCCESS;
}
void
nux_json_writer_close (nux_json_writer_t *j)
{
    json_append(j, "\n", 1);
    nux_serde_write_end(&j->writer);
    nux_close_file(j->file);
}
nux_status_t
nux_json_reader_init (nux_json_reader_t *j, const nux_c8_t *path)
{
    nux_serde_reader_init(&j->reader, j, json_reader);

    nux_arena_t *a = nux_frame_arena();

    j->json = nux_load_file(a, path, &j->json_size);
    NUX_CHECK(j->json, return NUX_FAILURE);

    jsmn_parser parser;
    jsmn_init(&parser);

    j->tokens_capa = 256;
    j->tokens      = nux_arena_malloc(a, sizeof(jsmntok_t) * j->tokens_capa);
    NUX_CHECK(j->tokens, return NUX_FAILURE);

    nux_i32_t r
        = jsmn_parse(&parser, j->json, j->json_size, j->tokens, j->tokens_capa);
    while (r == JSMN_ERROR_NOMEM)
    {
        nux_u32_t old_capa = j->tokens_capa;
        j->tokens_capa *= 2;
        j->tokens = nux_arena_realloc(a,
                                      j->tokens,
                                      sizeof(jsmntok_t) * old_capa,
                                      sizeof(jsmntok_t) * j->tokens_capa);
        NUX_CHECK(j->tokens, return NUX_FAILURE);
        r = jsmn_parse(
            &parser, j->json, j->json_size, j->tokens, j->tokens_capa);
    }
    if (r < 0)
    {
        return NUX_FAILURE;
    }
    j->tokens_count = r;
    j->it           = j->tokens;
    j->depth        = 0;
    nux_memset(j->iters, 0, sizeof(j->iters));

    NUX_ASSERT(j->it->type == JSMN_OBJECT);

    return NUX_SUCCESS;
}
