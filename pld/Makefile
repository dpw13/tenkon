GALETTE := ../../galette/target/release/galette

ALL_PLDS := $(wildcard *.pld)
ALL_JEDS := $(ALL_PLDS:.pld=.jed)

all: $(ALL_JEDS)

%.jed: %.pld
	$(GALETTE) $<