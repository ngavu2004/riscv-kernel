
#include <stdint.h>

// Note: this is the standard name for header struct. Source: https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
typedef struct {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry; // we are for sure on 64-bit system so we'll use 8 bytes for the entry
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shstrndx;

} ElfHeader;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} ProgramHeader;

enum PT_TYPES {
  PT_NULL,
  PT_LOAD,
  PT_DYNAMIC,
  PT_INTERP,
  PT_NOTE,
  PT_SHLIB,
  PT_PHDR,
  PT_LOPROC=0x70000000, //reserved
  PT_HIPROC=0x7FFFFFFF  //reserved
};

void uart_putstr(const char*);
void uart_putuint64(uint64_t num);
void uart_puthex(uint64_t value);

void print_elf_header(ElfHeader* elf);

int load_elf_header(ElfHeader *elf, const unsigned char * elf_start);