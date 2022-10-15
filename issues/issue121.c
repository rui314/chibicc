
//#include <libavutil/common.h>
#include <assert.h>
#include <stddef.h>
#include <stdatomic.h>
#include <stdalign.h>

// test

struct vcddev_s
{
    char *psz_dev; /* vcd device name */

    /* Section used in vcd image mode */
    int i_vcdimage_handle;
};

typedef struct vcddev_s vcddev_t;

typedef struct
{
    vcddev_t *vcddev;  /* vcd device descriptor */
    unsigned start;    /**< Track first sector */
    unsigned length;   /**< Track total sectors */
    unsigned position; /**< Current offset within track sectors */
} demux_sys_t;

static_assert(offsetof(demux_sys_t, vcddev) == 0, "Invalid cast");

static const size_t alignv[] = {
    alignof(char),
    alignof(short),
    alignof(int),
    alignof(long),
    alignof(long long),
    alignof(float),
    alignof(double),
    alignof(struct big_align_struct),
    alignof(void *),
    alignof(max_align_t),
};

static_assert(sizeof(atomic_uint) <= sizeof(struct vlc_suuint),
              "Size mismatch");

static_assert(alignof(atomic_uint) <= alignof(struct vlc_suuint),
              "Alignment mismatch");

int main(void)
{

    return 0;
}