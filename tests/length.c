/*  This file is part of libtweetlength
 *  Copyright (C) 2017 Timm Bäder
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

#include "libtweetlength.h"
#include "../src/data.h"

static void
empty (void)
{
  g_assert_cmpint (tl_count_characters (""), ==, 0);
  g_assert_cmpint (tl_count_characters (NULL), ==, 0);
}

static void
nonempty (void)
{
  g_assert_cmpint (tl_count_characters ("ab"), ==, 2);
  g_assert_cmpint (tl_count_characters ("ABC"), ==, 3);
  g_assert_cmpint (tl_count_characters ("a b"), ==, 3);
  g_assert_cmpint (tl_count_characters (" "),  ==, 1);
  g_assert_cmpint (tl_count_characters ("1234"),  ==, 4);
  g_assert_cmpint (tl_count_characters ("sample tweet"),  ==, 12);
  g_assert_cmpint (tl_count_characters ("Foo     Bar"),  ==, 11);
  g_assert_cmpint (tl_count_characters ("Foo     "),  ==, 8);
}

static void
basic_links (void)
{
  g_assert_cmpint (tl_count_characters ("http://twitter.com"), ==, 23);
  g_assert_cmpint (tl_count_characters ("https://twitter.com"), ==, 23);
  g_assert_cmpint (tl_count_characters ("https://foobar.org/thisissolong/itsnotevenfunnyanymore"), ==, 23);

  // Questionmark at the end is not part of the link!
  g_assert_cmpint (tl_count_characters ("https://twitter.com/"), ==, 23);
  g_assert_cmpint (tl_count_characters ("https://twitter.com?"), ==, 24);
  g_assert_cmpint (tl_count_characters ("https://twitter.com."), ==, 24);
  g_assert_cmpint (tl_count_characters ("https://twitter.com("), ==, 24);

  // Neither is white space
  g_assert_cmpint (tl_count_characters ("https://twitter.com "), ==, 24);
  g_assert_cmpint (tl_count_characters ("https://twitter.com/ "), ==, 24);

  // Punctuation before...
  g_assert_cmpint (tl_count_characters (")https://twitter.com"), ==, 24);
  g_assert_cmpint (tl_count_characters ("?https://twitter.com"), ==, 24);
  g_assert_cmpint (tl_count_characters (",https://twitter.com"), ==, 24);

  // link + (foobar
  g_assert_cmpint (tl_count_characters ("https://twitter.asdf(foobar"), ==, 27); // Invalid TLD!
  g_assert_cmpint (tl_count_characters ("https://twitter.com(foobar"),  ==, 30);
  g_assert_cmpint (tl_count_characters ("https://twitter.com(foobar)"), ==, 31);

  // Query string
  g_assert_cmpint (tl_count_characters ("https://twitter.com?foo=bar"), ==, 23);
  g_assert_cmpint (tl_count_characters ("https://abc.def.twitter.com/foobar.html?abc=foo"), ==, 23);

  // Parens...
  g_assert_cmpint (tl_count_characters ("https://twitter.com/foobar(ZOMG)"), ==, 23);
  g_assert_cmpint (tl_count_characters ("https://en.wikipedia.org/wiki/Glob_(programming)#DOS_COMMAND.COM_and_Windows_cmd.exe"), ==, 23);

  // No Protocol...
  g_assert_cmpint (tl_count_characters ("twitter.com"), ==, 23);
  g_assert_cmpint (tl_count_characters ("twitter.com/"), ==, 23);
  g_assert_cmpint (tl_count_characters ("dhl.de"), ==, 23); // ccTLDs are now linkified without protocols
  g_assert_cmpint (tl_count_characters ("channel5.tv"), ==, 23); // As all ccTLDs are now linkified then .tv and .co are no longer special cases
  g_assert_cmpint (tl_count_characters ("d3.select"), ==, 23); // This is actually a JavaScript function, but ".select" is now a TLD so Twitter linkifies it
  g_assert_cmpint (tl_count_characters ("abc twitter.com/"), ==, 27);
  g_assert_cmpint (tl_count_characters ("/ twitter.com/"), ==, 25);
  g_assert_cmpint (tl_count_characters ("...https://twitter.com/"), ==, 26);
  g_assert_cmpint (tl_count_characters ("Foo(foobar.com)"), ==, 28);

  g_assert_cmpint (tl_count_characters ("Foo\nhttp://fooobar.org"), ==, 27);
}

static void
advanced_links (void)
{
  g_message ("\n");

  g_assert_cmpint (tl_count_characters (".twitter.com"), ==, 12);

  // Some examples from https://github.com/twitter/twitter-text/blob/master/conformance/validate.yml
  g_assert_cmpint (tl_count_characters ("https://example.com/path/to/resource?search=foo&lang=en"), ==, 23);
  g_assert_cmpint (tl_count_characters ("http://twitter.com/#!/twitter"), ==, 23);
  g_assert_cmpint (tl_count_characters ("HTTPS://www.ExaMPLE.COM/index.html"), ==, 23);

  // Port
  g_assert_cmpint (tl_count_characters ("foobar.com:8080/foo.html"), ==, 23);
  g_assert_cmpint (tl_count_characters ("foobar.com:8080//foo.html"), ==, 23);
  g_assert_cmpint (tl_count_characters ("foobar.com::8080/foo.html"), ==, 23 + 15);
  g_assert_cmpint (tl_count_characters ("http://foobar.com:abc/bla.html"), ==, 36);

  // Non-balanced parentheses, not part of the link
  g_assert_cmpint (tl_count_characters ("twitter.com/foo.html)"), ==, 24);
  g_assert_cmpint (tl_count_characters ("twitter.com/foo.html?"), ==, 24);
  g_assert_cmpint (tl_count_characters ("twitter.com/foo(.html)"), ==, 23); // balanced!
  g_assert_cmpint (tl_count_characters ("twitter.com/foo.html((a)"), ==, 27);
  g_assert_cmpint (tl_count_characters ("twitter.com/foo.html((a))"), ==, 23);
  g_assert_cmpint (tl_count_characters ("twitter.com/foo.html(((a)))"), ==, 30);

  // Should NOT be links.
  g_assert_cmpint (tl_count_characters ("foo:test@example.com"), ==, 20);
  g_assert_cmpint (tl_count_characters ("test@example.com"), ==, 16);

  // https://github.com/baedert/corebird/issues/471
  g_assert_cmpint (tl_count_characters ("My build of @Corebird (https://software.opensuse.org/download.html?project=home%3AIBBoard%3Adesktop&package=corebird) now comes with more theme compatibility for Adwaita-compliant themes"), ==, 140 - 24);

  g_assert_cmpint (tl_count_characters ("https://foobar.example.com.com.com.com"), ==, 23);

  // XXX ALL of these are noted as links in [1] but twitter.com says no FFS
  /*g_assert_cmpint (tl_count_characters ("http://user:PASSW0RD@example.com/"), ==, 23);*/
  /*g_assert_cmpint (tl_count_characters ("http://user:PASSW0RD@example.com:8080/login.php"), ==, 23);*/
  /*g_assert_cmpint (tl_count_characters ("http://user:PASSW0RD@example.com:8080/login.php"), ==, 23);*/

  // [1] https://github.com/twitter/twitter-text/blob/master/conformance/validate.yml
}

static void
utf8 (void)
{
  g_assert_cmpint (tl_count_characters ("ä"), ==, 1);
  g_assert_cmpint (tl_count_characters ("a 😭 a"), ==, 5);

  // New "weighted-length" calculation https://developer.twitter.com/en/docs/developer-utilities/twitter-text
  g_assert_cmpint (tl_count_weighted_characters ("a", COUNT_COMPACT), ==, 1);
  g_assert_cmpint (tl_count_weighted_characters ("ä", COUNT_COMPACT), ==, 1);
  g_assert_cmpint (tl_count_weighted_characters ("火", COUNT_COMPACT), ==, 2);
  g_assert_cmpint (tl_count_weighted_characters ("a 😭 a", COUNT_COMPACT), ==, 6);
  g_assert_cmpint (tl_count_weighted_characters ("https://twitter.com/", COUNT_COMPACT), ==, 23);
  g_assert_cmpint (tl_count_weighted_characters ("a https://twitter.com/ 火", COUNT_COMPACT), ==, 28);
  g_assert_cmpint (tl_count_weighted_characters ("I am a Tweet", COUNT_COMPACT), <, 20);
  g_assert_cmpint (tl_count_weighted_characters ("A lie gets halfway around the world before the truth has a chance to get its pants on. Winston Churchill (1874-1965) http://bit.ly/dJpywL", TRUE), ==, 140);
  g_assert_cmpint (tl_count_weighted_characters ("A lié géts halfway arøünd thé wørld béføré thé truth has a chance tø get its pants øn. Winston Churchill (1874-1965) http://bit.ly/dJpywL", TRUE), ==, 140);
  g_assert_cmpint (tl_count_weighted_characters ("のののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののの", TRUE), ==, 280);
}

static void
validate (void)
{

  g_assert_cmpint (tl_count_characters ("I am a Tweet"), <, 20);
  g_assert_cmpint (tl_count_characters ("A lie gets halfway around the world before the truth has a chance to get its pants on. Winston Churchill (1874-1965) http://bit.ly/dJpywL"), ==, 140);
  g_assert_cmpint (tl_count_characters ("A lié géts halfway arøünd thé wørld béføré thé truth has a chance tø get its pants øn. Winston Churchill (1874-1965) http://bit.ly/dJpywL"), ==, 140);
  g_assert_cmpint (tl_count_characters ("のののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののの"), ==, 140);
  // XXX Both libtl and twitter.com count this as 143 characters, but [1] says it should be 141...
  g_assert_cmpint (tl_count_characters ("A lie gets halfway around the world before the truth has a chance to get its pants on. \n- Winston Churchill (1874-1965) http://bit.ly/dJpywL"), ==, 143);

  // [1] https://github.com/twitter/twitter-text/blob/master/conformance/validate.yml
}

static void
emoji (void)
{
  // Based on what Twitter supports, based on https://unicode.org/emoji/charts/emoji-zwj-sequences.html tested in https://twitter.com/IBBoard/status/1373292346033442820/photo/1
  // (rather than picking apart the regex, which is messy because of the "\u…" handling in Java vs Vala)
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468", COUNT_COMPACT), ==, 2); // Man
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D", COUNT_COMPACT), ==, 3); // Man and trailing ZWJ
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\U0001F468", COUNT_COMPACT), ==, 5); // Man+ZWJ+Man (doesn't combine)
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\U0001F468\u200D", COUNT_COMPACT), ==, 6); // Man+ZWJ+Man and trailing ZWJ
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\U0001F468\u200D\U0001F466", COUNT_COMPACT), ==, 2); // Man+ZWJ+Man+ZWJ+Girl
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\U0001F468\u200D\U0001F466\u200D", COUNT_COMPACT), ==, 3); // Man+ZWJ+Man+ZWJ+Girl and trailing ZWJ
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\U0001F468\u200D\U0001F466\u200D\U0001F466", COUNT_COMPACT), ==, 2); // Man+ZWJ+Man+ZWJ+Girl+ZWJ+Girl
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\u200D\U0001F467", COUNT_COMPACT), ==, 2); // One-parent, one-child family
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\u200D\U0001F467\u200D\U0001F467", COUNT_COMPACT), ==, 2); // One-parent, two-child family

  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\u200D\u2764\uFE0F\u200D\U0001F468", COUNT_COMPACT), ==, 2); // Woman and man love
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\u2764\uFE0F\u200D\U0001F468", COUNT_COMPACT), ==, 2); // Man and man love
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\u200D\u2764\uFE0F\u200D\U0001F469", COUNT_COMPACT), ==, 2); // Woman and woman love
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\u2764\uFE0F\u200D\U0001F469", COUNT_COMPACT), ==, 10); // Man and woman breaks it
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\U0001F3FF\u200D\u2764\uFE0F\u200D\U0001F469", COUNT_COMPACT), ==, 10); // Fitzpatrick breaks it
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\u200D\u2764\uFE0F\u200D\U0001F469\U0001F3FF", COUNT_COMPACT), ==, 4); // Fitzpatrick after is an extra

  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\u200D\u2764\uFE0F\u200D\U0001F48B\u200D\U0001F468", COUNT_COMPACT), ==, 2); // Woman and man kissing
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\u2764\uFE0F\u200D\U0001F48B\u200D\U0001F468", COUNT_COMPACT), ==, 2); // Man and man kissing
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\u200D\u2764\uFE0F\u200D\U0001F48B\u200D\U0001F469", COUNT_COMPACT), ==, 2); // Woman and woman kissing
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\u2764\uFE0F\u200D\U0001F48B\u200D\U0001F469", COUNT_COMPACT), ==, 13); // Man and Woman breaks it
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\U0001F3FF\u200D\u2764\uFE0F\u200D\U0001F48B\u200D\U0001F469", COUNT_COMPACT), ==, 13); // Fitzpatrick breaks it
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\u200D\u2764\uFE0F\u200D\U0001F48B\u200D\U0001F469\U0001F3FF", COUNT_COMPACT), ==, 4); // Fitzpatrick after is an extra

  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\u200D\U0001F9B0", COUNT_COMPACT), ==, 2); // Yellow readhead man
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\U0001F3FC\u200D\U0001F9B1", COUNT_COMPACT), ==, 2); // Medium skin tone curly haired woman
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F9D1\U0001F3FE\u200D\U0001F9B3", COUNT_COMPACT), ==, 2); // Medium-dark skin tone white haired person
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F469\U0001F3FF\u200D\U0001F9B2", COUNT_COMPACT), ==, 2); // Dark skin tone bald woman

  g_assert_cmpint (tl_count_weighted_characters ("\U0001F471\U0001F3FB\u200D\u2640\uFE0F", COUNT_COMPACT), ==, 2); // Light skin person plus female symbol
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F471\U0001F3FC\u200D\u2642\uFE0F", COUNT_COMPACT), ==, 2); // Medium-light skin person plus male symbol
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F64D\U0001F3FF\u200D\u2642\uFE0F", COUNT_COMPACT), ==, 2); // Dark skin tone frowning plus male symbol
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F64E\u200D\u2640\uFE0F", COUNT_COMPACT), ==, 2); // Pouting person plus female symbol

  g_assert_cmpint (tl_count_weighted_characters ("\U0001F46F\u200D\u2642\uFE0F", COUNT_COMPACT), ==, 2); // Men with bunny ears
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F46F\U0001F3FC\u200D\u2642\uFE0F", COUNT_COMPACT), ==, 7); // Fitzpatrick breaks it
}

static void
cawbird_bug_114 (void)
{
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F3F3", COUNT_COMPACT), ==, 2); // White flag without VS16
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F3F3\uFE0F", COUNT_COMPACT), ==, 2); // White flag with VS16
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F3F3\u200D\U0001F308", COUNT_COMPACT), ==, 2); // Rainbow flag without VS16
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F3F3\uFE0F\u200D\U0001F308", COUNT_COMPACT), ==, 2); // Rainbow flag with VS16
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F3F3\uFE0F\u200D\u26A7\uFE0F", COUNT_COMPACT), ==, 2); // Transgender flag
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F3F4\u200D\u2620\uFE0F", COUNT_COMPACT), ==, 2); // Pirate flag
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468\U0001F3FF", COUNT_COMPACT), ==, 2); // Man with dark skin
  g_assert_cmpint (tl_count_weighted_characters ("\U0001F468", COUNT_COMPACT), ==, 2); // Default yellow man
}

int
main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_assert (GTLDS[G_N_ELEMENTS (GTLDS) - 1].length >= CCTLDS[G_N_ELEMENTS (CCTLDS) - 1].length);

#ifdef LIBTL_DEBUG
  g_setenv ("G_MESSAGES_DEBUG", "libtl", TRUE);
#endif

  g_test_add_func ("/length/empty", empty);
  g_test_add_func ("/length/nonempty", nonempty);
  g_test_add_func ("/length/basic-links", basic_links);
  g_test_add_func ("/length/advanced-links", advanced_links);
  g_test_add_func ("/length/utf8", utf8);
  g_test_add_func ("/length/validate", validate);
  g_test_add_func ("/length/emoji", emoji);
  g_test_add_func ("/length/cawbird-bug114", cawbird_bug_114);

  return g_test_run ();
}
