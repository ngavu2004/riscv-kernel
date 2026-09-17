#include "elf.h"

void print_elf_header(ElfHeader *elf)
{
    int i;

    uart_putstr("ELF magic: ");
    for (i = 0; i < 4; i++)
    {
        uart_puthex(elf->e_ident[i]);
        uart_putstr(" ");
    }
    uart_putstr("\n");

    uart_putstr("Class: ");
    if (elf->e_ident[4] == 1)
    {
        uart_putstr("ELF32\n");
    }
    else if (elf->e_ident[4] == 2)
    {
        uart_putstr("ELF64\n");
    }
    else
    {
        uart_putstr("unknown\n");
    }

    uart_putstr("Endian: ");
    if (elf->e_ident[5] == 1)
    {
        uart_putstr("little\n");
    }
    else if (elf->e_ident[5] == 2)
    {
        uart_putstr("big\n");
    }
    else
    {
        uart_putstr("unknown\n");
    }

    uart_putstr("Version: ");
    uart_putuint64(elf->e_ident[6]);
    uart_putstr("\n");

    uart_putstr("Entry point: ");
    uart_puthex(elf->e_entry);
    uart_putstr("\n");

    uart_putstr("Elf header size: ");
    uart_puthex(elf->e_ehsize);
    uart_putstr("\n");

    uart_putstr("Size of an entry in the program header table: ");
    uart_putuint64(elf->e_phentsize);
    uart_putstr("\n");

    uart_putstr("Program header offset: ");
    uart_putuint64(elf->e_phoff);
    uart_putstr("\n");

    uart_putstr("Section header offset: ");
    uart_puthex(elf->e_shoff);
    uart_putstr("\n");

    uart_putstr("Machine: ");
    uart_puthex(elf->e_machine);
    uart_putstr("\n");

    uart_putstr("Number of program headers: ");
    uart_putuint64(elf->e_phnum);
    uart_putstr("\n");


}

int load_elf_header(ElfHeader *elf, const unsigned char * elf_start)
{
    if (elf->e_ident[0] == 0x7f && elf->e_ident[1] == 'E' && elf->e_ident[2] == 'L' && elf->e_ident[3] == 'F') {
        uart_putstr("Valid ELF file!\n");

        for (int i = 0; i < elf->e_phnum; i++)
        {
            ProgramHeader *curr_p = (ProgramHeader *)((char *)elf_start + elf->e_phoff + i * elf->e_phentsize);

            if (curr_p->p_type == PT_LOAD)
            {   
                uart_putstr("Load segment: ");
                uart_puthex(curr_p->p_vaddr);
                uart_putstr("\n");

                enable_user_memory(curr_p->p_vaddr, curr_p->p_memsz);

                // Copy p_filesz bytes from p_offset to p_vaddr
                char *src_addr = (char *)elf_start + curr_p->p_offset;
                char *dst_addr = (char *)(uintptr_t)curr_p->p_vaddr;
                for (uint64_t j = 0; j < curr_p->p_filesz; j++)
                {
                    dst_addr[j] = src_addr[j];
                };

                // Zero out the bytes from p_filesz to p_memsz
                for (uint64_t j = curr_p->p_filesz; j < curr_p->p_memsz; j++)
                {
                    dst_addr[j] = 0;
                }
            };
        }

        uart_putstr("Finished copy the load segments\n");
        return 0;
    }

    uart_putstr("Invalid Elf file!\n");
    return -1;
}