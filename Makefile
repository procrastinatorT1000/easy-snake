all:
ifeq ($(OS),Windows_NT)
	call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" amd64 && \
	cl /DWINDOWS /EHsc snake.c
else
	gcc -o snake snake.c
endif

clean:
ifneq ($(OS),Windows_NT)
	-rm snake
endif

