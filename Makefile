# See Rules.mk for the actual Makefile

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
_mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
I := $(patsubst %/,%,$(dir $(_mkfile_path)))

ifneq ($(words $(MAKECMDGOALS)),1)
.DEFAULT_GOAL = all
%:
	@$(MAKE) $@ --no-print-directory -rRf $(firstword $(MAKEFILE_LIST))
else
ifndef ECHO
ifeq ($(OS), Windows_NT)
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory -nrRf $(firstword $(MAKEFILE_LIST)) ECHO="COUNTTHIS" | find /c "COUNTTHIS")
else
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory -nrRf $(firstword $(MAKEFILE_LIST)) ECHO="COUNTTHIS" | grep -c "COUNTTHIS")
endif
N := x
C = $(words $N)$(eval N := x $N)
ECHO = echo [$C/$T]
endif

include Rules.mk

endif