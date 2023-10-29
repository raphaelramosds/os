TARGETS := passo1 passo2 passo3 passo4 passo5 passo6 passo7

all: $(TARGETS)

%: %.c
	gcc -g -Wall -O3 -o $@ $^

clean:
	rm -rf $(TARGETS) $(addsuffix .o,$(TARGETS))

.INTERMEDIATE: $(addsuffix .o,$(TARGETS))

.SECONDARY:
