# Makefile for netlist Parser

FLEX = flex
BISON = bison3

BISON_TMP = CktParser.output location.hh stack.hh position.hh

SRCS = CktScanner.cpp\
	   CktParser.cpp\
	   CktParser.hpp

all: $(SRCS)

CktScanner.cpp: CktScanner.l
	$(FLEX) -o $@ $<

CktParser.cpp CktParser.hpp: CktParser.y
	$(BISON) -d $<

.PHONY: clean distclean

RM = rm -f

clean:
	$(RM) $(BISON_TMP) $(SRCS)

distclean:
	$(RM) $(BISON_TMP) $(SRCS)
