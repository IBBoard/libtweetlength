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

static void
empty (void)
{
  gsize n_entities = 15;
  gsize length = 10;
  TlEntity *entities;

  entities = tl_extract_entities ("", &n_entities, &length);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_cmpint (length, ==, 0);
  g_assert_null (entities);

  g_free (entities);
}

static void
mentions (void)
{
  gsize text_length;
  gsize n_entities;
  TlEntity *entities;

  entities = tl_extract_entities ("@foobar", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_cmpint (text_length, ==, 7);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 7);
  g_free (entities);

  entities = tl_extract_entities ("@_foobar", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_cmpint (text_length, ==, 8);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("@foobar?", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_cmpint (text_length, ==, 8);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 7);
  g_free (entities);

  entities = tl_extract_entities ("@foobar-bla", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_cmpint (text_length, ==, 11);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 7);
  g_free (entities);

  entities = tl_extract_entities ("aaa @foobar-bla", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_cmpint (text_length, ==, 15);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 4);
  g_assert_cmpint (entities[0].length_in_characters, ==, 7);
  g_free (entities);

  entities = tl_extract_entities ("@12345", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 6);
  g_free (entities);

  // According to [1], this should be a mention, but twitter.com disagrees...
  entities = tl_extract_entities ("の@usernameに到着を待っている", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("abc@foo", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("Current Status: @_@ (cc: @username)", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 25);
  g_assert_cmpint (entities[0].length_in_characters, ==, 9);
  g_free (entities);

  entities = tl_extract_entities ("@username email me @test@example.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 9);
  g_free (entities);

  entities = tl_extract_entities ("@baedert!", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("@baedert~", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("@baedert-", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("~@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities (",@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities (";@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities (":@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("`@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("/@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("@🎇", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("@bädert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("á@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("aä@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 2);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("áá@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 2);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("áaá@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 3);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("aáa@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  // Special rules apply to mention characters, even inside text tokens
  entities = tl_extract_entities ("@báedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("áfoo@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("fooá@baedert", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].start_character_index, ==, 4);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("Is it @baedert, I wonder?", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[0].length_in_characters, ==, 8);
  g_free (entities);

  entities = tl_extract_entities ("_@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("123@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("abc@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("!@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("$@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("&@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("#@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("*@baedert", &n_entities, &text_length);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  // [1] https://github.com/twitter/twitter-text/blob/master/conformance/validate.yml
}

static void
valid_hashtags (void)
{
  const char * const hashtags[] = {
    "#foobar",
    ",#foobar",
    "{#foobar",
    "}#foobar",
    "<#foobar",
    ">#foobar",
    ";#foobar",
    ":#foobar",
    "?#foobar",
    "\\#foobar",
    "/#foobar",
    "@#foobar",
    "##foobar",
    "`#foobar",
    "*#foobar",
    "#123bar",
    "#foo123",
    "#fo12ar",
    "#ашок",
    "#트위터",
    "#__abc__",
  };
  gsize text_length;
  gsize n_entities;
  TlEntity *entities;
  guint i;

  for (i = 0; i < G_N_ELEMENTS (hashtags); i ++)
    {
      const char *text = hashtags[i];

      entities = tl_extract_entities (text, &n_entities, &text_length);
      g_assert_cmpint (n_entities, ==, 1);
      g_assert_nonnull (entities);
      g_assert_cmpint (entities[0].type, ==, TL_ENT_HASHTAG);
      g_free (entities);
    }
}

static void
invalid_hashtags (void)
{
  const char * const hashtags[] = {
    "&#foobar",
    "_#foobar",
    "#123",
    "#,",
    "#_",
    "foo#bar",
  };
  gsize text_length;
  gsize n_entities;
  TlEntity *entities;
  guint i;

  for (i = 0; i < G_N_ELEMENTS (hashtags); i ++)
    {
      const char *text = hashtags[i];

      entities = tl_extract_entities (text, &n_entities, &text_length);
      g_assert_cmpint (n_entities, ==, 0);
      g_assert_null (entities);
      g_free (entities);
    }
}

static void
hashtags (void)
{
  gsize text_length;
  gsize n_entities;
  TlEntity *entities;

  entities = tl_extract_entities ("What is this #shit, I wonder?", &n_entities, &text_length);
  g_assert_nonnull (entities);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 5);
  g_free (entities);
}

static void
links (void)
{
  gsize n_entities;
  gsize text_length;
  TlEntity *entities;

  entities = tl_extract_entities ("twitter.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 11);
  g_free (entities);

  // Numbers at the start of domains are also valid - RFC 952 said no but RFC 1123 said yes
  // https://en.wikipedia.org/wiki/Hostname#Restrictions_on_valid_hostnames
  entities = tl_extract_entities ("000example.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  // That also allows completely numeric domains (e.g. "123.[many-tlds]" is already registered)
  entities = tl_extract_entities ("123.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 7);
  g_free (entities);

  // But make sure that we don't allow numeric TLD
  entities = tl_extract_entities ("http://example.000", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("twitter.c", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("http://", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com::8080/foo.html", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foo /twitter.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("foo.商城", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 6);
  g_free (entities);

  entities = tl_extract_entities ("foo.de http://foo.de", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 2);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[1].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[1].start_character_index, ==, 7);
  g_free (entities);

  entities = tl_extract_entities ("http://foobar.co.uk", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 19);
  g_free (entities);

  entities = tl_extract_entities ("http://example.org.uk", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 21);
  g_free (entities);

  // And just to make sure that we're not special-casing TLD-like entries:
  // 1) France normally just has "….fr" but also allows ".com.fr"
  entities = tl_extract_entities ("http://example.com.fr", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 21);
  g_free (entities);

  // 2) Some Brits buy ".uk.com" because the .co.uk and .com are taken (or something)
  entities = tl_extract_entities ("http://example.uk.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 21);
  g_free (entities);

  entities = tl_extract_entities ("foobar.uk", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 9);
  g_free (entities);

  // .co is a "special tld"
  entities = tl_extract_entities ("foobar.co", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 9);
  g_free (entities);

  entities = tl_extract_entities ("http://-foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("http://foobar-.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("http://foobar.c_o_m", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("http://foo_bar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("http://trailingunderscore_.foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("http://_leadingunderscore.foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("http://www.bestbuy.com/site/Currie+Technologies+-+Ezip+400+Scooter/9885188.p?id=1218189013070&skuId=9885188", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (text_length, ==, 23);
  g_free (entities);

  entities = tl_extract_entities ("http://www.foo.org.za/foo/bar/688.1", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (text_length, ==, 23);
  g_free (entities);

  entities = tl_extract_entities ("http://www.foo.com/bar-path/some.stm?param1=foo;param2=P1|0||P2|0", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (text_length, ==, 23);
  g_free (entities);

  entities = tl_extract_entities ("http://www.cp.sc.edu/events/65", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (text_length, ==, 23);
  g_free (entities);

  entities = tl_extract_entities ("I really like http://t.co/pbY2NfTZ's website", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 14);
  g_assert_cmpint (entities[0].length_in_characters, ==, 20);
  g_free (entities);

  entities = tl_extract_entities ("Trailing http://t.co/pbY2NfTZ- hyphen", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].start_character_index, ==, 9);
  g_assert_cmpint (entities[0].length_in_characters, ==, 21);
  g_free (entities);

  // The it.so ones are not valid links because .aa is not an assigned ccTLD
  entities = tl_extract_entities ("http://twitter.com\nhttp://example.com\nhttp://example.com/path\nexample.com/path\nit.aa\nit.aa/abcde", &n_entities, &text_length);
  g_assert_cmpint (n_entities, ==, 4);
  g_assert_nonnull (entities);
  g_free (entities);

  entities = tl_extract_entities ("$http://twitter.com $twitter.com $http://t.co/abcde $t.co/abcde $t.co $TVI.CA $RBS.CA", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("john.doe.gov@gmail.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 0);
  g_assert_null (entities);
  g_free (entities);

  entities = tl_extract_entities ("http://www.foo.com?referer=https://t.co/abcde", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_free (entities);

  // We disallow dashes at the start and end of the domain, but we need to allow
  // it in the middle of course.
  entities = tl_extract_entities ("https://foo-bar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_free (entities);

  // We do try to find the last domain (e.g. in foo.com.com.com), but we stop doing
  // that once we find a / or ?
  entities = tl_extract_entities ("https://foobar.com?b=.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].length_in_characters, ==, 25);
  g_free (entities);

  // Looks weird but still a link
  entities = tl_extract_entities ("https://foobar.com/b=.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].length_in_characters, ==, 25);
  g_free (entities);

  entities = tl_extract_entities ("'foobar.com'", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("\"foobar.com\"", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com|", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("|foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("&foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com&", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("^foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com^", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("%foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com%", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);


  entities = tl_extract_entities ("+foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com+", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("\\foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com\\", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("[foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com[", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("]foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com]", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("`foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com`", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("~foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com~", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("!foobar.com", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 1);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com!", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b,", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b!", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b`", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b~", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b/", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 15);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b^", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b\\", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b%", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b|", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 14);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com?a=b|b=c", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 18);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com/&", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 11);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com!", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com_", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 10);
  g_free (entities);

  entities = tl_extract_entities ("_foobar.com", &n_entities, NULL);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities ("foobar.com//", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_nonnull (entities);
  g_assert (entities[0].type == TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 12);
  g_free (entities);
}

static void
combined (void)
{
  gsize n_entities;
  TlEntity *entities;

  entities = tl_extract_entities ("twitter.com abc #foo def @zomg", &n_entities, NULL);
  g_assert_cmpint (n_entities, ==, 3);
  g_assert_nonnull (entities);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_LINK);
  g_assert_cmpint (entities[0].start_character_index, ==, 0);
  g_assert_cmpint (entities[0].length_in_characters, ==, 11);
  g_assert_cmpint (entities[1].type, ==, TL_ENT_HASHTAG);
  g_assert_cmpint (entities[1].start_character_index, ==, 16);
  g_assert_cmpint (entities[1].length_in_characters, ==, 4);
  g_assert_cmpint (entities[2].type, ==, TL_ENT_MENTION);
  g_assert_cmpint (entities[2].start_character_index, ==, 25);
  g_assert_cmpint (entities[2].length_in_characters, ==, 5);
  g_free (entities);
}

static void
link_conformance1 (void)
{
  // [1] https://github.com/twitter/twitter-text/blob/master/conformance/tlds.yml
  const char * const containing_urls[] = {
//    "https://twitter.澳門",   Listed in [1], but the tld is not in their list of tlds?
//    "https://twitter.ഭാരതം",    Same
//    "https://twitter.ಭಾರತ",    Same
//    "https://twitter.ଭାରତ",    Same
//    "https://twitter.ভাৰত",    Same
    "https://twitter.한국",
    "https://twitter.香港",
    "https://twitter.新加坡",
    "https://twitter.台灣",
    "https://twitter.台湾",
    "https://twitter.中国",
    "https://twitter.გე",
    "https://twitter.ไทย",
    "https://twitter.ලංකා",
    "https://twitter.భారత్",
    "https://twitter.சிங்கப்பூர்",
    "https://twitter.இலங்கை",
    "https://twitter.இந்தியா",
    "https://twitter.ભારત",
    "https://twitter.ਭਾਰਤ",
    "https://twitter.বাংলা",
    // I'm stopping here since the .yml file is once again wrong all the time.
  };
  guint i;


  for (i = 0; i < G_N_ELEMENTS (containing_urls); i ++) {
    gsize n_entities;
    TlEntity *entities = NULL;

    entities = tl_extract_entities (containing_urls[i], &n_entities, NULL);
    g_assert_nonnull (entities);
    g_assert_cmpint (n_entities, ==, 1);
    g_assert_cmpint (entities[0].start_character_index, ==, 0);
    g_free (entities);
  }
}

static void
and_text (void)
{
  gsize n_entities;
  TlEntity *entities;

  entities = tl_extract_entities_and_text ("", &n_entities, NULL);
  g_assert_null (entities);
  g_assert_cmpint (n_entities, ==, 0);
  g_free (entities);

  entities = tl_extract_entities_and_text ("fobar @corebi ", &n_entities, NULL);
  g_assert_nonnull (entities);
  g_assert_cmpint (n_entities, ==, 2);
  g_free (entities);

  entities = tl_extract_entities_and_text ("fobar @corebi \n\n\n\n\n\n\n \n\n\n\n\n", &n_entities, NULL);
  g_assert_nonnull (entities);
  g_assert_cmpint (n_entities, ==, 2);
  g_free (entities);


  entities = tl_extract_entities_and_text ("asduiehf aiosufsifui hfuis0", &n_entities, NULL);
  g_assert_nonnull (entities);
  g_free (entities);

  entities = tl_extract_entities_and_text ("á", &n_entities, NULL);
  g_assert_nonnull (entities);
  g_assert_cmpint (n_entities, ==, 1);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_TEXT);
  g_assert_cmpint (entities[0].length_in_characters, ==, 1);
  g_free (entities);


  entities = tl_extract_entities_and_text ("á ó ö", &n_entities, NULL);
  g_assert_nonnull (entities);
  g_assert_cmpint (n_entities, ==, 3);
  g_assert_cmpint (entities[0].type, ==, TL_ENT_TEXT);
  g_assert_cmpint (entities[0].length_in_characters, ==, 1);
  g_assert_cmpint (entities[1].type, ==, TL_ENT_TEXT);
  g_assert_cmpint (entities[1].length_in_characters, ==, 1);
  g_assert_cmpint (entities[2].type, ==, TL_ENT_TEXT);
  g_assert_cmpint (entities[2].length_in_characters, ==, 1);
  g_free (entities);
}

int
main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);

#ifdef LIBTL_DEBUG
  g_setenv ("G_MESSAGES_DEBUG", "libtl", TRUE);
#endif

  g_test_add_func ("/entities/empty", empty);
  g_test_add_func ("/entities/mentions", mentions);
  g_test_add_func ("/entities/hashtags", hashtags);
  g_test_add_func ("/entities/valid-hashtags", valid_hashtags);
  g_test_add_func ("/entities/invalid-hashtags", invalid_hashtags);
  g_test_add_func ("/entities/links", links);
  g_test_add_func ("/entities/combined", combined);
  g_test_add_func ("/entities/link-conformance1", link_conformance1);
  g_test_add_func ("/entities/and-text", and_text);

  return g_test_run ();
}
