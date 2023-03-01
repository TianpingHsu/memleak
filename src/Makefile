

test: test.o memleak.o
	g++ -o a.out $^ -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=_Znwm -Wl,--wrap=_ZdlPv -Wl,--wrap=_Znam -Wl,--wrap=_ZdaPv -lpthread

clean:
	@rm a.out memleak.o test.o
