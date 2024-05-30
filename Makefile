
SUBDIRS = examples reading/programs

.PHONY: all clean $(SUBDIRS)

INCLUDE_DIRS = .
INCLUDE = $(foreach i, $(INCLUDE_DIRS),-I$(CURDIR)/$i)
INCLUDE_HEADERS = $(foreach dir, $(abspath $(INCLUDE_DIRS)), $(wildcard $(dir)/*.h))

export

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
	
clean:
	$(foreach dir,$(SUBDIRS),$(MAKE) -C $(dir) clean;)
