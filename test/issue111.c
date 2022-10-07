
#include <stdarg.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <stdint.h>

enum input_control_e
{
    INPUT_CONTROL_SET_STATE,

    INPUT_CONTROL_SET_RATE,

    INPUT_CONTROL_SET_POSITION,
    INPUT_CONTROL_JUMP_POSITION,

    INPUT_CONTROL_SET_TIME,
    INPUT_CONTROL_JUMP_TIME,

    INPUT_CONTROL_SET_PROGRAM,

    INPUT_CONTROL_SET_TITLE,
    INPUT_CONTROL_SET_TITLE_NEXT,
    INPUT_CONTROL_SET_TITLE_PREV,

    INPUT_CONTROL_SET_SEEKPOINT,
    INPUT_CONTROL_SET_SEEKPOINT_NEXT,
    INPUT_CONTROL_SET_SEEKPOINT_PREV,

    INPUT_CONTROL_SET_BOOKMARK,

    INPUT_CONTROL_NAV_ACTIVATE, // NOTE: INPUT_CONTROL_NAV_* values must be
    INPUT_CONTROL_NAV_UP,       // contiguous and in the same order as
    INPUT_CONTROL_NAV_DOWN,     // INPUT_NAV_* and DEMUX_NAV_*.
    INPUT_CONTROL_NAV_LEFT,
    INPUT_CONTROL_NAV_RIGHT,
    INPUT_CONTROL_NAV_POPUP,
    INPUT_CONTROL_NAV_MENU,

    INPUT_CONTROL_SET_ES,
    INPUT_CONTROL_SET_ES_LIST,  // select a list of ES atomically
    INPUT_CONTROL_UNSET_ES,
    INPUT_CONTROL_RESTART_ES,
    INPUT_CONTROL_SET_ES_CAT_IDS,

    INPUT_CONTROL_SET_VIEWPOINT,    // new absolute viewpoint
    INPUT_CONTROL_SET_INITIAL_VIEWPOINT, // set initial viewpoint (generally from video)
    INPUT_CONTROL_UPDATE_VIEWPOINT, // update viewpoint relative to current

    INPUT_CONTROL_SET_CATEGORY_DELAY,
    INPUT_CONTROL_SET_ES_DELAY,

    INPUT_CONTROL_ADD_SLAVE,
    INPUT_CONTROL_SET_SUBS_FPS,

    INPUT_CONTROL_SET_RECORD_STATE,

    INPUT_CONTROL_SET_FRAME_NEXT,

    INPUT_CONTROL_SET_RENDERER,

    INPUT_CONTROL_SET_VBI_PAGE,
    INPUT_CONTROL_SET_VBI_TRANSPARENCY,
};
typedef struct input_item_node_t     input_item_node_t;
struct input_item_node_t
{
    char *         p_item;
    int                    i_children;
    input_item_node_t      **pp_children;
};


struct vlc_object_t
{
    struct vlc_logger *logger;
    union {
        struct vlc_object_internals *priv;
        struct vlc_object_marker *obj;
    };

    bool no_interact;

    /** Module probe flag
     *
     * A boolean during module probing when the probe is "forced".
     * See \ref module_need().
     */
    bool force;
};



typedef struct stream_t     stream_t;
struct stream_t
{
    struct vlc_object_t obj;

    char        *psz_name;
    char        *psz_url; /**< Full URL or MRL (can be NULL) */
    const char  *psz_location; /**< Location (URL with the scheme stripped) */
    char        *psz_filepath; /**< Local file path (if applicable) */
    bool         b_preparsing; /**< True if this access is used to preparse */
    char *p_input_item;/**< Input item (can be NULL) */

    stream_t *s;

    char    *out;   /* our p_es_out */
    ssize_t     (*pf_read)(stream_t *, void *buf, size_t len);
    char    *(*pf_block)(stream_t *, bool *eof);

    int         (*pf_readdir)(stream_t *, input_item_node_t *);

    int         (*pf_demux)(stream_t *);
    int         (*pf_seek)(stream_t *, uint64_t);
    int         (*pf_control)(stream_t *, int i_query, va_list);
    void *p_sys;
};


typedef union
{
    char * val;
    char * viewpoint;
    char *id;
    struct {
        int cat;
        char **ids;
    } list;
    struct {
        bool b_fast_seek;
        int i_val;
    } time;
    struct {
        bool b_fast_seek;
        double f_val;
    } pos;
    struct
    {
        int cat;
        int delay;
    } cat_delay;
    struct
    {
        int cat;
        char *str_ids;
    } cat_ids;
    struct
    {
        char *id;
        int delay;
    } es_delay;
    struct {
        char *id;
        unsigned page;
    } vbi_page;
    struct {
        char *id;
        bool enabled;
    } vbi_transparency;
    struct {
        bool enabled;
        char *dir_path;
    } record_state;
} input_control_param_t;





int main(void) {
    char * p_input;
    input_control_param_t it = { .id = p_input, .time.i_val = 1,};
    return 0;
}

