
FILES = \
		main.c \
    	os/os.c \
    	os/vga.c \
    	os/dbg.c \
    	os/log.c \
    	os/mouse.c \
    	lib/list.c \
    	window/event.c \
    	window/rect.c \
    	window/window.c \
    	window/desktop.c \
    	window/label.c \
    	window/button.c

all:
	./run.sh

cc: clean ${FILES}
	${CC} -c -Werror -Wall -I. ${FILES} 

gcc: clean ${FILES}
	gcc -c -Werror -Wall -I. ${FILES} 

clang: clean ${FILES}
	clang -c -Werror -Wall -I. ${FILES} 

warning: cc gcc clang
	# check for warnings

clean:
	rm -f *.exe *.s *.o
