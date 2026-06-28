CPP ?= cpp
ifeq ($(suffix $(SHELL)),.exe)
    CPP := cpp
    RM := del
endif
OUT_PATH := ./preprocess
HEADERS := $(addprefix $(OUT_PATH)/,$(wildcard *.h))

all: $(HEADERS)

$(OUT_PATH)/%.h: %.h
	$(CPP) $< > $@

clean:
	$(RM) $(OUT_PATH)/*

.phony: all clean
