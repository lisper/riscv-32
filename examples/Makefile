CC = /opt/tools/riscv/bin/riscv64-unknown-elf-gcc
LD = /opt/tools/riscv/bin/riscv64-unknown-elf-ld
OBJCOPY = objcopy
OBJDUMP = /opt/tools/riscv/bin/riscv64-unknown-elf-objdump

#ARCH=rv64i
#ARCH=rv32g
#ARCH=rv32e
ARCH=rv32i
#ARCH=rv32ic
#ARCH=rv32im
#ARCH=rv32imc

ABI=ilp32

all: m1 m2 m3 m4 m5

m1: main1.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c main1.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c start.s
	$(LD) -o m1.elf -Triscv32-virt.ld start.o main1.o
	$(OBJCOPY) -O ihex -j .init -j .text -j .eh_frame m1.elf m1.hex
	$(OBJDUMP) -D m1.elf >m1.elf.dis.txt

m2: main2.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c main2.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c start.s
	$(LD) -o m2.elf -Triscv32-virt.ld start.o main2.o
	$(OBJCOPY) -O ihex -j .init -j .text -j .eh_frame m2.elf m2.hex
	$(OBJDUMP) -D m2.elf >m2.elf.dis.txt

m3: main3.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c main3.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c start.s
	$(LD) -o m3.elf -Triscv32-virt.ld start.o main3.o
	$(OBJCOPY) -O ihex -j .init -j .text -j .eh_frame m3.elf m3.hex
	$(OBJDUMP) -D m3.elf >m3.elf.dis.txt

m4: main4.c utils.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c utils.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c main4.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c start.s
	$(LD) -o m4.elf -Triscv32-virt.ld start.o main4.o utils.o
	$(OBJCOPY) -O ihex -j .init -j .text -j .eh_frame m4.elf m4.hex
	$(OBJDUMP) -D m4.elf >m4.elf.dis.txt

m5: printf.c utils.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c utils.c
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c printf.c -DTEST_PRINTF
	$(CC) -march=$(ARCH) -mabi=$(ABI) -c start.s
	$(LD) -o m5.elf -Triscv32-virt.ld start.o printf.o utils.o
	$(OBJCOPY) -O ihex -j .init -j .text -j .rodata -j .eh_frame m5.elf m5.hex
	$(OBJDUMP) -D m5.elf >m5.elf.dis.txt

clean:
	rm -f *.elf *.dis.txt *.o *.hex
