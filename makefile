
CC = gcc
CFLAGS = -g -lm -I./intr

impl = ./impl
obj = ./obj

impl_files = $(wildcard $(impl)/*.c)
obj_files = $(patsubst $(impl)/%.c, $(obj)/%.o, $(impl_files))

run: $(obj_files)
	$(CC) main.c $^ $(CFLAGS)	


$(obj)/%.o : $(impl)/%.c
	@mkdir -p $(@D)
	@$(CC) $< -o $@ -c $(CFLAGS)

clean:
	@rm obj/* a.out disk/*
