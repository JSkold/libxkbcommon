/*
Copyright 1985, 1987, 1990, 1998  The Open Group
Copyright 2008  Dan Nicholson

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the names of the authors or their
institutions shall not be used in advertising or otherwise to promote the
sale, use or other dealings in this Software without prior written
authorization from the authors.
*/

/************************************************************
Copyright (c) 1993 by Silicon Graphics Computer Systems, Inc.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Silicon Graphics not be
used in advertising or publicity pertaining to distribution
of the software without specific prior written permission.
Silicon Graphics makes no representation about the suitability
of this software for any purpose. It is provided "as is"
without any express or implied warranty.

SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SILICON
GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/*
 * Copyright © 2009 Daniel Stone
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Author: Daniel Stone <daniel@fooishbar.org>
 */


#ifndef _XKBCOMMON_H_
#define _XKBCOMMON_H_

#include <stddef.h>
#include <stdint.h>

typedef uint32_t xkb_keycode_t;
typedef uint32_t xkb_keysym_t;
typedef uint32_t xkb_mod_index_t;
typedef uint32_t xkb_mod_mask_t;
typedef uint32_t xkb_group_index_t;
typedef uint32_t xkb_led_index_t;

#define XKB_MOD_INVALID                 (0xffffffff)
#define XKB_GROUP_INVALID               (0xffffffff)
#define XKB_KEYCODE_INVALID             (0xffffffff)
#define XKB_LED_INVALID                 (0xffffffff)

#define XKB_KEYSYM_NO_SYMBOL            0

#define XKB_KEYCODE_MAX                 (0xffffffff - 1)
#define xkb_keycode_is_legal_ext(kc)    (kc <= XKB_KEYCODE_MAX)
#define xkb_keycode_is_legal_x11(kc)    (kc <= XKB_KEYCODE_MAX)
#define xkb_keymap_keycode_range_is_legal(xkb) \
    (xkb->max_key_code > 0 && \
     xkb->max_key_code > xkb->min_key_code && \
     xkb_keycode_is_legal_ext(xkb->min_key_code) && \
     xkb_keycode_is_legal_ext(xkb->max_key_code))

/**
 * Names to compile a keymap with, also known as RMLVO.  These names together
 * should be the primary identifier for a keymap.
 */
struct xkb_rule_names {
    char *rules;
    char *model;
    char *layout;
    char *variant;
    char *options;
};

/**
 * Legacy names for the components of an XKB keymap, also known as KcCGST.
 * This is only used in deprecated entrypoints which might be removed or
 * shuffled off to a support library.
 */
struct xkb_component_names {
    char *keymap;
    char *keycodes;
    char *types;
    char *compat;
    char *symbols;
};

/**
 * Opaque context object; may only be created, accessed, manipulated and
 * destroyed through the xkb_context_*() API.
 */
struct xkb_context;

/**
 * Opaque keymap object; may only be created, accessed, manipulated and
 * destroyed through the xkb_state_*() API.
 */
struct xkb_keymap;

/**
 * Opaque state object; may only be created, accessed, manipulated and
 * destroyed through the xkb_state_*() API.
 */
struct xkb_state;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Canonicalises component names by prepending the relevant component from
 * 'old' to the one in 'names' when the latter has a leading '+' or '|', and
 * by replacing a '%' with the relevant component, e.g.:
 *
 * names        old           output
 * ------------------------------------------
 * +bar         foo           foo+bar
 * |quux        baz           baz|quux
 * foo+%|baz    bar           foo+bar|baz
 *
 * If a component in names needs to be modified, the existing value will be
 * free()d, and a new one allocated with malloc().
 */
void
xkb_canonicalise_components(struct xkb_component_names *names,
                            const struct xkb_component_names *old);

/*
 * Converts a keysym to a string; will return unknown Unicode codepoints
 * as "Ua1b2", and other unknown keysyms as "0xabcd1234".
 */
void
xkb_keysym_to_string(xkb_keysym_t ks, char *buffer, size_t size);

/*
 * See xkb_keysym_to_string comments: this function will accept any string
 * from that function.
 */
xkb_keysym_t
xkb_string_to_keysym(const char *s);

/**
 * @defgroup ctx XKB contexts
 * Every keymap compilation request must have an XKB context associated with
 * it.  The context keeps around state such as the include path.
 *
 * @{
 */

/**
 * Returns a new XKB context, or NULL on failure.  If successful, the caller
 * holds a reference on the context, and must free it when finished with
 * xkb_context_unref().
 */
struct xkb_context *
xkb_context_new(void);

/**
 * Appends a new entry to the include path used for keymap compilation.
 * Returns 1 on success, or 0 if the include path could not be added or is
 * inaccessible.
 */
int
xkb_context_include_path_append(struct xkb_context *context, const char *path);

/**
 * Appends the default include paths to the context's current include path.
 * Returns 1 on success, or 0 if the primary include path could not be
 * added.
 */
int
xkb_context_include_path_append_default(struct xkb_context *context);

/**
 * Removes all entries from the context's include path, and inserts the
 * default paths.  Returns 1 on success, or 0 if the primary include path
 * could not be added.
 */
int
xkb_context_include_path_reset_defaults(struct xkb_context *context);

/**
 * Removes all entries from the context's include path.
 */
void
xkb_context_include_path_clear(struct xkb_context *context);

/**
 * Returns the number of include paths currently active in the context.
 */
unsigned int
xkb_context_num_include_paths(struct xkb_context *context);

/**
 * Returns the include path at the specified index within the context.
 */
const char *
xkb_context_include_path_get(struct xkb_context *context, unsigned int index);

/**
 * Takes a new reference on an XKB context.
 */
struct xkb_context *
xkb_context_ref(struct xkb_context *context);

/**
 * Releases a reference on an XKB context, and possibly frees it.
 */
void
xkb_context_unref(struct xkb_context *context);

/** @} */

/**
 * @defgroup map Keymap management
 * These utility functions allow you to create and deallocate XKB keymaps.
 *
 * @{
 */

/**
 * The primary keymap entry point: creates a new XKB keymap from a set of
 * RMLVO (Rules + Model + Layout + Variant + Option) names.
 *
 * You should almost certainly be using this and nothing else to create
 * keymaps.
 */
struct xkb_keymap *
xkb_map_new_from_names(struct xkb_context *context,
                       const struct xkb_rule_names *names);

/**
 * Deprecated entrypoint for legacy users who need to be able to compile
 * XKB keymaps by KcCGST (Keycodes + Compat + Geometry + Symbols + Types)
 * names.
 *
 * You should not use this unless you are the X server.  This entrypoint
 * may well disappear in future releases.  Please, please, don't use it.
 *
 * Geometry will be ignored since xkbcommon does not support it in any way.
 */
struct xkb_keymap *
xkb_map_new_from_kccgst(struct xkb_context *context,
                        const struct xkb_component_names *kccgst);

enum xkb_keymap_format {
    /** The current/classic XKB text format, as generated by xkbcomp -xkb. */
    XKB_KEYMAP_FORMAT_TEXT_V1 = 1,
};

/**
 * Creates an XKB keymap from a full text XKB keymap passed into the
 * file descriptor.
 */
struct xkb_keymap *
xkb_map_new_from_fd(struct xkb_context *context,
                    int fd, enum xkb_keymap_format format);

/**
 * Creates an XKB keymap from a full text XKB keymap serialised into one
 * enormous string.
 */
struct xkb_keymap *
xkb_map_new_from_string(struct xkb_context *context,
                        const char *string,
                        enum xkb_keymap_format format);

/**
 * Takes a new reference on a keymap.
 */
struct xkb_keymap *
xkb_map_ref(struct xkb_keymap *xkb);

/**
 * Releases a reference on a keymap.
 */
void
xkb_map_unref(struct xkb_keymap *xkb);

/** @} */

/**
 * @defgroup components XKB state components
 * Allows enumeration of state components such as modifiers and groups within
 * the current keymap.
 *
 * @{
 */

/**
 * Returns the number of modifiers active in the keymap.
 */
xkb_mod_index_t
xkb_map_num_mods(struct xkb_keymap *xkb);

/**
 * Returns the name of the modifier specified by 'idx', or NULL if invalid.
 */
const char *
xkb_map_mod_get_name(struct xkb_keymap *xkb, xkb_mod_index_t idx);

/**
 * Returns the index of the modifier specified by 'name', or XKB_MOD_INVALID.
 */
xkb_mod_index_t
xkb_map_mod_get_index(struct xkb_keymap *xkb, const char *name);

/**
 * Returns the number of groups active in the keymap.
 */
xkb_group_index_t
xkb_map_num_groups(struct xkb_keymap *xkb);

/**
 * Returns the name of the group specified by 'idx', or NULL if invalid.
 */
const char *
xkb_map_group_get_name(struct xkb_keymap *xkb, xkb_group_index_t idx);

/**
 * Returns the index of the group specified by 'name', or XKB_GROUP_INVALID.
 */
xkb_group_index_t
xkb_map_group_get_index(struct xkb_keymap *xkb, const char *name);

/**
 * Returns the number of groups active for the specified key.
 */
xkb_group_index_t
xkb_key_num_groups(struct xkb_keymap *xkb, xkb_keycode_t key);

/**
 * Returns the number of LEDs in the given map.
 */
xkb_led_index_t
xkb_map_num_leds(struct xkb_keymap *xkb);

/**
 * Returns the name of the LED specified by 'idx', or NULL if invalid.
 */
const char *
xkb_map_led_get_name(struct xkb_keymap *xkb, xkb_led_index_t idx);

/**
 * Returns the index of the LED specified by 'name', or XKB_LED_INVALID.
 */
xkb_led_index_t
xkb_map_led_get_index(struct xkb_keymap *xkb, const char *name);

/** @} */

/**
 * @defgroup state XKB state objects
 * Creation, destruction and manipulation of keyboard state objects,
 * representing modifier and group state.
 *
 * @{
 */

/**
 * Returns a new XKB state object for use with the given keymap, or NULL on
 * failure.
 */
struct xkb_state *
xkb_state_new(struct xkb_keymap *xkb);

/**
 * Takes a new reference on a state object.
 */
struct xkb_state *
xkb_state_ref(struct xkb_state *state);

/**
 * Unrefs and potentially deallocates a state object; the caller must not
 * use the state object after calling this.
 */
void
xkb_state_unref(struct xkb_state *state);

enum xkb_key_direction {
    XKB_KEY_UP,
    XKB_KEY_DOWN,
};

/**
 * Updates a state object to reflect the given key being pressed or released.
 */
void
xkb_state_update_key(struct xkb_state *state, xkb_keycode_t key,
                     enum xkb_key_direction direction);

/**
 * Gives the symbols obtained from pressing a particular key with the given
 * state.  *syms_out will be set to point to an array of keysyms, with the
 * return value being the number of symbols in *syms_out.  If the return
 * value is 0, *syms_out will be set to NULL, as there are no symbols produced
 * by this event.
 *
 * This should be called before xkb_state_update_key.
 */
unsigned int
xkb_key_get_syms(struct xkb_state *state, xkb_keycode_t key,
                 const xkb_keysym_t **syms_out);

/**
 * Modifier and group types for state objects.  This enum is bitmaskable,
 * e.g. (XKB_STATE_DEPRESSED | XKB_STATE_LATCHED) is valid to exclude
 * locked modifiers.
 */
enum xkb_state_component {
    /** A key holding this modifier or group is currently physically
     *  depressed; also known as 'base'. */
    XKB_STATE_DEPRESSED = (1 << 0),
    /** Modifier or group is latched, i.e. will be unset after the next
     *  non-modifier key press. */
    XKB_STATE_LATCHED = (1 << 1),
    /** Modifier or group is locked, i.e. will be unset after the key
     *  provoking the lock has been pressed again. */
    XKB_STATE_LOCKED = (1 << 2),
    /** Combinatination of depressed, latched, and locked. */
    XKB_STATE_EFFECTIVE =
        (XKB_STATE_DEPRESSED | XKB_STATE_LATCHED | XKB_STATE_LOCKED),
};

/**
 * Updates a state object from a set of explicit masks.  This entrypoint is
 * really only for window systems and the like, where a master process
 * holds an xkb_state, then serialises it over a wire protocol, and clients
 * then use the serialisation to feed in to their own xkb_state.
 *
 * All parameters must always be passed, or the resulting state may be
 * incoherent.
 *
 * The serialisation is lossy and will not survive round trips; it must only
 * be used to feed slave state objects, and must not be used to update the
 * master state.
 *
 * Please do not use this unless you fit the description above.
 */
void
xkb_state_update_mask(struct xkb_state *state,
                      xkb_mod_mask_t base_mods,
                      xkb_mod_mask_t latched_mods,
                      xkb_mod_mask_t locked_mods,
                      xkb_group_index_t base_group,
                      xkb_group_index_t latched_group,
                      xkb_group_index_t locked_group);

/**
 * The counterpart to xkb_state_update_mask, to be used on the server side
 * of serialisation.  Returns a xkb_mod_mask_t representing the given
 * component(s) of the state.
 *
 * This function should not be used in regular clients; please use the
 * xkb_state_mod_*_is_active or xkb_state_foreach_active_mod API instead.
 *
 * Can return NULL on failure.
 */
xkb_mod_mask_t
xkb_state_serialise_mods(struct xkb_state *state,
                         enum xkb_state_component component);

/**
 * The group equivalent of xkb_state_serialise_mods: please see its
 * documentation.
 */
xkb_group_index_t
xkb_state_serialise_group(struct xkb_state *state,
                          enum xkb_state_component component);

/**
 * Returns 1 if the modifier specified by 'name' is active in the manner
 * specified by 'type', 0 if it is unset, or -1 if the modifier does not
 * exist in the current map.
 */
int
xkb_state_mod_name_is_active(struct xkb_state *state, const char *name,
                             enum xkb_state_component type);

/**
 * Returns 1 if the modifier specified by 'idx' is active in the manner
 * specified by 'type', 0 if it is unset, or -1 if the modifier does not
 * exist in the current map.
 */
int
xkb_state_mod_index_is_active(struct xkb_state *state, xkb_mod_index_t idx,
                              enum xkb_state_component type);

/**
 * Returns 1 if the group specified by 'name' is active in the manner
 * specified by 'type', 0 if it is unset, or -1 if the group does not
 * exist in the current map.
 */
int
xkb_state_group_name_is_active(struct xkb_state *state, const char *name,
                               enum xkb_state_component type);

/**
 * Returns 1 if the group specified by 'idx' is active in the manner
 * specified by 'type', 0 if it is unset, or -1 if the group does not
 * exist in the current map.
 */
int
xkb_state_group_index_is_active(struct xkb_state *state, xkb_group_index_t idx,
                                enum xkb_state_component type);

/**
 * Returns 1 if the LED specified by 'name' is active, 0 if it is unset, or
 * -1 if the LED does not exist in the current map.
 */
int
xkb_state_led_name_is_active(struct xkb_state *state, const char *name);

/**
 * Returns 1 if the LED specified by 'idx' is active, 0 if it is unset, or
 * -1 if the LED does not exist in the current map.
 */
int
xkb_state_led_index_is_active(struct xkb_state *state, xkb_led_index_t idx);

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _XKBCOMMON_H_ */
