# ----------------------------
# MATHWIKI - makefile options
#
# The app code lives in ../engine/main.c and is shared with the other wikis;
# only src/content.c (generated from content/) differs between them.
# ----------------------------

NAME        = MATHWIKI
ICON        = icon.png
DESCRIPTION = "Math reference wiki"
COMPRESSED  = YES
ARCHIVED    = YES

CFLAGS = -Wall -Wextra -Oz

EXTRA_CSOURCES = ../engine/main.c

# ----------------------------

include $(shell cedev-config --makefile)
