
CC = gcc
CFLAGS = -lm -I./intr

impl = ./impl
obj = ./obj

impl_files = $(wildcard $(impl)/*.c)
impl_files += $(wildcard $(impl)/list/*.c)
obj_files = $(patsubst $(impl)/%.c, $(obj)/%.o, $(impl_files))

run: $(obj_files)
	$(CC) main.c $^ -o list $(CFLAGS)	

.PHONY:disk_creator
disk_creator: $(obj)/bitmap.o $(obj)/helper.o $(obj)/bitsarr.o $(obj)/disk.o 
	$(CC) vdd.c $^ $(CFLAGS)
	@./a.out
	@rm a.out

$(obj)/%.o : $(impl)/%.c
	@mkdir -p $(@D)
	@$(CC) $< -o $@ -c $(CFLAGS)

clean:
	@rm obj/* list disk/* bitmap saved_list
