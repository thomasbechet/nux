#include "internal.h"

#define JSMN_STRICT
#define JSMN_PARENT_LINKS
#include <externals/jsmn/jsmn.h>

static void
json_append (nux_serde_json_writer_t *j, const nux_c8_t *s, nux_u32_t n)
{
    nux_ctx_t *ctx = j->ctx;
    nux_io_write(ctx, &j->file, s, n);
}
static nux_status_t
json_writer (const nux_serde_writer_t *s)
{
    nux_serde_json_writer_t *j = s->userdata;
    nux_c8_t                 buf[256];
    nux_u32_t                n;
    if (j->has_previous_value && s->type != NUX_SERDE_END)
    {
        json_append(j, ",", 1);
    }
    if (s->key && j->stack[s->depth] == NUX_SERDE_OBJECT)
    {
        n = nux_snprintf(buf, sizeof(buf), "\"%s\":", s->key);
        json_append(j, buf, n);
    }
    switch (s->type)
    {
        case NUX_SERDE_OBJECT:
            json_append(j, "{", 1);
            j->has_previous_value  = NUX_FALSE;
            j->stack[s->depth + 1] = s->type;
            break;
        case NUX_SERDE_ARRAY:
            json_append(j, "[", 1);
            j->has_previous_value  = NUX_FALSE;
            j->stack[s->depth + 1] = s->type;
            break;
        case NUX_SERDE_END:
            if (j->stack[s->depth + 1] == NUX_SERDE_OBJECT)
            {
                json_append(j, "}", 1);
            }
            else if (j->stack[s->depth + 1] == NUX_SERDE_ARRAY)
            {
                json_append(j, "]", 1);
            }
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_U32:
            n = nux_snprintf(buf, sizeof(buf), "%d", *s->value.u32);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_STR:
            n = nux_snprintf(buf, sizeof(buf), "\"%s\"", *s->value.str.s);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_V3:
            n = nux_snprintf(buf,
                             sizeof(buf),
                             "[%lf,%lf,%lf]",
                             s->value.v3->x,
                             s->value.v3->y,
                             s->value.v3->z);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
    }
    return NUX_SUCCESS;
}
static nux_b32_t
json_equals (const nux_c8_t *json, jsmntok_t *tok, const nux_c8_t *s)
{
    if (tok->type == JSMN_STRING
        && (nux_i32_t)nux_strnlen(s, 256) == tok->end - tok->start
        && nux_strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return NUX_TRUE;
    }
    return NUX_FALSE;
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
json_find (const nux_serde_json_reader_t *j,
           const jsmntok_t               *obj,
           jsmntype_t                     type,
           const nux_c8_t                *key)
{
    jsmntok_t *toks = j->tokens;
    nux_i32_t  it   = j->it + 1;
    for (nux_i32_t i = 0; i < obj->size; ++i)
    {
        if (json_equals(j->json, &toks[it], key))
        {
            if (toks[it + 1].type != type)
            {
                return NUX_NULL;
            }
            return &toks[it + 1];
        }
        // skip value
        ++it;
        json_nested_skip(toks, j->tokens_count, &it);
    }
    return NUX_NULL;
}
static nux_status_t
json_reader (const nux_serde_reader_t *s)
{
    nux_serde_json_reader_t *j    = s->userdata;
    jsmntok_t               *toks = j->tokens;
    jsmntok_t               *tok  = toks + j->it;

    if (s->key && tok->type != JSMN_OBJECT)
    {
        return NUX_FAILURE;
    }

    switch (s->type)
    {
        case NUX_SERDE_OBJECT:
            tok = json_find(j, tok, JSMN_OBJECT, s->key);
            NUX_CHECK(tok, return NUX_FAILURE);
            j->it = tok - (jsmntok_t *)j->tokens;
            break;
        case NUX_SERDE_ARRAY:
            tok = json_find(j, tok, JSMN_ARRAY, s->key);
            NUX_CHECK(tok, return NUX_FAILURE);
            j->it = tok - (jsmntok_t *)j->tokens;
            break;
        case NUX_SERDE_END:
            j->it = toks[j->it].parent;
            break;
        case NUX_SERDE_U32:
            tok = json_find(j, tok, JSMN_PRIMITIVE, s->key);
            NUX_CHECK(tok, return NUX_FAILURE);
            *s->value.u32 = nux_strtof(j->json + tok->start, NUX_NULL);
            break;
        case NUX_SERDE_STR:
            tok = json_find(j, tok, JSMN_STRING, s->key);
            NUX_CHECK(tok, return NUX_FAILURE);
            *s->value.str.s = j->json + tok->start;
            *s->value.str.n = tok->end - tok->start;
            break;
        case NUX_SERDE_V3:
            tok = json_find(j, tok, JSMN_ARRAY, s->key);
            NUX_CHECK(tok && tok->size == NUX_V3_SIZE, return NUX_FAILURE);
            for (nux_u32_t i = 0; i < NUX_V3_SIZE; ++i)
            {
                jsmntok_t *p = tok + i + 1;
                NUX_CHECK(p->type == JSMN_PRIMITIVE, return NUX_FAILURE);
                s->value.v3->data[i] = nux_strtof(j->json + p->start, NUX_NULL);
            }
            break;
    }
    return NUX_SUCCESS;
}

nux_status_t
nux_serde_json_writer_init (nux_serde_json_writer_t *j,
                            nux_ctx_t               *ctx,
                            const nux_c8_t          *path)
{
    j->ctx                = ctx;
    j->has_previous_value = NUX_FALSE;
    nux_serde_writer_init(&j->writer, json_writer, j);
    NUX_CHECK(nux_io_open(ctx, path, NUX_IO_READ_WRITE, &j->file),
              return NUX_FAILURE);
    j->stack[0] = NUX_SERDE_OBJECT;
    nux_serde_write_object(&j->writer, NUX_NULL);
    return NUX_SUCCESS;
}
void
nux_serde_json_writer_close (nux_serde_json_writer_t *j)
{
    nux_serde_write_end(&j->writer);
    nux_io_close(j->ctx, &j->file);
}
nux_status_t
nux_serde_json_reader_init (nux_serde_json_reader_t *j,
                            nux_ctx_t               *ctx,
                            const nux_c8_t          *path)
{
    nux_serde_reader_init(&j->reader, json_reader, j);
    nux_arena_t *a
        = nux_resource_get(ctx, NUX_RESOURCE_ARENA, ctx->frame_arena_rid);
    NUX_ASSERT(a);

    j->json = nux_io_load(ctx, ctx->frame_arena_rid, path, &j->json_size);
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
        j->tokens = nux_arena_alloc(a,
                                    j->tokens,
                                    sizeof(jsmntok_t) * old_capa,
                                    sizeof(jsmntok_t) * j->tokens_capa);
        NUX_CHECK(j->tokens, return NUX_FAILURE);
    }
    if (r < 0)
    {
        return NUX_FAILURE;
    }
    j->tokens_count = r;
    j->it           = 0;

    return NUX_SUCCESS;
}
