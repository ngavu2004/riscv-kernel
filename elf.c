#include "elf.h"
#include "csr.h"
#include "pmp.h"
#include "memory_management.h"

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

int load_elf_header(ElfHeader *elf, const unsigned char * elf_start, int entry, uintptr_t process_pt)
{
    if (elf->e_ident[0] == 0x7f && elf->e_ident[1] == 'E' && elf->e_ident[2] == 'L' && elf->e_ident[3] == 'F') {
        uart_putstr("Valid ELF file!\n");

        for (int i = 0; i < elf->e_phnum; i++)
        {
            ProgramHeader *curr_p = (ProgramHeader *)((char *)elf_start + elf->e_phoff + i * elf->e_phentsize);

            if (curr_p->p_type == PT_LOAD)
            {   
                if (curr_p->p_memsz < curr_p->p_filesz) {
                    return -1;
                };

                uart_putstr("Load segment: ");
                uart_puthex(curr_p->p_vaddr);
                uart_putstr("\n");

                // Get the number of page needed to allocate for this process
                uint64_t page_count = curr_p->p_memsz / PAGE_SIZE + (curr_p->p_memsz % PAGE_SIZE != 0);

                char *src_addr = (char *)elf_start + curr_p->p_offset;
                
                for (int page = 0; page<page_count; page++) {
                    // Copy p_filesz bytes from p_offset to the allocated page
                    char *src_addr = (char *)elf_start + curr_p->p_offset + PAGE_SIZE * page;
                    char *dst_addr = page_allocate();

                    if (dst_addr == NULL) {
                        return -1;
                    }

                    uint64_t segment_offset = PAGE_SIZE * page;
                    uint64_t bytes_to_copy = 0;

                    if (segment_offset < curr_p->p_filesz) {
                        uint64_t bytes_remaining =
                            curr_p->p_filesz - segment_offset;

                        bytes_to_copy =
                            bytes_remaining < PAGE_SIZE
                                ? bytes_remaining
                                : PAGE_SIZE;
                    }

                    uint64_t va = curr_p->p_vaddr + PAGE_SIZE * page;
                    uint64_t flags = PTE_V | PTE_U;
                    
                    // Map the segment according to the flag
                    if (curr_p->p_flags & PF_R) flags |= PTE_R;
                    if (curr_p->p_flags & PF_W) flags |= PTE_W;
                    if (curr_p->p_flags & PF_X) flags |= PTE_X;

                    map_page(
                        (pagetable_t *)process_pt,
                        va,
                        (uint64_t)dst_addr,
                        1,
                        flags
                    );

                    for (uint64_t j = 0; j < bytes_to_copy; j++) {
                        dst_addr[j] = src_addr[j];
                    }

                    // Zero out the bytes from p_filesz to p_memsz at the last page
                    if (segment_offset + bytes_to_copy < curr_p->p_memsz) {
                        uint64_t zero_start = bytes_to_copy;
                        uint64_t zero_end = PAGE_SIZE;

                        if (segment_offset + zero_end > curr_p->p_memsz) {
                            zero_end = curr_p->p_memsz - segment_offset;
                        }

                        for (uint64_t j = zero_start; j < zero_end; j++) {
                            dst_addr[j] = 0;
                        }
                    }
                    
                }

                
            };
        }

        uart_putstr("Finished copy the load segments\n");
        return 0;
    }

    uart_putstr("Invalid Elf file!\n");
    return -1;
}