#include <stdio.h>

#define VLC_USED  __attribute__((warn_unused_result))

#ifndef VLC_CHARSET_H
#define VLC_CHARSET_H 1


int vlc_sscanf_c(const char*, const char*, ...) VLC_USED
#ifdef __GNUC__
__attribute__((format(scanf, 2, 3)))
#endif
;


/** @} */
/** @} */

#endif

int main(void) {
    return 0;
}