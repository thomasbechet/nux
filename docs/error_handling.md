# Error handling

NUX_CHECK:
- Report line and file
- Can trace error
- No root error
NUX_CHECKM:
- Same as NUX_CHECK
- But indicates root cause

- Base data structures must not produce error messages (except for memory allocation *_alloc functions), they are library code
