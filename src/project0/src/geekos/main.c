/*
 * GeekOS C code entry point
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, Iulian Neamtiu <neamtiu@cs.umd.edu>
 * $Revision: 1.51 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/bootinfo.h>
#include <geekos/string.h>
#include <geekos/screen.h>
#include <geekos/mem.h>
#include <geekos/crc32.h>
#include <geekos/tss.h>
#include <geekos/int.h>
#include <geekos/kthread.h>
#include <geekos/trap.h>
#include <geekos/timer.h>
#include <geekos/keyboard.h>

bool Key_Released(unsigned int key)
{
  return((key & KEY_RELEASE_FLAG) == 0);
}

bool Should_Exit(unsigned int key)
{
    return( key == (KEY_CTRL_FLAG | 'd') );
}

void Print_Key_Pressed()
{
  unsigned int key_pressed = 0;
  
  Print("Welcome to Caro!\n");
  while (!Should_Exit(key_pressed))
  {
    key_pressed = Wait_For_Key();
    if ( Key_Released(key_pressed) )
    {
      Print("%c",key_pressed);
    }
  }
  
    Print("\nGoodbye!\n");
}

/*
 * Kernel C code entry point.
 * Initializes kernel subsystems, mounts filesystems,
 * and spawns init process.
 */
void Main(struct Boot_Info* bootInfo)
{
    Init_BSS();
    Init_Screen();
    Init_Mem(bootInfo);
    Init_CRC32();
    Init_TSS();
    Init_Interrupts();
    Init_Scheduler();
    Init_Traps();
    Init_Timer();
    Init_Keyboard();

    Set_Current_Attr(ATTRIB(BLACK, GREEN|BRIGHT));
    Print("Welcome to GeekOS!\n");
    Set_Current_Attr(ATTRIB(BLACK, GRAY));
    Start_Kernel_Thread(Print_Key_Pressed, 0, PRIORITY_NORMAL, false);

    /* Now this thread is done. */
    Exit(0);
}









