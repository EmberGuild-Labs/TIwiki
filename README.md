# MATHWIKI — an offline math wiki for the TI-84 Plus CE

A Wikipedia-style math reference that runs natively on the TI-84 Plus CE /
CE Python. Browse 93 articles across 13 categories, jump straight to any
article A–Z, or search titles by typing on the ALPHA key labels — all offline,
in flash, with no internet and no notes app.

Written in C with the [CE C/C++ toolchain](https://ce-programming.github.io/toolchain/),
so it is a real native program (`MATHWIKI.8xp`), not TI-BASIC. That is what
makes the fast full-screen text rendering, instant search and smooth scrolling
possible.

```
+--------------------------------------+
|MATHWIKI                         1/13 |
|>Forms of a Function            15    |
| Algebra                        11    |
| Geometry                        6    |
| Conic Sections                  5    |
| Trigonometry                   11    |
| ...                                  |
|2ND open  Y= search  ZOOM all  CLEAR  |
+--------------------------------------+
```

---

## What's in it

| Category | Articles | Covers |
|---|---|---|
| Forms of a Function | 15 | linear, quadratic, polynomial, rational, exponential, log, radical, absolute value, piecewise, trig, transformations, inverses, symmetry, composition, domain/range |
| Algebra | 11 | exponent & log rules, factoring, special products, completing the square, polynomial division, rational expressions, radicals, inequalities, systems, complex numbers |
| Geometry | 6 | area/perimeter, volume/surface area, triangles, circles, coordinate geometry, angles |
| Conic Sections | 5 | circle, parabola, ellipse, hyperbola, identifying a conic |
| Trigonometry | 11 | right triangle trig, unit circle, exact values, identities, sum/difference, double/half angle, laws of sines & cosines, graphs, inverse trig, solving, polar |
| Sequences and Series | 5 | arithmetic, geometric, sigma notation, convergence, binomial theorem |
| Vectors and Matrices | 5 | vectors, dot/cross product, matrix ops, determinants/inverses, solving systems |
| Limits and Continuity | 4 | limit basics, special limits, continuity, L'Hôpital |
| Derivatives | 7 | definition, rules, common derivatives, implicit, applications, motion/related rates, linearization |
| Integrals | 7 | antiderivatives, FTC, u-sub, by parts, area/volume, Riemann sums, applications |
| Statistics and Probability | 7 | descriptive stats, normal distribution, probability rules, counting, discrete distributions, regression, confidence & tests |
| TI-84 CE Tips | 5 | solvers, graphing tricks, nDeriv/fnInt, lists & stats, modes and error fixes |
| Reference Tables | 5 | constants, number sets, symbols, unit conversions, in-app help |

93 articles, ~2,200 lines, about 46 KB of text.

---

## Installing on the calculator

You need **two files** on the calculator:

| File | What it is |
|---|---|
| `bin/MATHWIKI.8xp` | the app itself (35 KB) |
| `clibs.8xg` | the shared C libraries (graphx, keypadc) that every CE toolchain program links against |

**Steps**

1. Connect the calculator by USB and open **TI Connect CE**.
2. Drag both `MATHWIKI.8xp` and `clibs.8xg` into the *Calculator Explorer* window.
   `clibs.8xg` is a group — it unpacks into several `AppVar`s automatically.
   You only ever need to send it once; other CE programs share it.
3. Launch it with whatever loader your jailbreak gives you:
   - **arTIfiCE** (OS 5.5+): run the exploit (e.g. through Cabri™ Jr.), then
     pick `MATHWIKI` from the program list.
   - **A shell such as Cesium**: `MATHWIKI` appears in the program list; press
     enter on it.
   - **OS 5.2 and older**, where the `Asm(` token still exists:
     `Asm(prgmMATHWIKI)` from the home screen.

**Notes**

- The program is flagged archived, so it lives in flash and survives a RAM
  clear. Keep `clibs` archived too.
- It ships zx7-compressed (35 KB file) and expands to roughly 67 KB in RAM when
  it runs, so leave a reasonable amount of free RAM. `2nd + (MEM) > 2` will show
  you what you have.
- Nothing is written to the calculator while the app runs — it only reads its
  own flash, so it cannot corrupt your variables or programs.

---

## Controls

**In any list**

| Key | Action |
|---|---|
| `UP` / `DOWN` | move the selection (wraps around, auto-repeats when held) |
| `LEFT` / `RIGHT` | page up / page down |
| `2ND` or `ENTER` | open |
| `CLEAR` / `DEL` | back (`CLEAR` on the main menu quits) |
| any letter | jump to the first entry starting with that letter |
| `Y=` | search |
| `ZOOM` | list every article A–Z |
| `MODE` | quit to the OS |

**Reading an article**

| Key | Action |
|---|---|
| `UP` / `DOWN` | scroll one line |
| `LEFT` / `RIGHT` | scroll one screen |
| `WINDOW` / `GRAPH` | jump to top / end |
| `TRACE` | next article in the category |
| `CLEAR` / `DEL` | back to the list |

**Searching**

Type using the letters *printed on the keys* — `MATH` is A, `APPS` is B, `SIN`
is E, and so on. You do **not** press `ALPHA` first; the app reads the raw
keypad. `0` types a space, `DEL` backspaces, `CLEAR` exits. Matching is
case-insensitive, matches anywhere inside a title, and also matches category
names (so typing `INTEGRAL` finds everything in the Integrals category).

---

## Adding your own articles

The wiki content is plain text in `content/`, not C. Editing it is the whole
point — add your teacher's formulas, a unit you're studying, whatever.

Format:

```
@CATEGORY Algebra
@ARTICLE Quadratic Formula
# A heading, shown in blue with an underline
Ordinary prose. Consecutive lines are joined and re-wrapped to fit the
screen, so you do not have to worry about where your line breaks fall.

  Two leading spaces = a formula. Kept exactly as typed, shown in orange
  on a grey band. Max 37 characters, never re-wrapped.

- A bullet. Wrapped with a hanging indent; following plain lines belong
  to the same bullet.
```

Rules of thumb:

- Blank lines separate paragraphs; runs of blanks collapse to one.
- Headings and formula lines must be **≤ 37 characters** — the build reports
  the file and line number of anything too long and refuses to finish.
- File name order (`01_`, `02_`, …) sets category order in the menu.
- Articles keep their authored order inside a category; the A–Z list and
  search are sorted separately at build time.

Then rebuild and re-send:

```sh
python3 tools/build_content.py     # content/*.txt -> src/content.{c,h}
make                               # -> bin/MATHWIKI.8xp
```

---

## Building from source

**Requirements:** the CE C/C++ toolchain (v15.0 was used here). On macOS,
download `CEdev-macOS-arm.dmg` (or `-intel`) from the
[toolchain releases](https://github.com/CE-Programming/toolchain/releases),
mount it, and copy the `CEdev` folder to `~/CEdev`.

```sh
export PATH="$HOME/CEdev/bin:$PATH"
python3 tools/build_content.py     # regenerate content tables (optional)
python3 tools/make_icon.py         # regenerate icon.png (optional)
make                               # -> bin/MATHWIKI.8xp
make clean
```

### Project layout

```
content/            the wiki, as editable plain text (the source of truth)
tools/
  build_content.py  content/*.txt  ->  src/content.c + src/content.h
  make_icon.py      writes the 16x16 icon.png
  hostsim/          desktop simulator, see below
src/
  main.c            the whole app: UI, navigation, search, rendering
  content.c/.h      GENERATED - do not edit by hand
makefile            CE toolchain makefile
clibs.8xg           C libraries to send to the calculator
bin/MATHWIKI.8xp    the built program
```

---

## The desktop simulator

`tools/hostsim/` compiles **the real `src/main.c`** against stub `graphx.h` /
`keypadc.h` headers so the app runs on a Mac/Linux box. Text drawn by the app
lands in an ASCII grid that is printed once per frame, and keypresses come from
a script on the command line. This means content and UI changes can be checked
without a calculator in hand.

```sh
cd tools/hostsim
cc -std=c11 -I. -I../../src -Dmain=wiki_main -c ../../src/main.c -o main_sim.o
cc -std=c11 -I. -c ../../src/content.c -o content_sim.o
cc -std=c11 -I. -I../../src -c sim.c -o sim.o
cc main_sim.o content_sim.o sim.o -o sim

./sim ""                       # the main menu
./sim "2nd down 2nd" --frame 4 # open a category, then an article
./sim "y= q u a d"             # search for "quad"
```

Key tokens: `up down left right enter 2nd clear del mode y= window zoom trace
graph`, a single letter `a`–`z`, or `_` for a space. `--frame N` prints only
frame N. The script quits the app when it runs out of keys.

---

## Notes and decisions

**Why native C and not TI-BASIC or Python.** A `.8xp` can be either a BASIC
program or a native eZ80 one. BASIC would have made a 46 KB text corpus painful
(strings are slow, program size is awkward, there is no real full-screen text
control) and CE Python stores scripts as `.8xv` AppVars and only draws in its
shell. Since the calculator is jailbroken, native C was the right call: full
320×240 control, instant search across all 93 titles, and the whole corpus in
flash.

**Content is data, not code.** The articles live in `content/*.txt` and are
compiled into C tables. Wrapping, hyphen-free reflow, style tagging and the
37-column limit are all enforced at build time, so the calculator never does
text layout at runtime — it just prints pre-wrapped lines. That also makes the
wiki easy to extend without touching any C.

**One string per article, indexed on open.** Each article body is a single
`\n`-separated string with a one-byte style prefix per line (`0` body,
`1` heading, `2` formula, `3` bullet). Storing one pointer per line instead
would have cost ~3 bytes × 2,235 lines ≈ 6.7 KB of extra flash. The app scans
for line starts once when you open an article (at most 47 lines) and scrolling
is then just an array index.

**Compressed, single file.** Uncompressed, the program is ~67 KB, which exceeds
the 64 KB limit on a single calculator variable — the toolchain would split it
into a loader plus two AppVars. With `COMPRESSED = YES` it is one 35 KB `.8xp`
that expands into RAM at launch. Fewer files to send, and it fits in flash
comfortably.

**Explicit palette.** The app defines all 15 of its colors with
`gfx_RGBTo1555` at startup rather than relying on the default palette's index
meanings, and reserves index 0 as the text-transparency key. Colors are then
guaranteed regardless of toolchain defaults.

**ASCII-only glyphs.** The UI uses plain ASCII (`>` for the selection marker,
`UP/DN`, `L/R` in hints) rather than the TI font's control-code arrow glyphs,
whose positions are not guaranteed across font variants.

### Testing

- Every article was opened, scrolled to the end, paged back and forth and
  advanced through, plus search overflow/backspace-past-empty and list
  wraparound — 877 frames — under AddressSanitizer and UndefinedBehaviorSanitizer
  in the host simulator. Clean.
- The content build fails loudly on any heading or formula wider than 37
  columns, so layout regressions can't sneak into a release.
- The final `.8xp` was built and its size verified, but it has **not** been run
  on real hardware or in an emulator (CEmu needs a ROM dump from your own
  calculator). If anything looks off on device, the simulator is the fastest
  place to reproduce it.
