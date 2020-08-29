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
  g_assert_cmpint (tl_count_weighted_characters ("a"), ==, 1);
  g_assert_cmpint (tl_count_weighted_characters ("ä"), ==, 1);
  g_assert_cmpint (tl_count_weighted_characters ("火"), ==, 2);
  g_assert_cmpint (tl_count_weighted_characters ("a 😭 a"), ==, 6);
  g_assert_cmpint (tl_count_weighted_characters ("https://twitter.com/"), ==, 23);
  g_assert_cmpint (tl_count_weighted_characters ("a https://twitter.com/ 火"), ==, 28);
  g_assert_cmpint (tl_count_weighted_characters ("I am a Tweet"), <, 20);
  g_assert_cmpint (tl_count_weighted_characters ("A lie gets halfway around the world before the truth has a chance to get its pants on. Winston Churchill (1874-1965) http://bit.ly/dJpywL"), ==, 140);
  g_assert_cmpint (tl_count_weighted_characters ("A lié géts halfway arøünd thé wørld béføré thé truth has a chance tø get its pants øn. Winston Churchill (1874-1965) http://bit.ly/dJpywL"), ==, 140);
  g_assert_cmpint (tl_count_weighted_characters ("のののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののののの"), ==, 280);
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

  return g_test_run ();
}
