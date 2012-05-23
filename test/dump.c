/*
Copyright 2009 Dan Nicholson

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xkbcommon/xkbcommon.h"

int main(int argc, char *argv[])
{
    struct xkb_context *ctx = xkb_context_new(0);
    struct xkb_keymap *keymap;
    struct xkb_rule_names names = {
        .rules = "evdev",
        .model = "pc105",
        .layout = "gb",
        .variant = NULL,
        .options = "terminate:ctrl_alt_bksp",
    };
    char *as_string;

    assert(ctx);
    keymap = xkb_map_new_from_names(ctx, &names, 0);

    as_string = xkb_map_get_as_string(keymap);
    assert(as_string);
    printf("%s\n", as_string);

    free(as_string);
    xkb_map_unref(keymap);
    xkb_context_unref(ctx);

    return 0;
}