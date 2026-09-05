# ----------------------------
# MATHWIKI - makefile options
# ----------------------------

NAME        = MATHWIKI
ICON        = icon.png
DESCRIPTION = "Math reference wiki"
COMPRESSED  = YES
ARCHIVED    = YES

CFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
