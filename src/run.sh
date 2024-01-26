#!/bin/bash
bcc -ansi -Md -I. -o main.exe \
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
    window/button.c \
    \
    && dosbox -exit main.exe
    #&& dosbox -exit main.exe
    #&& dosbox main.exe
