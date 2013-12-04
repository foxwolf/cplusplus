## w32ce-add.h - Snippet to be be included into gpg-error.h.
## (Comments are indicated by a double hash mark)

/* Substitute for strerror - this one is thread safe.  */
char *_gpg_w32ce_strerror (int err);
#define strerror(a) _gpg_w32ce_strerror (a)

