FILES = ./build/kernel.asm.o build/kernel.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/mem/mem.o ./build/IO/io.asm.o ./build/mem/heap/heap.o ./build/mem/heap/kheap.o ./build/mem/paging/paging.o ./build/mem/paging/paging.asm.o ./build/disk/disk.o ./build/fs/pparser.o ./build/string/string.o ./build/fs/file.o ./build/fs/FAT/fat16.o ./build/disk/streamer.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	sudo mount -t vfat ./bin/os.bin /mnt/d
	# copy a file over
	sudo cp ./hello.txt /mnt/d


	sudo umount /mnt/d

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/IO/io.asm.o: ./src/IO/io.asm
	nasm -f elf -g ./src/IO/io.asm -o ./build/IO/io.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I ./src/idt  $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

./build/mem/mem.o: ./src/mem/mem.c
	i686-elf-gcc $(INCLUDES) -I ./src/mem $(FLAGS) -std=gnu99 -c ./src/mem/mem.c -o ./build/mem/mem.o

./build/mem/heap/heap.o: ./src/mem/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I ./src/mem/heap $(FLAGS) -std=gnu99 -c ./src/mem/heap/heap.c -o ./build/mem/heap/heap.o

./build/mem/heap/kheap.o: ./src/mem/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I ./src/mem/heap $(FLAGS) -std=gnu99 -c ./src/mem/heap/kheap.c -o ./build/mem/heap/kheap.o

./build/mem/paging/paging.o: ./src/mem/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I ./src/mem/paging $(FLAGS) -std=gnu99 -c ./src/mem/paging/paging.c -o ./build/mem/paging/paging.o

./build/mem/paging/paging.asm.o: ./src/mem/paging/paging.asm
	nasm -f elf -g ./src/mem/paging/paging.asm -o ./build/mem/paging/paging.asm.o

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I ./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

./build/fs/pparser.o: ./src/fs/pparser.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/pparser.c -o ./build/fs/pparser.o

./build/string/string.o: ./src/string/string.c
	i686-elf-gcc $(INCLUDES) -I./src/string $(FLAGS) -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o

./build/fs/file.o: ./src/fs/file.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/file.c -o ./build/fs/file.o
./build/fs/FAT/fat16.o: ./src/fs/FAT/fat16.c
	i686-elf-gcc $(INCLUDES) -I./src/fs -I./src/fs/FAT $(FLAGS) -std=gnu99 -c ./src/fs/FAT/fat16.c -o ./build/fs/FAT/fat16.o
./build/disk/streamer.o: ./src/disk/streamer.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/streamer.c -o ./build/disk/streamer.o
clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf $(FILES)
	rm -rf ./build/kernelfull.o