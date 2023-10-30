// This is an implementation of the open-addressing hash table.

#include "chibicc.h"

// Initial hash bucket size
#define INIT_SIZE 16

// Rehash if the usage exceeds 70%.
#define HIGH_WATERMARK 70

// We'll keep the usage below 50% after rehashing.
#define LOW_WATERMARK 50

// Represents a deleted hash entry
#define TOMBSTONE ((void *)-1)

static uint64_t fnv_hash(char *s, int len) {
  uint64_t hash = 0xcbf29ce484222325;
  for (int i = 0; i < len; i++) {
    hash *= 0x100000001b3;
    hash ^= (unsigned char)s[i];
  }
  return hash;
}

// Make room for new entires in a given hashmap by removing
// tombstones and possibly extending the bucket size.
static void rehash(HashMap *map) {
  // Compute the size of the new hashmap.
  int nkeys = 0;
  for (int i = 0; i < map->capacity; i++)
    if (map->buckets[i].key && map->buckets[i].key != TOMBSTONE)
      nkeys++;

  int cap = map->capacity;
  while ((nkeys * 100) / cap >= LOW_WATERMARK)
    cap = cap * 2;
  assert(cap > 0);

  // Create a new hashmap and copy all key-values.
  HashMap map2 = {};
  map2.buckets = calloc(cap, sizeof(HashEntry));
  map2.capacity = cap;

  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[i];
    if (ent->key && ent->key != TOMBSTONE)
      hashmap_put2(&map2, ent->key, ent->keylen, ent->val);
  }

  assert(map2.used == nkeys);
  *map = map2;
}

static bool match(HashEntry *ent, char *key, int keylen) {
  return ent->key && ent->key != TOMBSTONE &&
         ent->keylen == keylen && memcmp(ent->key, key, keylen) == 0;
}

static HashEntry *get_entry(HashMap *map, char *key, int keylen) {
  if (!map->buckets)
    return NULL;

  uint64_t hash = fnv_hash(key, keylen);

  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[(hash + i) % map->capacity];
    if (match(ent, key, keylen))
      return ent;
    if (ent->key == NULL)
      return NULL;
  }
  unreachable();
}

static HashEntry *get_or_insert_entry(HashMap *map, char *key, int keylen) {
  if (!map->buckets) {
    map->buckets = calloc(INIT_SIZE, sizeof(HashEntry));
    map->capacity = INIT_SIZE;
  } else if ((map->used * 100) / map->capacity >= HIGH_WATERMARK) {
    rehash(map);
  }

  uint64_t hash = fnv_hash(key, keylen);

  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[(hash + i) % map->capacity];

    if (match(ent, key, keylen))
      return ent;

    if (ent->key == TOMBSTONE) {
      ent->key = key;
      ent->keylen = keylen;
      return ent;
    }

    if (ent->key == NULL) {
      ent->key = key;
      ent->keylen = keylen;
      map->used++;
      return ent;
    }
  }
  unreachable();
}

void *hashmap_get(HashMap *map, char *key) {
  return hashmap_get2(map, key, strlen(key));
}

void *hashmap_get2(HashMap *map, char *key, int keylen) {
  HashEntry *ent = get_entry(map, key, keylen);
  return ent ? ent->val : NULL;
}

void hashmap_put(HashMap *map, char *key, void *val) {
   hashmap_put2(map, key, strlen(key), val);
}

void hashmap_put2(HashMap *map, char *key, int keylen, void *val) {
  HashEntry *ent = get_or_insert_entry(map, key, keylen);
  ent->val = val;
}

void hashmap_delete(HashMap *map, char *key) {
  hashmap_delete2(map, key, strlen(key));
}

void hashmap_delete2(HashMap *map, char *key, int keylen) {
  HashEntry *ent = get_entry(map, key, keylen);
  if (ent)
    ent->key = TOMBSTONE;
}

void deletion_ordering_test(void) {
  HashMap* map = calloc(1, sizeof(HashMap));

  hashmap_put(map, "c_malloc", (void*)1);
  hashmap_put(map, "__inline", (void*)1);
  hashmap_put(map, "c_scope", (void*)1);
  hashmap_put(map, "__SIZEOF_SIZE_T__", (void*)1);
  hashmap_put(map, "NULL", (void*)1);
  hashmap_put(map, "__SIZEOF_LONG_DOUBLE__", (void*)1);
  hashmap_put(map, "c_default_drop", (void*)1);
  hashmap_put(map, "ccharptr_cmp", (void*)1);
  hashmap_put(map, "__int32", (void*)1);
  hashmap_put(map, "__SIZEOF_FLOAT__", (void*)1);
  hashmap_put(map, "__SIZEOF_INT__", (void*)1);
  hashmap_put(map, "c_new", (void*)1);
  hashmap_put(map, "c_ROTL", (void*)1);
  hashmap_put(map, "$vec", (void*)1);
  hashmap_put(map, "forward_cqueue", (void*)1);
  hashmap_put(map, "ccharptr_hash", (void*)1);
  hashmap_put(map, "c_memcpy", (void*)1);
  hashmap_put(map, "__signed__", (void*)1);
  hashmap_put(map, "_c_ARG_N", (void*)1);
  hashmap_put(map, "__USER_LABEL_PREFIX__", (void*)1);
  hashmap_put(map, "i_static", (void*)1);
  hashmap_put(map, "c_NUMARGS", (void*)1);
  hashmap_put(map, "STC_API", (void*)1);
  hashmap_put(map, "__STDC__", (void*)1);
  hashmap_put(map, "__ptr32", (void*)1);
  hashmap_put(map, "c_ZU", (void*)1);
  hashmap_put(map, "c_free", (void*)1);
  hashmap_put(map, "__alignof__", (void*)1);
  hashmap_put(map, "i_free", (void*)1);
  hashmap_put(map, "c_defer", (void*)1);
  hashmap_put(map, "c_foreach_4", (void*)1);
  hashmap_put(map, "i_calloc", (void*)1);
  hashmap_put(map, "c_drop", (void*)1);
  hashmap_put(map, "c_foreach_3", (void*)1);
  hashmap_put(map, "forward_carc", (void*)1);
  hashmap_put(map, "__pragma", (void*)1);
  hashmap_put(map, "forward_cmap", (void*)1);
  hashmap_put(map, "c_i2u_size", (void*)1);
  hashmap_put(map, "forward_cvec", (void*)1);
  hashmap_put(map, "_WIN32", (void*)1);
  hashmap_put(map, "c_JOIN", (void*)1);
  hashmap_put(map, "_M_AMD64", (void*)1);
  hashmap_put(map, "c_ZI", (void*)1);
  hashmap_put(map, "__SIZEOF_SHORT__", (void*)1);
  hashmap_put(map, "c_assert", (void*)1);
  hashmap_put(map, "_i_new", (void*)1);
  hashmap_put(map, "__STDC_NO_COMPLEX__", (void*)1);
  hashmap_put(map, "__LP64__", (void*)1);
  hashmap_put(map, "__STDC_VERSION__", (void*)1);
  hashmap_put(map, "c_JOIN0", (void*)1);
  hashmap_put(map, "c_safe_cast", (void*)1);
  hashmap_put(map, "c_strncmp", (void*)1);
  hashmap_put(map, "c_forrange", (void*)1);
  hashmap_put(map, "__typeof__", (void*)1);
  hashmap_put(map, "__SIZEOF_DOUBLE__", (void*)1);
  hashmap_put(map, "__x86_64__", (void*)1);
  hashmap_put(map, "__WINT_TYPE__", (void*)1);
  hashmap_put(map, "__int8", (void*)1);
  hashmap_put(map, "c_MACRO_OVERLOAD", (void*)1);
  hashmap_put(map, "c_default_hash", (void*)1);
  hashmap_put(map, "c_arraylen", (void*)1);
  hashmap_put(map, "c_rs", (void*)1);
  hashmap_put(map, "STC_DEF", (void*)1);
  hashmap_put(map, "c_scope_2", (void*)1);
  hashmap_put(map, "c_init", (void*)1);
  hashmap_put(map, "__unaligned", (void*)1);
  hashmap_put(map, "c_scope_3", (void*)1);
  hashmap_put(map, "forward_clist", (void*)1);
  hashmap_put(map, "stc_hash_mix", (void*)1);
  hashmap_put(map, "__stdcall", (void*)1);
  hashmap_put(map, "c_sizeof", (void*)1);
  hashmap_put(map, "c_container_of", (void*)1);
  hashmap_put(map, "_CRT_DECLARE_NONSTDC_NAMES", (void*)1);
  hashmap_put(map, "c_static_assert", (void*)1);
  hashmap_put(map, "c_memmove", (void*)1);
  hashmap_put(map, "__STDC_UTF_16__", (void*)1);
  hashmap_put(map, "__volatile__", (void*)1);
  hashmap_put(map, "__FILE__", (void*)1);
  hashmap_put(map, "__TIMESTAMP__", (void*)1);
  hashmap_put(map, "__DATE__", (void*)1);
  hashmap_put(map, "i_malloc", (void*)1);
  hashmap_put(map, "c_realloc", (void*)1);
  hashmap_put(map, "c_less_unsigned", (void*)1);
  hashmap_put(map, "c_default_toraw", (void*)1);
  hashmap_put(map, "c_rs_2", (void*)1);
  hashmap_put(map, "c_NPOS", (void*)1);
  hashmap_put(map, "c_memcmp_eq", (void*)1);
  hashmap_put(map, "c_with", (void*)1);
  hashmap_put(map, "c_memset", (void*)1);
  hashmap_put(map, "__STDC_HOSTED__", (void*)1);
  hashmap_put(map, "__dyibicc__", (void*)1);
  hashmap_put(map, "_AMD64_", (void*)1);
  hashmap_put(map, "c_SV", (void*)1);
  hashmap_put(map, "__COUNTER__", (void*)1);
  hashmap_put(map, "stc_hash", (void*)1);
  hashmap_put(map, "forward_cset", (void*)1);
  hashmap_put(map, "__const__", (void*)1);
  hashmap_put(map, "__TIME__", (void*)1);
  hashmap_put(map, "c_swap", (void*)1);
  hashmap_put(map, "c_default_eq", (void*)1);
  hashmap_put(map, "c_foreach", (void*)1);
  hashmap_put(map, "forward_deq", (void*)1);
  hashmap_put(map, "WIN32", (void*)1);
  hashmap_put(map, "_c_RSEQ_N", (void*)1);
  hashmap_put(map, "c_forrange_2", (void*)1);
  hashmap_put(map, "CCOMMON_H_INCLUDED", (void*)1);
  hashmap_put(map, "ccharptr_drop", (void*)1);
  hashmap_put(map, "__BASE_FILE__", (void*)1);
  hashmap_put(map, "c_forrange_1", (void*)1);
  hashmap_put(map, "c_forrange_3", (void*)1);
  hashmap_put(map, "c_forrange_4", (void*)1);
  hashmap_put(map, "c_default_clone", (void*)1);
  hashmap_put(map, "c_litstrlen", (void*)1);
  hashmap_put(map, "c_const_cast", (void*)1);
  hashmap_put(map, "__int16", (void*)1);
  hashmap_put(map, "__inline__", (void*)1);
  hashmap_put(map, "__C99_MACRO_WITH_VA_ARGS", (void*)1);
  hashmap_put(map, "__x86_64", (void*)1);
  hashmap_put(map, "forward_cbox", (void*)1);
  hashmap_put(map, "_i_alloc", (void*)1);
  hashmap_put(map, "_MSC_VER", (void*)1);
  hashmap_put(map, "forward_arc", (void*)1);
  hashmap_put(map, "__amd64__", (void*)1);
  hashmap_put(map, "forward_cdeq", (void*)1);
  hashmap_put(map, "__ptr64", (void*)1);
  hashmap_put(map, "__STDC_UTF_32__", (void*)1);
  hashmap_put(map, "STC_FORWARD_H_INCLUDED", (void*)1);
  hashmap_put(map, "i_realloc", (void*)1);
  hashmap_put(map, "c_new_n", (void*)1);
  hashmap_put(map, "i_val", (void*)1);
  hashmap_put(map, "__alignof", (void*)1);
  hashmap_put(map, "c_default_less", (void*)1);
  hashmap_put(map, "__forceinline", (void*)1);
  hashmap_put(map, "c_forlist", (void*)1);
  hashmap_put(map, "__SIZEOF_PTRDIFF_T__", (void*)1);
  hashmap_put(map, "_M_X64", (void*)1);
  hashmap_put(map, "c_EXPAND", (void*)1);
  hashmap_put(map, "c_foriter", (void*)1);
  hashmap_put(map, "__SIZEOF_LONG_LONG__", (void*)1);
  hashmap_put(map, "_WIN64", (void*)1);
  hashmap_put(map, "__SIZE_TYPE__", (void*)1);
  hashmap_put(map, "_MSC_EXTENSIONS", (void*)1);
  hashmap_put(map, "i_tag", (void*)1);
  hashmap_put(map, "i_allocator", (void*)1);
  hashmap_put(map, "c_delete", (void*)1);
  hashmap_put(map, "c_memcmp", (void*)1);
  hashmap_put(map, "c_default_cmp", (void*)1);
  hashmap_put(map, "_i_prefix", (void*)1);
  hashmap_put(map, "c_LITERAL", (void*)1);
  hashmap_put(map, "assert", (void*)1);
  hashmap_put(map, "c_sv_1", (void*)1);
  hashmap_put(map, "c_calloc", (void*)1);
  hashmap_put(map, "c_forindexed", (void*)1);
  hashmap_put(map, "c_sv_2", (void*)1);
  hashmap_put(map, "forward_csmap", (void*)1);
  hashmap_put(map, "_NO_CRT_STDIO_INLINE", (void*)1);
  hashmap_put(map, "c_strlen", (void*)1);
  hashmap_put(map, "__cdecl", (void*)1);
  hashmap_put(map, "__SIZEOF_POINTER__", (void*)1);
  hashmap_put(map, "_c_APPLY_ARG_N", (void*)1);
  hashmap_put(map, "forward_cpque", (void*)1);
  hashmap_put(map, "__SIZEOF_LONG__", (void*)1);
  hashmap_put(map, "stc_hash_1", (void*)1);
  hashmap_put(map, "c_with_2", (void*)1);
  hashmap_put(map, "c_with_3", (void*)1);
  hashmap_put(map, "forward_cstack", (void*)1);
  hashmap_put(map, "__LINE__", (void*)1);
  hashmap_put(map, "forward_csset", (void*)1);
  hashmap_put(map, "c_umul128", (void*)1);
  hashmap_put(map, "forward_box", (void*)1);
  hashmap_put(map, "__amd64", (void*)1);
  hashmap_put(map, "STC_INLINE", (void*)1);
  hashmap_put(map, "c_forpair", (void*)1);
  hashmap_put(map, "c_u2i_size", (void*)1);
  hashmap_put(map, "_LP64", (void*)1);
  hashmap_put(map, "__declspec", (void*)1);
  hashmap_put(map, "i_key", (void*)1);
  hashmap_put(map, "$map", (void*)1);
  hashmap_put(map, "c_sv", (void*)1);
  hashmap_put(map, "ccharptr_clone", (void*)1);
  hashmap_put(map, "forward_hmap", (void*)1);
  hashmap_put(map, "forward_hset", (void*)1);
  hashmap_put(map, "forward_smap", (void*)1);
  hashmap_put(map, "forward_sset", (void*)1);
  hashmap_put(map, "forward_stack", (void*)1);
  hashmap_put(map, "forward_pque", (void*)1);
  hashmap_put(map, "forward_queue", (void*)1);
  hashmap_put(map, "forward_vec", (void*)1);
  hashmap_put(map, "c_true", (void*)1);
  hashmap_put(map, "c_false", (void*)1);
  hashmap_put(map, "_c_carc_types", (void*)1);
  hashmap_put(map, "_c_cbox_types", (void*)1);
  hashmap_put(map, "_c_cdeq_types", (void*)1);
  hashmap_put(map, "_c_clist_types", (void*)1);
  hashmap_put(map, "_c_chash_types", (void*)1);
  hashmap_put(map, "_c_aatree_types", (void*)1);
  hashmap_put(map, "_c_cstack_fixed", (void*)1);
  hashmap_put(map, "_c_cstack_types", (void*)1);
  hashmap_put(map, "_c_cvec_types", (void*)1);
  hashmap_put(map, "_c_cpque_types", (void*)1);
  hashmap_put(map, "_i_ismap", (void*)1);  // <--- Inserted OK
  hashmap_put(map, "_i_MAP_ONLY", (void*)1);
  hashmap_put(map, "_i_SET_ONLY", (void*)1);
  hashmap_put(map, "_i_keyref", (void*)1);
  hashmap_put(map, "_i_ishash", (void*)1);
  hashmap_put(map, "_i_template", (void*)1);
  hashmap_put(map, "STC_TEMPLATE_H_INCLUDED", (void*)1);
  hashmap_put(map, "_c_MEMB", (void*)1);
  hashmap_put(map, "_c_DEFTYPES", (void*)1);
  hashmap_put(map, "_m_value", (void*)1);
  hashmap_put(map, "_m_key", (void*)1);
  hashmap_put(map, "_m_mapped", (void*)1);
  hashmap_put(map, "_m_rmapped", (void*)1);
  hashmap_put(map, "_m_raw", (void*)1);
  hashmap_put(map, "_m_keyraw", (void*)1);
  hashmap_put(map, "_m_iter", (void*)1);
  hashmap_put(map, "_m_result", (void*)1);
  hashmap_put(map, "_m_node", (void*)1);
  hashmap_put(map, "i_type", (void*)1);
  hashmap_put(map, "c_option", (void*)1);
  hashmap_put(map, "c_is_forward", (void*)1);
  hashmap_put(map, "c_no_atomic", (void*)1);
  hashmap_put(map, "c_no_clone", (void*)1);
  hashmap_put(map, "c_no_emplace", (void*)1);
  hashmap_put(map, "c_no_hash", (void*)1);
  hashmap_put(map, "c_use_cmp", (void*)1);  // <--- Inserted OK
  hashmap_put(map, "c_more", (void*)1);
  hashmap_put(map, "i_use_cmp", (void*)1);
  hashmap_put(map, "_i_has_cmp", (void*)1);
  hashmap_put(map, "_i_has_eq", (void*)1);
  hashmap_put(map, "i_hash", (void*)1);
  hashmap_put(map, "i_eq", (void*)1);
  hashmap_put(map, "i_less", (void*)1);
  hashmap_put(map, "i_cmp", (void*)1);
  hashmap_put(map, "i_keyraw", (void*)1);
  hashmap_put(map, "i_keyfrom", (void*)1);
  hashmap_put(map, "i_keyto", (void*)1);
  hashmap_put(map, "i_keyclone", (void*)1);
  hashmap_put(map, "i_keydrop", (void*)1);
  hashmap_put(map, "i_valraw", (void*)1);
  hashmap_put(map, "i_valfrom", (void*)1);
  hashmap_put(map, "i_valto", (void*)1);
  hashmap_put(map, "i_valclone", (void*)1);
  hashmap_put(map, "i_valdrop", (void*)1);
  hashmap_put(map, "i_no_emplace", (void*)1);
  hashmap_put(map, "i_max_load_factor", (void*)1);
  hashmap_put(map, "fastrange_2", (void*)1);
  hashmap_delete(map, "i_max_load_factor");
  hashmap_delete(map, "_i_ismap");  // <--- Removed in the middle of chain of c_use_cmp.
  hashmap_delete(map, "_i_ishash");
  hashmap_delete(map, "_i_keyref");
  hashmap_delete(map, "_i_MAP_ONLY");
  hashmap_delete(map, "_i_SET_ONLY");
  hashmap_put(map, "CMAP_H_INCLUDED", (void*)1);
  hashmap_delete(map, "i_type");
  hashmap_delete(map, "i_tag");
  hashmap_delete(map, "i_less");
  hashmap_delete(map, "i_cmp");
  hashmap_delete(map, "i_eq");
  hashmap_delete(map, "i_hash");
  hashmap_delete(map, "i_val");
  hashmap_delete(map, "i_valraw");
  hashmap_delete(map, "i_valclone");
  hashmap_delete(map, "i_valfrom");
  hashmap_delete(map, "i_valto");
  hashmap_delete(map, "i_valdrop");
  hashmap_delete(map, "i_key");
  hashmap_delete(map, "i_keyraw");
  hashmap_delete(map, "i_keyclone");
  hashmap_delete(map, "i_keyfrom");
  hashmap_delete(map, "i_keyto");
  hashmap_delete(map, "i_keydrop");
  hashmap_delete(map, "i_use_cmp");
  hashmap_delete(map, "i_no_emplace");
  hashmap_delete(map, "_i_has_cmp");
  hashmap_delete(map, "_i_has_eq");
  hashmap_delete(map, "_i_prefix");
  hashmap_delete(map, "_i_template");
  hashmap_delete(map, "i_allocator");
  hashmap_delete(map, "i_malloc");
  hashmap_delete(map, "i_calloc");
  hashmap_delete(map, "i_realloc");
  hashmap_delete(map, "i_free");
  hashmap_delete(map, "i_static");
  hashmap_put(map, "i_key", (void*)1);
  hashmap_put(map, "i_tag", (void*)1);
  hashmap_put(map, "_vec.h", (void*)1);
  hashmap_put(map, "_i_prefix", (void*)1);
  hashmap_put(map, "assert", (void*)1);
  hashmap_put(map, "NULL", (void*)1);
  hashmap_delete(map, "STC_API");
  hashmap_delete(map, "STC_DEF");
  hashmap_put(map, "i_static", (void*)1);
  hashmap_put(map, "STC_API", (void*)1);
  hashmap_put(map, "STC_DEF", (void*)1);
  hashmap_put(map, "i_allocator", (void*)1);
  hashmap_put(map, "i_malloc", (void*)1);
  hashmap_put(map, "i_calloc", (void*)1);
  hashmap_put(map, "i_realloc", (void*)1);
  hashmap_put(map, "i_free", (void*)1);
  hashmap_put(map, "_it2_ptr", (void*)1);
  hashmap_put(map, "_it_ptr", (void*)1);
  hashmap_put(map, "_i_template", (void*)1);
  hashmap_put(map, "i_type", (void*)1);
  hashmap_put(map, "c_option", (void*)1);
  hashmap_put(map, "c_is_forward", (void*)1);
  hashmap_put(map, "c_no_atomic", (void*)1);
  hashmap_put(map, "c_no_clone", (void*)1);
  hashmap_put(map, "c_no_emplace", (void*)1);
  hashmap_put(map, "c_no_hash", (void*)1);
  hashmap_put(map, "c_use_cmp",
              (void*)1);  // <--- Finds TOMBSTONE in chain, but is from _i_ismap, not c_use_cmp.

  // At this point, there are two "c_use_cmp"s in the map, and so things are
  // broken. The rest of the puts are just to cause a rehash which is when we
  // realize that things have gone wrong.

  hashmap_put(map, "c_more", (void*)1);
  hashmap_put(map, "i_hash", (void*)1);
  hashmap_put(map, "i_eq", (void*)1);
  hashmap_put(map, "i_less", (void*)1);
  hashmap_put(map, "i_cmp", (void*)1);
  hashmap_put(map, "i_keyraw", (void*)1);
  hashmap_put(map, "i_keyfrom", (void*)1);
  hashmap_put(map, "i_keyto", (void*)1);
  hashmap_put(map, "i_keyclone", (void*)1);
  hashmap_put(map, "i_keydrop", (void*)1);
  hashmap_put(map, "i_val", (void*)1);
  hashmap_put(map, "i_valraw", (void*)1);
  hashmap_put(map, "i_no_emplace", (void*)1);
  hashmap_put(map, "CVEC_H_INCLUDED", (void*)1);
  hashmap_delete(map, "i_type");
  hashmap_delete(map, "i_tag");
  hashmap_delete(map, "i_less");
  hashmap_delete(map, "i_cmp");
  hashmap_delete(map, "i_eq");
  hashmap_delete(map, "i_hash");
  hashmap_delete(map, "i_val");
  hashmap_delete(map, "i_valraw");
  hashmap_delete(map, "i_key");
  hashmap_delete(map, "i_keyraw");
  hashmap_delete(map, "i_keyclone");
  hashmap_delete(map, "i_keyfrom");
  hashmap_delete(map, "i_keyto");
  hashmap_delete(map, "i_keydrop");
  hashmap_delete(map, "i_no_emplace");
  hashmap_delete(map, "_i_prefix");
  hashmap_delete(map, "_i_template");
  hashmap_delete(map, "i_allocator");
  hashmap_delete(map, "i_malloc");
  hashmap_delete(map, "i_calloc");
  hashmap_delete(map, "i_realloc");
  hashmap_delete(map, "i_free");
  hashmap_delete(map, "i_static");
  hashmap_put(map, "sal.h", (void*)1);
  hashmap_put(map, "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\shared/sal.h",
              (void*)1);
  hashmap_put(map, "__ATTR_SAL", (void*)1);
  hashmap_put(map, "_SAL_VERSION", (void*)1);
  hashmap_put(map, "__SAL_H_VERSION", (void*)1);
  hashmap_put(map, "_USE_DECLSPECS_FOR_SAL", (void*)1);
  hashmap_put(map, "_USE_ATTRIBUTES_FOR_SAL", (void*)1);
  hashmap_put(map, "_SAL1_Source_", (void*)1);
  hashmap_put(map, "_SAL1_1_Source_", (void*)1);
  hashmap_put(map, "_SAL1_2_Source_", (void*)1);
  hashmap_put(map, "_SAL2_Source_", (void*)1);
  hashmap_put(map, "_SAL_L_Source_", (void*)1);
  hashmap_put(map, "_At_", (void*)1);
  hashmap_put(map, "_At_buffer_", (void*)1);
  hashmap_put(map, "_When_", (void*)1);
  hashmap_put(map, "_Group_", (void*)1);
  hashmap_put(map, "_GrouP_", (void*)1);
  hashmap_put(map, "_Success_", (void*)1);
  hashmap_put(map, "_Return_type_success_", (void*)1);
  hashmap_put(map, "_On_failure_", (void*)1);
  hashmap_put(map, "_Always_", (void*)1);
  hashmap_put(map, "_Use_decl_annotations_", (void*)1);
  hashmap_put(map, "_Notref_", (void*)1);
  hashmap_put(map, "_Pre_defensive_", (void*)1);
  hashmap_put(map, "_Post_defensive_", (void*)1);
  hashmap_put(map, "_In_defensive_", (void*)1);
  hashmap_put(map, "_Out_defensive_", (void*)1);
  hashmap_put(map, "_Inout_defensive_", (void*)1);
  hashmap_put(map, "_Reserved_", (void*)1);
  hashmap_put(map, "_Const_", (void*)1);
  hashmap_put(map, "_In_", (void*)1);
  hashmap_put(map, "_In_opt_", (void*)1);
  hashmap_put(map, "_In_z_", (void*)1);
  hashmap_put(map, "_In_opt_z_", (void*)1);
  hashmap_put(map, "_In_reads_", (void*)1);
  hashmap_put(map, "_In_reads_opt_", (void*)1);
  hashmap_put(map, "_In_reads_bytes_", (void*)1);
  hashmap_put(map, "_In_reads_bytes_opt_", (void*)1);
  hashmap_put(map, "_In_reads_z_", (void*)1);
  hashmap_put(map, "_In_reads_opt_z_", (void*)1);
  hashmap_put(map, "_In_reads_or_z_", (void*)1);
  hashmap_put(map, "_In_reads_or_z_opt_", (void*)1);
  hashmap_put(map, "_In_reads_to_ptr_", (void*)1);
  hashmap_put(map, "_In_reads_to_ptr_opt_", (void*)1);
  hashmap_put(map, "_In_reads_to_ptr_z_", (void*)1);
  hashmap_put(map, "_In_reads_to_ptr_opt_z_", (void*)1);
  hashmap_put(map, "_Out_", (void*)1);
  hashmap_put(map, "_Out_opt_", (void*)1);
  hashmap_put(map, "_Out_writes_", (void*)1);
  hashmap_put(map, "_Out_writes_opt_", (void*)1);
  hashmap_put(map, "_Out_writes_bytes_", (void*)1);
  hashmap_put(map, "_Out_writes_bytes_opt_", (void*)1);
  hashmap_put(map, "_Out_writes_z_", (void*)1);
  hashmap_put(map, "_Out_writes_opt_z_", (void*)1);
  hashmap_put(map, "_Out_writes_to_", (void*)1);
  hashmap_put(map, "_Out_writes_to_opt_", (void*)1);
  hashmap_put(map, "_Out_writes_all_", (void*)1);
  hashmap_put(map, "_Out_writes_all_opt_", (void*)1);
  hashmap_put(map, "_Out_writes_bytes_to_", (void*)1);
  hashmap_put(map, "_Out_writes_bytes_to_opt_", (void*)1);
  hashmap_put(map, "_Out_writes_bytes_all_", (void*)1);
  hashmap_put(map, "_Out_writes_bytes_all_opt_", (void*)1);
  hashmap_put(map, "_Out_writes_to_ptr_", (void*)1);
  hashmap_put(map, "_Out_writes_to_ptr_opt_", (void*)1);
  hashmap_put(map, "_Out_writes_to_ptr_z_", (void*)1);
  hashmap_put(map, "_Out_writes_to_ptr_opt_z_", (void*)1);
  hashmap_put(map, "_Inout_", (void*)1);
  hashmap_put(map, "_Inout_opt_", (void*)1);
  hashmap_put(map, "_Inout_z_", (void*)1);
  hashmap_put(map, "_Inout_opt_z_", (void*)1);
  hashmap_put(map, "_Inout_updates_", (void*)1);
  hashmap_put(map, "_Inout_updates_opt_", (void*)1);
  hashmap_put(map, "_Inout_updates_z_", (void*)1);
  hashmap_put(map, "_Inout_updates_opt_z_", (void*)1);
  hashmap_put(map, "_Inout_updates_to_", (void*)1);
  hashmap_put(map, "_Inout_updates_to_opt_", (void*)1);
  hashmap_put(map, "_Inout_updates_all_", (void*)1);
  hashmap_put(map, "_Inout_updates_all_opt_", (void*)1);
  hashmap_put(map, "_Inout_updates_bytes_", (void*)1);
  hashmap_put(map, "_Inout_updates_bytes_opt_", (void*)1);
  hashmap_put(map, "_Inout_updates_bytes_to_", (void*)1);
  hashmap_put(map, "_Inout_updates_bytes_to_opt_", (void*)1);
  hashmap_put(map, "_Inout_updates_bytes_all_", (void*)1);
  hashmap_put(map, "_Inout_updates_bytes_all_opt_", (void*)1);
  hashmap_put(map, "_Outptr_", (void*)1);
  hashmap_put(map, "_Outptr_result_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_opt_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_result_z_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_z_", (void*)1);
  hashmap_put(map, "_Outptr_result_maybenull_z_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_maybenull_z_", (void*)1);
  hashmap_put(map, "_Outptr_result_nullonfailure_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_nullonfailure_", (void*)1);
  hashmap_put(map, "_COM_Outptr_", (void*)1);
  hashmap_put(map, "_COM_Outptr_result_maybenull_", (void*)1);
  hashmap_put(map, "_COM_Outptr_opt_", (void*)1);
  hashmap_put(map, "_COM_Outptr_opt_result_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_result_buffer_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_buffer_", (void*)1);
  hashmap_put(map, "_Outptr_result_buffer_to_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_buffer_to_", (void*)1);
  hashmap_put(map, "_Outptr_result_buffer_all_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_buffer_all_", (void*)1);
  hashmap_put(map, "_Outptr_result_buffer_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_buffer_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_result_buffer_to_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_buffer_to_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_result_buffer_all_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_buffer_all_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_result_bytebuffer_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_bytebuffer_", (void*)1);
  hashmap_put(map, "_Outptr_result_bytebuffer_to_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_bytebuffer_to_", (void*)1);
  hashmap_put(map, "_Outptr_result_bytebuffer_all_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_bytebuffer_all_", (void*)1);
  hashmap_put(map, "_Outptr_result_bytebuffer_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_bytebuffer_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_result_bytebuffer_to_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_bytebuffer_to_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_result_bytebuffer_all_maybenull_", (void*)1);
  hashmap_put(map, "_Outptr_opt_result_bytebuffer_all_maybenull_", (void*)1);
  hashmap_put(map, "_Outref_", (void*)1);
  hashmap_put(map, "_Outref_result_maybenull_", (void*)1);

  // Assert due to rehash on previous line.
}

void hashmap_test(void) {
  HashMap *map = calloc(1, sizeof(HashMap));

  for (int i = 0; i < 5000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);
  for (int i = 1000; i < 2000; i++)
    hashmap_delete(map, format("key %d", i));
  for (int i = 1500; i < 1600; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);
  for (int i = 6000; i < 7000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);

  for (int i = 0; i < 1000; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);
  for (int i = 1000; i < 1500; i++)
    assert(hashmap_get(map, "no such key") == NULL);
  for (int i = 1500; i < 1600; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);
  for (int i = 1600; i < 2000; i++)
    assert(hashmap_get(map, "no such key") == NULL);
  for (int i = 2000; i < 5000; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);
  for (int i = 5000; i < 6000; i++)
    assert(hashmap_get(map, "no such key") == NULL);
  for (int i = 6000; i < 7000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);

  assert(hashmap_get(map, "no such key") == NULL);

  deletion_ordering_test();

  printf("OK\n");
}
