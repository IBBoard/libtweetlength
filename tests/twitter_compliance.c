/*  This file is part of libtweetlength
 *  Copyright (C) 2021 IBBoard
 *
 *  libtweetlength is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libtweetlength is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libtweetlength.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <yaml.h>
#include "libtweetlength.h"

typedef enum {
    SKIP,
    DESCRIPTION,
    TEXT,
    WEIGHTED_LENGTH
} ScalarValue;

static gchar *basedir;

static void
test_compliance (void) {
    yaml_parser_t parser;
    yaml_event_t event;

    FILE *input = fopen(g_strconcat(basedir, "/validate.yml", NULL), "r");

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, input);
    int done = 0;
    gboolean run_tests = FALSE;
    guint count_mode = COUNT_BASIC;
    int indent = 0;

    char *description;
    char *text;
    gint64 expected_length;
    int next_scalar = SKIP;
    gchar *value;

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            g_warning("Parsing failed");
            g_test_fail ();
            return;
        }

        switch(event.type)
        {
            case YAML_MAPPING_START_EVENT:
                indent += 1;
                if (indent <= 3 && run_tests) {
                    g_free(description);
                    description = NULL;
                    g_free(text);
                    text = NULL;
                    expected_length = 0;
                }
                break;
            case YAML_MAPPING_END_EVENT:
                if (indent == 3 && run_tests) {
                    gsize length = tl_count_weighted_characters(text, count_mode) ;
                    if (length != expected_length) {
                        g_test_fail();
                        g_warning("Test '%s': expected length %ld but got %ld for text \"%s\"", description, expected_length, length, text);
                    }
                }
                indent -= 1;
                break;
            case YAML_SCALAR_EVENT:
                value = (gchar *)event.data.scalar.value;
                if (indent == 2) {
                    if (g_strcmp0(value, "WeightedTweetsCounterTest") == 0) {
                        run_tests = TRUE;
                        count_mode = COUNT_SHORT_URLS;
                    }
                    else if (g_strcmp0(value, "WeightedTweetsWithDiscountedEmojiCounterTest") == 0) {
                        run_tests = TRUE;
                        count_mode = COUNT_COMPACT;
                    }
                    else {
                        run_tests = FALSE;
                    }
                }
                else if (indent >= 3 && run_tests) {
                    if (next_scalar != SKIP) {
                        if (next_scalar == DESCRIPTION) {
                            description = g_strdup(value);
                        }
                        else if (next_scalar == TEXT) {
                            text = g_strdup(value);
                        }
                        else if (next_scalar == WEIGHTED_LENGTH) {
                            expected_length = g_ascii_strtoll((const gchar *)value, NULL, 10);
                        }
                        next_scalar = SKIP;
                    }
                    else if (g_strcmp0(value, "description") == 0) {
                        next_scalar = DESCRIPTION;
                    }
                    else if (g_strcmp0(value, "text") == 0) {
                        next_scalar = TEXT;
                    }
                    else if (g_strcmp0(value, "weightedLength") == 0) {
                        next_scalar = WEIGHTED_LENGTH;
                    }
                }
                break;
            default:
                break;
        }

        done = (event.type == YAML_STREAM_END_EVENT);
        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
}

int
main (int argc, char **argv)
{
    basedir = g_path_get_dirname(argv[0]);
    g_test_init (&argc, &argv, NULL);

#ifdef LIBTL_DEBUG
    g_setenv ("G_MESSAGES_DEBUG", "libtl", TRUE);
#endif

    g_test_add_func ("/twitter_compliance/test_compliance", test_compliance);

    return g_test_run ();
}