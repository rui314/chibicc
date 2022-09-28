

struct vlc_param {
    union {
        _Atomic int64_t i; /**< Current value (if integer or boolean) */
        _Atomic float f; /**< Current value (if floating point) */
        char *_Atomic str; /**< Current value (if character string) */
    } value;

    unsigned char shortname; /**< Optional short option name */
    unsigned internal:1; /**< Hidden from preferences and help */
    unsigned unsaved:1; /**< Not stored in persistent configuration */
    unsigned safe:1; /**< Safe for untrusted provisioning (playlists) */
    unsigned obsolete:1; /**< Ignored for backward compatibility */

};

int main(void) {
    return 0;
}