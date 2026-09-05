# TIwiki — offline reference wikis for the TI-84 Plus CE

Wikipedia-style reference apps that run natively on the TI-84 Plus CE /
CE Python. Browse by category, list every article A–Z, or search titles by
typing on the ALPHA key labels — all offline, in flash.

| App | File | Articles | Size |
|---|---|---|---|
| **MATHWIKI** | `mathwiki/bin/MATHWIKI.8xp` | 93 across 13 categories | 35 KB |
| **PHYSWIKI** | `physicswiki/bin/PHYSWIKI.8xp` | 89 across 14 categories | 42 KB |

Both are built from **one shared engine** (`engine/main.c`); only the content
differs. Written in C with the
[CE C/C++ toolchain](https://ce-programming.github.io/toolchain/), so these are
real native programs, not TI-BASIC — which is what makes the full-screen text
rendering, instant search and smooth scrolling possible.

```
+--------------------------------------+
|PHYSWIKI                        1/14  |
|>Kinematics                      6    |
| Forces and Newton's Laws        8    |
| Circular Motion and Gravity     5    |
| Work and Energy                 6    |
| Momentum                        5    |
| ...                                  |
|2ND open  Y= search  ZOOM all  CLEAR  |
+--------------------------------------+
```

---

## Installing on the calculator

You need the app(s) you want plus **`clibs.8xg`** — the shared C libraries
(graphx, keypadc) every CE toolchain program links against. Send it once; both
wikis and any other CE program share it.

1. Connect by USB and open **TI Connect CE**.
2. Drag `clibs.8xg` and whichever `.8xp` files you want into *Calculator
   Explorer*. `clibs.8xg` is a group and unpacks into several AppVars
   automatically — click **Replace** if it asks; the libraries are versioned and
   backward compatible, so the newest set is always the right one to keep.
3. Launch through your jailbreak's loader:
   - **arTIfiCE** (OS 5.5+): run the exploit (e.g. via Cabri™ Jr.), then pick
     the program.
   - **A shell such as Cesium**: press enter on it in the program list.
   - **OS 5.2 and older**: `Asm(prgmMATHWIKI)` from the home screen.

**Notes**

- Both programs are flagged archived, so they live in flash and survive a RAM
  clear. Keep the `clibs` AppVars archived too.
- They ship zx7-compressed and expand in RAM to run: MATHWIKI to ~63 KB,
  PHYSWIKI to ~78 KB, against roughly 150 KB of user RAM. Both fit with room to
  spare, but `2nd + (MEM) > 2` will tell you what you have.
- Neither writes anything to the calculator — they only read their own flash,
  so they cannot disturb your variables or programs.

---

## Controls

**In any list**

| Key | Action |
|---|---|
| `UP` / `DOWN` | move the selection (wraps, auto-repeats when held) |
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
names (so `INTEGRAL` finds everything in the Integrals category).

---

## What's in them

### MATHWIKI — 93 articles

| Category | N | Covers |
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

### PHYSWIKI — 89 articles

| Category | N | Covers |
|---|---|---|
| Kinematics | 6 | motion basics, kinematic equations, free fall, motion graphs, projectiles, relative motion |
| Forces and Newton's Laws | 8 | the three laws, free-body diagrams, weight & normal force, friction, inclines, tension & pulleys, springs, drag & terminal velocity |
| Circular Motion and Gravity | 5 | uniform circular motion, centripetal force, vertical circles & banking, gravitation, orbits & Kepler |
| Work and Energy | 6 | work, kinetic energy, potential energy, conservation, power, springs |
| Momentum | 5 | impulse, conservation, collisions, ballistic pendulum, center of mass |
| Rotational Motion | 7 | angular kinematics, torque, moment of inertia, dynamics, rotational energy, angular momentum, statics |
| Oscillations and Waves | 7 | SHM, spring & pendulum, wave basics, superposition, standing waves, sound, Doppler |
| Fluids and Thermodynamics | 8 | pressure, buoyancy, fluid flow, thermal expansion, calorimetry, ideal gas, the laws, engines & entropy |
| Electrostatics | 6 | charge, Coulomb's law, electric field, Gauss's law, potential, capacitors |
| Circuits | 6 | current & resistance, Ohm's law & power, series/parallel, Kirchhoff, real batteries & meters, RC circuits |
| Magnetism | 5 | force on a charge, force on a wire, sources of field, induction & Faraday, inductance & AC |
| Optics and Light | 7 | EM waves, reflection & mirrors, refraction, total internal reflection, lenses, mirror/lens equations, interference & diffraction |
| Modern Physics | 6 | photoelectric effect, duality, atomic models & spectra, nuclear physics, radioactive decay, relativity |
| Reference and Constants | 7 | constants, SI units, conversions, dimensional analysis, vectors, problem solving, in-app help |

---

## Repo layout

```
engine/
  main.c              the entire app: UI, navigation, search, rendering.
                      Content-agnostic; shared by every wiki.
  tools/
    build_content.py  <wiki>/content/*.txt -> <wiki>/src/content.{c,h}
    make_icon.py      writes <wiki>/icon.png
    hostsim/          desktop simulator (see below)

mathwiki/
  content/*.txt       the wiki, as editable plain text (the source of truth)
  src/content.c/.h    GENERATED - do not edit by hand
  makefile            NAME=MATHWIKI, pulls in ../engine/main.c
  icon.png
  bin/MATHWIKI.8xp    the built program

physicswiki/          same shape, NAME=PHYSWIKI

clibs.8xg             C libraries to send to the calculator
```

Each wiki's makefile pulls the shared engine in with
`EXTRA_CSOURCES = ../engine/main.c`, and the toolchain puts `<wiki>/src` on the
include path, so `main.c` picks up that wiki's generated `content.h`. The app
title on the main menu comes from the `@WIKI` directive in the content, via a
generated `WIKI_TITLE` define — the engine has no wiki-specific code in it.

---

## Building

**Requirements:** the CE C/C++ toolchain (v15.0 used here). On macOS, download
`CEdev-macOS-arm.dmg` (or `-intel`) from the
[toolchain releases](https://github.com/CE-Programming/toolchain/releases),
mount it, and copy the `CEdev` folder to `~/CEdev`.

```sh
export PATH="$HOME/CEdev/bin:$PATH"

python3 engine/tools/build_content.py            # rebuild every wiki's content
python3 engine/tools/build_content.py mathwiki   # or just one

cd mathwiki && make          # -> bin/MATHWIKI.8xp
cd physicswiki && make       # -> bin/PHYSWIKI.8xp
```

`build_content.py` with no arguments compiles every directory that has a
`content/` folder, so adding a third wiki needs no changes to it.

---

## Adding your own articles

The content is plain text, not C — that's the point. Add your teacher's
formulas, a unit you're studying, whatever.

```
@WIKI PHYSWIKI                     (optional, sets the main-menu title)
@CATEGORY Kinematics
@ARTICLE Projectile Motion
# A heading, shown in the accent colour with an underline
Ordinary prose. Consecutive lines are joined and re-wrapped to fit the
screen, so it does not matter where your line breaks fall.

  Two leading spaces = a formula. Kept exactly as typed, shown in orange
  on a grey band. Max 37 characters, never re-wrapped.

- A bullet. Wrapped with a hanging indent; following plain lines belong
  to the same bullet.
```

- Blank lines separate paragraphs; runs of blanks collapse to one.
- Headings and formula lines must be **≤ 37 characters** — the build reports the
  file and line number of anything too long and exits non-zero.
- File name order (`01_`, `02_`, …) sets category order in the menu.
- Articles keep their authored order within a category; the A–Z list and search
  are sorted separately at build time.

### Starting a whole new wiki

```sh
mkdir -p chemwiki/content
# write chemwiki/content/*.txt, starting with "@WIKI CHEMWIKI"
sed 's/PHYSWIKI/CHEMWIKI/g' physicswiki/makefile > chemwiki/makefile
# then edit DESCRIPTION in chemwiki/makefile
python3 engine/tools/build_content.py chemwiki
python3 engine/tools/make_icon.py chemwiki
cd chemwiki && make
```

(Add a colour theme for it in `make_icon.py` if you want a distinct icon;
otherwise it falls back to the MATHWIKI blue.)

---

## The desktop simulator

`engine/tools/hostsim/` compiles **the real `engine/main.c`** against stub
`graphx.h` / `keypadc.h` headers so a wiki runs on a Mac/Linux box. Text drawn
by the app lands in an ASCII grid printed once per frame, and keypresses come
from a script on the command line. Content and UI changes can be checked without
a calculator in hand.

```sh
cd engine/tools/hostsim
./build.sh physicswiki              # -> ./sim-physicswiki
./build.sh physicswiki --asan       # -> ./sim-physicswiki-asan

./sim-physicswiki ""                     # the main menu
./sim-physicswiki "2nd down 2nd" --frame 4
./sim-physicswiki "y= d o p enter"       # search for "dop" and open the hit
```

Key tokens: `up down left right enter 2nd clear del mode y= window zoom trace
graph`, a single letter `a`–`z`, or `_` for a space. `--frame N` prints only
frame N. The app quits when the script runs out of keys.

---

## Notes and decisions

**Why native C and not TI-BASIC or Python.** A `.8xp` can be either a BASIC
program or a native eZ80 one. BASIC would have made a 45–60 KB text corpus
painful (strings are slow, program size is awkward, there is no real
full-screen text control) and CE Python stores scripts as `.8xv` AppVars and
only draws in its shell. Since the calculator is jailbroken, native C was the
right call.

**One engine, many wikis.** `engine/main.c` contains no subject-specific code.
Everything it displays — including the app's own title — comes from the
generated `content.c`/`content.h` of whichever wiki is being built. Adding a
subject means writing text files and a three-line makefile, and a UI fix lands
in every wiki at once.

**Content is data, not code.** Wrapping, reflow, style tagging and the
37-column limit are all enforced at build time, so the calculator never does
text layout at runtime — it just prints pre-wrapped lines.

**One string per article, indexed on open.** Each article body is a single
`\n`-separated string with a one-byte style prefix per line (`0` body,
`1` heading, `2` formula, `3` bullet). One pointer per line would have cost
~3 bytes × ~2,200 lines ≈ 6.7 KB of extra flash per wiki. The app scans for
line starts once when you open an article and scrolling is then an array index.

**Compressed, single file.** Uncompressed, either program exceeds the 64 KB
limit on a single calculator variable and the toolchain would split it into a
loader plus AppVars. With `COMPRESSED = YES` each is one `.8xp` that expands
into RAM at launch.

**Explicit palette.** The engine defines all 15 of its colours with
`gfx_RGBTo1555` at startup rather than relying on the default palette's index
meanings, and reserves index 0 as the text-transparency key.

**ASCII-only glyphs.** The UI uses plain ASCII (`>` for the selection marker,
`UP/DN`, `L/R` in hints) rather than the TI font's control-code arrow glyphs,
whose positions are not guaranteed across font variants.

### Testing

- Every article in both wikis was opened, scrolled to the end, paged back and
  forth and advanced through — 877 frames for MATHWIKI, 844 for PHYSWIKI — plus
  search overflow, backspace-past-empty and list wraparound, all under
  AddressSanitizer and UndefinedBehaviorSanitizer in the host simulator. Clean.
- The content build fails loudly on any heading or formula wider than 37
  columns, so layout regressions can't reach a release.
- The `.8xp` files were built and their RAM footprints checked against the map
  files (63 KB and 78 KB against ~150 KB available), but they have **not** been
  run on real hardware or in an emulator (CEmu needs a ROM dump from your own
  calculator). If something looks wrong on device, the simulator is the fastest
  place to reproduce it.
