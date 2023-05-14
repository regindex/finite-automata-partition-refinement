# compilation flags
#CCX_FLAGS=-std=c++11 -O3 -Wall -Wextra -pedantic -DNDEBUG -g -I /usr/local/include/ -L /usr/local/lib/
CCX_FLAGS=-std=c++17 -g -O3 -Wall -Wextra -pedantic
#CCX_FLAGS=-std=c++17 -Ofast -Wall -Wextra -pedantic -g
#CCX_FLAGS=-std=c++11 -O3 -Wall -Wextra -pedantic -g -I /usr/local/include/ -L /usr/local/lib/
CCX=g++

# main executables 
EXECS = newexe newexe_verb

# targets not producing a file declared phony
.PHONY: all clean

all: $(EXECS)

newexe: exe.cpp lib/part_ref.cpp internal/NFA.hpp internal/DFA.hpp internal/partition.hpp internal/definitions.hpp external/malloc_count/malloc_count.o 
	$(CCX) $(CCX_FLAGS) -o $@ exe.cpp lib/part_ref.cpp internal/NFA.hpp internal/DFA.hpp internal/partition.hpp internal/definitions.hpp external/malloc_count/malloc_count.o -ldl

newexe_verb: exe.cpp lib/part_ref.cpp internal/NFA.hpp internal/DFA.hpp internal/partition.hpp internal/definitions.hpp external/malloc_count/malloc_count.o 
	$(CCX) $(CCX_FLAGS) -o $@ exe.cpp lib/part_ref.cpp internal/NFA.hpp internal/DFA.hpp internal/partition.hpp internal/definitions.hpp external/malloc_count/malloc_count.o -ldl -DVERBOSE

#newexe2: exe2.cpp lib/part_ref.cpp internal/NFA.hpp internal/DFA.hpp internal/partition.hpp internal/definitions.hpp external/malloc_count/malloc_count.o 
#	$(CCX) $(CCX_FLAGS) -o $@ exe2.cpp lib/part_ref.cpp internal/NFA.hpp internal/DFA.hpp internal/partition.hpp internal/definitions.hpp external/malloc_count/malloc_count.o -ldl

clean:
	rm -f $(EXECS)