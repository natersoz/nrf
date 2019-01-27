###
# version_git.mak
# Shell out to git and retrieve ght hash and tag (TBD) information
#
# Sets the following symbols:
#	GIT_HASH	The 8 left-most characters (4 bytes) of the git hash.
#	GIT_TAG		The string of the most recent git tag.
#			Not yet implemented
###

# Use sed to convert the git hash from digits to bytes.
# Example: "97887a6c" -> "0x97,0x88,0x7a,0x6c,"
GIT_HASH := $(shell git rev-parse --short=8 HEAD | sed -e 's/\([0-9A-Fa-f][0-9A-Fa-f]\)/0x\1,/g')

# Placeholder for tag based versions:
GIT_TAG := "\"0.0.0\""

# TBD: When tags are used for versioning:
# GIT_TAG  = $shell(git describe --tags --abbrev=0)
