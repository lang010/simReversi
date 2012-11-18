#
# File    : Makefile
# Arthur  : Lang Lee
# Created : Thu Nov  1 00:05:14 CST 2012
# Version : 0.1.0
#
# Makefile for the sources of simReversi.
# Just hit make.
#

CC = gcc
CFLAG = -std=c99
PRO = simReversi

all:
	$(CC) $(CFLAG)  src/*.c -o $(PRO)

