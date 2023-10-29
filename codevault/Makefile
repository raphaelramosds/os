TARGETS := lesson1 lesson2 lesson3 lesson4

all: $(TARGETS)

%: %.c
	gcc -g -pthread -o $@ $^

clean:
	rm -rf $(TARGETS) $(addsufix .o, $(TARGETS))

.INTERMEDIATE: $(addsufix .o, $(TARGETS))

.SECONDARY: 
