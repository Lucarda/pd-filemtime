# library name
lib.name = filemtime

class.sources = \
	filemtime.c \

datafiles = \
	filemtime-help.pd \
	CHANGELOG.txt \
	README.md \


# include Makefile.pdlibbuilder
# (for real-world projects see the "Project Management" section
# in tips-tricks.md)
PDLIBBUILDER_DIR=./pd-lib-builder
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder