
//#include <libavutil/common.h>
#include <assert.h>
// test
typedef enum
{
    AOUT_CHANIDX_DISABLE = -1,
    AOUT_CHANIDX_LEFT,
    AOUT_CHANIDX_RIGHT,
    AOUT_CHANIDX_MIDDLELEFT,
    AOUT_CHANIDX_MIDDLERIGHT,
    AOUT_CHANIDX_REARLEFT,
    AOUT_CHANIDX_REARRIGHT,
    AOUT_CHANIDX_REARCENTER,
    AOUT_CHANIDX_CENTER,
    AOUT_CHANIDX_LFE,
    AOUT_CHANIDX_MAX
} vlc_chan_order_idx_t;
#define INPUT_CLOCK_LATE_COUNT (3)

#define AOUT_CHAN_MAX 9
// static inline uint8_t clip_uint8_vlc(int32_t a)
// {
//     if (a & (~255))
//         return (-a) >> 31;
//     else
//         return a;
// }

static_assert(AOUT_CHANIDX_MAX == AOUT_CHAN_MAX, "channel count mismatch");

int main(void)
{

    static_assert(INPUT_CLOCK_LATE_COUNT == 3,
                  "unsupported INPUT_CLOCK_LATE_COUNT");

    return 0;
}