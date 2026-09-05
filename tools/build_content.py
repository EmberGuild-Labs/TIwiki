#!/usr/bin/env python3
"""Compile the plain-text wiki sources in content/ into C tables in src/.

Source format (see content/*.txt):

    @CATEGORY Algebra
    @ARTICLE Quadratic Formula
    # Heading line
    Prose line, wrapped automatically to WIDTH columns.
    - Bullet, wrapped with a hanging indent.
      Two-space indent = formula, kept verbatim (never wrapped).

Each rendered line is stored with a one-byte style prefix so the calculator
can colour it without re-parsing anything:

    '0' body   '1' heading   '2' formula   '3' bullet

An article body is emitted as a single "\n"-separated string; the app indexes
the lines at runtime, which saves ~3 bytes of pointer per line in flash.
"""

import os
import re
import sys
import textwrap

WIDTH = 37  # characters that fit beside the scrollbar at 8px monospace

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONTENT_DIR = os.path.join(ROOT, "content")
SRC_DIR = os.path.join(ROOT, "src")

STYLE_BODY, STYLE_HEAD, STYLE_FORM, STYLE_BULLET = "0", "1", "2", "3"


class Article:
    def __init__(self, title):
        self.title = title
        self.lines = []


class Category:
    def __init__(self, title):
        self.title = title
        self.articles = []


class Block:
    """Runs of prose (or one bullet) are gathered here, then reflowed as a
    unit, so hard line breaks in the source do not survive into the app."""

    def __init__(self):
        self.kind = None      # None, "prose" or "bullet"
        self.words = []

    def add(self, kind, text):
        self.kind = kind
        self.words.append(text)

    def flush(self, article):
        if self.kind is None:
            return
        text = " ".join(self.words)
        if self.kind == "bullet":
            wrapped = textwrap.wrap(text, WIDTH - 2) or [""]
            article.lines.append(STYLE_BULLET + wrapped[0])
            article.lines += [STYLE_BULLET + "\x01" + w for w in wrapped[1:]]
        else:
            article.lines += [STYLE_BODY + w
                              for w in (textwrap.wrap(text, WIDTH) or [""])]
        self.kind = None
        self.words = []


def parse_file(path, categories, problems):
    category = None
    article = None
    block = Block()

    for lineno, raw in enumerate(open(path, encoding="utf-8"), 1):
        line = raw.rstrip("\n").rstrip()
        where = "%s:%d" % (os.path.basename(path), lineno)

        if line.startswith("@CATEGORY "):
            if article:
                block.flush(article)
            category = Category(line[len("@CATEGORY "):].strip())
            categories.append(category)
            article = None
            continue
        if line.startswith("@ARTICLE "):
            if article:
                block.flush(article)
            if category is None:
                problems.append("%s: @ARTICLE before any @CATEGORY" % where)
                continue
            article = Article(line[len("@ARTICLE "):].strip())
            category.articles.append(article)
            continue
        if line.startswith("@"):
            problems.append("%s: unknown directive %r" % (where, line.split()[0]))
            continue
        if article is None:
            if line:
                problems.append("%s: text outside of an @ARTICLE" % where)
            continue

        if not line.strip():                       # paragraph break
            block.flush(article)
            if article.lines and article.lines[-1] != STYLE_BODY:
                article.lines.append(STYLE_BODY)   # collapse runs of blanks
            continue

        if line.startswith("# "):                  # heading
            block.flush(article)
            text = line[2:].strip()
            if len(text) > WIDTH:
                problems.append("%s: heading is %d cols (max %d)"
                                % (where, len(text), WIDTH))
            article.lines.append(STYLE_HEAD + text)
            continue

        if line.startswith("  "):                  # formula: never reflowed
            block.flush(article)
            text = line[2:].rstrip()
            if len(text) > WIDTH:
                problems.append("%s: formula is %d cols (max %d)"
                                % (where, len(text), WIDTH))
            article.lines.append(STYLE_FORM + text)
            continue

        if line.startswith("- "):                  # new bullet
            block.flush(article)
            block.add("bullet", line[2:].strip())
            continue

        # Plain text continues whatever block is open (a bullet's wrapped
        # tail, or the current paragraph).
        block.add(block.kind or "prose", line.strip())

    if article:
        block.flush(article)
    return categories


def c_string(text):
    out = text.replace("\\", "\\\\").replace('"', '\\"')
    out = out.replace("\n", "\\n")
    # octal escape: fixed length, so a following digit cannot extend it
    out = out.replace("\x01", "\\001")
    return '"' + out + '"'


def c_ident(text):
    return re.sub(r"[^A-Za-z0-9]+", "_", text).strip("_").lower() or "x"


def emit(categories, problems):
    for cat in categories:
        for art in cat.articles:            # trim blank lines at both ends
            while art.lines and art.lines[0] == STYLE_BODY:
                art.lines.pop(0)
            while art.lines and art.lines[-1] == STYLE_BODY:
                art.lines.pop()

    total_articles = sum(len(c.articles) for c in categories)
    total_lines = sum(len(a.lines) for c in categories for a in c.articles)
    total_bytes = sum(len(l) + 1 for c in categories for a in c.articles for l in a.lines)

    # flat, title-sorted index used by the search screen
    flat = sorted(
        ((a.title, ci, ai)
         for ci, c in enumerate(categories)
         for ai, a in enumerate(c.articles)),
        key=lambda t: t[0].lower(),
    )

    out = []
    w = out.append
    w("/* Generated by tools/build_content.py -- do not edit by hand. */")
    w('#include "content.h"')
    w("")

    for ci, cat in enumerate(categories):
        for ai, art in enumerate(cat.articles):
            w("static const char body_%d_%d[] =" % (ci, ai))
            if art.lines:
                for i, line in enumerate(art.lines):
                    tail = ";" if i == len(art.lines) - 1 else ""
                    body = line + ("" if i == len(art.lines) - 1 else "\n")
                    w("    " + c_string(body) + tail)
            else:
                w('    "0";')
            w("")

    for ci, cat in enumerate(categories):
        w("static const article_t arts_%d[] = {" % ci)
        for ai, art in enumerate(cat.articles):
            w("    { %-34s body_%d_%d, %d }," %
              (c_string(art.title) + ",", ci, ai, len(art.lines)))
        w("};")
        w("")

    w("const category_t wiki_categories[] = {")
    for ci, cat in enumerate(categories):
        w("    { %-24s arts_%d, %d }," % (c_string(cat.title) + ",", ci, len(cat.articles)))
    w("};")
    w("")
    w("const uint8_t wiki_num_categories = %d;" % len(categories))
    w("")
    w("const artref_t wiki_index[] = {")
    for _, ci, ai in flat:
        w("    { %d, %d }," % (ci, ai))
    w("};")
    w("")
    w("const uint16_t wiki_index_size = %d;" % len(flat))
    w("")

    header = [
        "/* Generated by tools/build_content.py -- do not edit by hand. */",
        "#ifndef CONTENT_H",
        "#define CONTENT_H",
        "",
        "#include <stdint.h>",
        "",
        "/* First byte of every display line is its style: */",
        "#define STYLE_BODY    '0'",
        "#define STYLE_HEAD    '1'",
        "#define STYLE_FORM    '2'",
        "#define STYLE_BULLET  '3'",
        "#define BULLET_CONT   '\\x01'  /* wrapped continuation of a bullet */",
        "",
        "#define WIKI_COLS %d" % WIDTH,
        "#define WIKI_MAX_LINES %d  /* longest article, for the runtime line index */"
        % max([len(a.lines) for c in categories for a in c.articles] + [1]),
        "",
        "typedef struct {",
        "    const char *title;",
        "    const char *body;      /* newline-separated, style-prefixed lines */",
        "    uint16_t    num_lines;",
        "} article_t;",
        "",
        "typedef struct {",
        "    const char     *title;",
        "    const article_t *articles;",
        "    uint8_t         num_articles;",
        "} category_t;",
        "",
        "typedef struct {",
        "    uint8_t cat;",
        "    uint8_t art;",
        "} artref_t;",
        "",
        "extern const category_t wiki_categories[];",
        "extern const uint8_t    wiki_num_categories;",
        "extern const artref_t   wiki_index[];   /* all articles, sorted by title */",
        "extern const uint16_t   wiki_index_size;",
        "",
        "#endif",
        "",
    ]

    with open(os.path.join(SRC_DIR, "content.c"), "w") as f:
        f.write("\n".join(out))
    with open(os.path.join(SRC_DIR, "content.h"), "w") as f:
        f.write("\n".join(header))

    print("categories: %d   articles: %d   lines: %d   text: %.1f KB"
          % (len(categories), total_articles, total_lines, total_bytes / 1024.0))
    if problems:
        print("\n%d formatting problem(s):" % len(problems))
        for p in problems:
            print("  " + p)
        return 1
    return 0


def main():
    categories, problems = [], []
    files = sorted(f for f in os.listdir(CONTENT_DIR) if f.endswith(".txt"))
    if not files:
        sys.exit("no content/*.txt files found")
    for name in files:
        parse_file(os.path.join(CONTENT_DIR, name), categories, problems)
    sys.exit(emit(categories, problems))


if __name__ == "__main__":
    main()
