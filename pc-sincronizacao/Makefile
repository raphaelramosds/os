TARGETS := ex1 ex2 ex3 ex4 peterson1 peterson2 peterson3

all: $(TARGETS)

%: %.c
	gcc -Wall -O3 -o $@ $^

clean:
	rm -rf $(TARGETS) $(addsuffix .o,$(TARGETS))

.INTERMEDIATE: $(addsuffix .o,$(TARGETS))

.SECONDARY:
