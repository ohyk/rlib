CC = clang
ARM_CC = arm-gcc
FLAGS = -g -fPIC -Wall -Werror -I inc -O3
BASE_SRC = src/rassert.c src/rexcept.c src/rarith.c src/ratom.c \
		   src/rarena.c src/rtest.c src/rtools.c src/rlog.c \
		   src/rvector.c src/rrandom.c
MEM_SRC = src/rmem.c
MEMCHK_SRC = src/rmemchk.c

ADT_SRC = src/rarray.c src/rtable.c src/rseq.c src/rring.c src/rlist.c \
		  src/rlink.c src/rqueue.c src/rset.c

BASE_OBJ = $(patsubst %.c, %.o, $(BASE_SRC))
MEM_OBJ = $(patsubst %.c, %.o, $(MEM_SRC))
MEMCHK_OBJ = $(patsubst %.c, %.o, $(MEMCHK_SRC))
ADT_OBJ = $(patsubst %.c, %.o, $(ADT_SRC))

librlib.so: $(BASE_OBJ) $(MEM_OBJ)
	$(CC) -shared $(FLAGS) $^ -o $@

arm: $(BASE_OBJ) $(MEM_OBJ)
	$(ARM_CC) -shared $(FLAGS) $^ -o arm_libriclib.so

memchk: $(BASE_OBJ) $(MEMCHK_OBJ)
	$(CC) -shared $(FLAGS) $^ -o libriclib.so

all: $(BASE_OBJ) $(ADT_OBJ)
	$(CC) -shared $(FLAGS) $^ -o libriclib.so

%.o: %.c
	$(CC) $(FLAGS) $< -c -o $@

.PHONY: clean
clean:
	-rm -rf $(BASE_OBJ) $(MEM_OBJ) $(MEMCHK_OBJ)
