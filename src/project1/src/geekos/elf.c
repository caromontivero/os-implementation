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

#define EV_NONE		0
#define EV_CURRENT	1

#define EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3
#define ELFMAG0		0x7f
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

#define PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB	5
#define PT_PHDR		6
#define PT_LOCPROC	0x70000000
#define PT_HIPROC	0x7fffffff


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
      int result = 0;
      elfHeader e_hdr;
      int i = 0;
      programHeader p_hdr;
      
      memcpy(&e_hdr,exeFileData,sizeof(elfHeader));
      if( (e_hdr.type != ET_EXEC) && 
        (e_hdr.machine != EM_386) && 
        (e_hdr.ident[EI_MAG0] != ELFMAG0) && (e_hdr.ident[EI_MAG1] != ELFMAG1) && (e_hdr.ident[EI_MAG2] != ELFMAG2) && (e_hdr.ident[EI_MAG3] != ELFMAG3) )
      {
          result = -1;
      }
      else
      {
          exeFormat->numSegments = e_hdr.phnum;            	
          //Load program headers
          while(i < e_hdr.phnum)
          {
              memcpy(&p_hdr,&exeFileData[e_hdr.phoff + sizeof(programHeader) * i],sizeof(programHeader));
              exeFormat->segmentList[i].offsetInFile = p_hdr.offset;
              exeFormat->segmentList[i].lengthInFile = p_hdr.fileSize;
              exeFormat->segmentList[i].startAddress = p_hdr.vaddr;
              exeFormat->segmentList[i].sizeInMemory = p_hdr.memSize;
              exeFormat->segmentList[i].protFlags = p_hdr.flags;
              i++;
          }

      }
        
      return result;
}

