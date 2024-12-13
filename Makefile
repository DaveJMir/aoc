DAYS := $(shell find . -maxdepth 1 -type d -name 'day*' | sort) skeleton
BINS := $(foreach day,$(DAYS),$(day)/$(notdir $(day)))

.PHONY: all clean test


# Default rule: build all days
all: $(BINS)

# Clean all days
clean: $(addsuffix /.clean, $(DAYS))

# Test all days
test: $(addsuffix /.test, $(DAYS))

# Include each day's rules as targets
$(BINS):
	$(MAKE) -C $(dir $@)

# Per-day clean and test rules
%/.clean:
	$(MAKE) -C $(dir $@) clean

%/.test:
	$(MAKE) -C $(dir $@) test

debug:
	@echo days $(DAYS)
	@echo bins $(BINS)
