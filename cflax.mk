
# ============================================================================ #
#
# CFLAX
#
# Some compiler flags to help catch bugs and enforce good coding practices.
# See https://github.com/darkfirezz/cflax
#
# ============================================================================ #

CFLAX := -Wall -Wextra -Wpedantic

# Warn if a switch statement does not cover all enum values
CFLAX += -Wswitch-enum
# Warn if a local variable shadows another variable
CFLAX += -Wshadow
# Warn about implicit conversions that may alter a value
CFLAX += -Wconversion
# Warn about missing braces in nested if statements, loops, and switches
CFLAX += -Wmissing-braces
# Check the format strings in printf-like functions
CFLAX += -Wformat
# Warn if a function is defined without a prototype
CFLAX += -Wmissing-prototypes
# Enforce explicit declaration of function parameters
CFLAX += -Wstrict-prototypes

