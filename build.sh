#!/bin/bash

make run
nasm -f elf64 nasm.s -o nasm.o
gcc -no-pie nasm.o -o nasm
./nasm