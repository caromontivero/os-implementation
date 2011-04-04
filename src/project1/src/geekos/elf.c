/*
 * ELF executable loading
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.29 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/kassert.h>
#include <geekos/ktypes.h>
#include <geekos/screen.h>  /* for debug Print() statements */
#include <geekos/pfat.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/elf.h>

#define ET_NONE		0
#define ET_REL		1
#define ET_EXEC		2
#define ET_DYN		3
#define ET_CORE		4
#define ET_LOPROC	0xff00
#define ET_HIPROC	0xffff

#define EM_NONE		0
#define EM_M32		1
#define EM_SPARC	2
#define EM_386		3
#define EM_68K		4
#define EM_88K		5
#define EM_860		7
#define EM_MIPS		8

#define EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3
#define ELFMAG0		0x7f
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

//Returned value:
// 0 = success
// != 0 Failure

int Parse_ELF_Header(char *exeFileData, ulong_t exeFileLength, elfHeader *elf_hdr)
{
    int result = -1;
    if(sizeof(elfHeader) < exeFileLength)
    {
        memcpy(elf_hdr,exeFileData,sizeof(elfHeader));
        result = 0;
    }
    return result;
}

bool Is_Valid_ELF(elfHeader *elf_hdr)
{
    return ( (elf_hdr->type == ET_EXEC) && (elf_hdr->machine == EM_386) && (elf_hdr->ident[EI_MAG0] == ELFMAG0) && (elf_hdr->ident[EI_MAG1] == ELFMAG1) && (elf_hdr->ident[EI_MAG2] == ELFMAG2) && (elf_hdr->ident[EI_MAG3] == ELFMAG3) ); 
}

void Load_Program_Headers(char *exeFileData, struct Exe_Segment *segmentList, int offset, int numProgs)
{
    int count = 0;
    programHeader p_hdr;

    while(count < numProgs)
    {
        memcpy(&p_hdr,&exeFileData[offset + sizeof(programHeader) * count],sizeof(programHeader));
        segmentList[count].offsetInFile = p_hdr.offset;
        segmentList[count].lengthInFile = p_hdr.fileSize;
        segmentList[count].startAddress = p_hdr.vaddr;
        segmentList[count].sizeInMemory = p_hdr.memSize;
        segmentList[count].protFlags = p_hdr.flags;
        count++;  
    }
}
/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat)
{
      int result = -1;
      elfHeader *e_hdr = NULL;
      e_hdr = Malloc(sizeof(elfHeader));
      if(e_hdr != NULL)
      {
          if(Parse_ELF_Header(exeFileData,exeFileLength,e_hdr) == 0)
          {
              if(Is_Valid_ELF(e_hdr))
              {
	  exeFormat->numSegments = e_hdr->phnum;
	  Load_Program_Headers(exeFileData,exeFormat->segmentList,e_hdr->phoff, exeFormat->numSegments);
	  result = 0;
              }
              Free(e_hdr);
              e_hdr = NULL;
          }
      }
      return result;
}

