Sac State, Computer Engineering/Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang

Phase 8 Virtual Process Runtime Space (Virtual Memory)


Goals

So far a new process only runs within a confined 4KB memory space.
In this phase a one-giga (1G) bytes of virtual runtime space is
given to each new process: bytes 1G (0x40000000) to 2G-1 (0x7fffffff).

A virtual space is actually fragmently supported by real DRAM pages
(recently used instructions and data, in 4KB units/pages). A hardware
device Memeory Mapping/Management Unit (MMU) will translate each address
for the CPU, in order to address the memory system (via caches) to
fetch an instruction or access data.

A process created by the sys_vfork() call will run in its virtual space.
The EIP of the process will be set at 1GB, and its trapframe will be
created toward the end of the virtual space (2G - trapframe size).

In VForkSR(), besides a new process, a translation directory is also
created for the MMU to translate every address during the process run.
As a virtual-space process is to be loaded by the Loader(), the MMU
should first be given the address of the process directory:
set_cr3(directory address). As the process exits, the MMU will be
returned to use the original kernel directory instead.

The Kernel Directory (KDir) is created by the SPEDE code. We can
obtain its address during the bootstrap by get_cr3(). This address
is used to return the MMU to the real address space as a virtual
space process runtime ends, and for the following reason.

The first 16 entries in it point to 16 tables which in terms point
to the total 64M-byte DRAM memory in the target PC. These entries
are copied to create the new directory when creating a new process
in VForkSR() so its runtime can map into the kernel space (as needed
for the CPU to fetch service instructions and access kernel data
when responding to events).


Virtual Address Translation

The MMU uses a two-level address translation method: given a 32-bit
virtual address, the leftmost 10 bits form an index number to point
to an entry in a directory. There, a real DRAM byte address of a
table can be found. The next 10 bits in the virtual address, again,
as an index, points to an entry in the found table. There, the
address of a 4K-byte instruction/data page can be found. The final
12 bits in the virtual address is the byte address in the found
page, to locate an instruction or data.

Each PCB will need the information of an 'unsigned Dir' (directory)
to hold the real address of a directory for the new process (to
find instruction and data pages). The directory address will be
used in the kernel code to call set_cr3() to set the MMU so it
can translation virtual instruction addresses to get instructions.
Then, the Loader() is called with the virtual address of the
process trapframe to get it to be loaded to run the process.

A valid entry in a directory/table is initialized with the real
byte address of a 4KB DRAM page. Initially, most entries are not
used. During translation, if encountering an invalid/empty entry,
the MMU will issue a 'not-present' runtime exception to the CPU,
and the event is a type of 'page fault' to be handled but not in
this OS project phase.

There are flag-bits tagged in each translation entry, e.g.,
'present (or not),' 'read-only (or read/writable),' etc.
The flags indicate the access restrictions of the destination
location that the entry points to. (More about flags later.)

Initially, there are 5 DRAM pages that are needed in order to
create a virtual space-running process: directory (Dir),
instrution table (IT), data table (DT), instruction page (IP),
and data page (DP). A directory/table contains 1024 entries.
Each entry is 4 bytes (20 address bits and 12 flag bits).

Every virtual process space starts at byte 1G (0x40000000)
where the process instructions have been copied to (EIP). The
MMU reads the first 10 bits (64) in EIP and uses it to index
into the Dir to get the address for an IT. The second 10 bits
(0) indexes into the IT to get the address of an IP. The last
12 bits (0) is the byte offset in the page to fetch a CPU
instruction. Hence, the above entries are to be set correctly
in anticipation of the MMU's visit as the process runs.

As and trapframe of the new process will be located towards the
end of the virtual space byte 2G - sizeof(tf_t) = 0x7fffffc0.
The first 10 bits (511) is the entry of the Dir where the address
of a DT can be located. The second 10 bits of this virtual address
(1023) is the entry a DP can be located. The last 12 bits (4048)
are the byte offset in DP. Hence, the above entries are to be
set correctly in anticipation of the MMU's visit.

When recycling an exiting process, not only these five pages should
return to the kernel's procession but also the MMU is to return to
use the kernel's directory. Otherwise, the system reboot when the
translation tables can no longer be found (especially when the five
pages are erased when recycling).

When creating the trapframe for a virtual space process, its virtual
EIP is set to 1G, and the virtual trapframe address is 2G - size of
the trapframe (48). We should declare constants G1 and G2 for them.


2-Level Address Translation in Detail

The 2 levels are a single 1024-entry directory and 1024 tables. The
purpose is to save DRAM space since most entries in the directory
would be empty (sparse entry usage). A single-level translation
would mean each process needs a big table that has 1024x1024=1048576
entries (4M bytes in size). And, since translation tables cannot be
swapped out (to the system swap space), a typical laptop computer
starting with 300+ processes would need more than 300x4M=1.2GB of
DRAM just for their translation tables (not counting code and data)!

A 32-bit virtual address is read by MMU into 3 parts:
The leftmost 10 bits form an entry index to a directory to find the
address of a table. The second leftmost 10 bits form an entry index
to the found table to find the address of a page.  The rest (rightmost)
12 bits form the byte offset in the found page for instruction/data.

To create the needed translation information that the MMU will be using
for this process, during VForkSR(), 5 free DRAM pages are allocated for
Directory (Dir), Instruction Table (IT), Data Table (DT), Instruction
Page (IP), and Data Page (DP). And, clear their contents and initialize
their contents:

Dir:
   copy the first 16 entries from the kernel directory (KDir);
   entry 256 should be set to the address of IT, with the correct flag bits;
   entry 511 should be set to the address of DT, plus the correct flag bits.
IT:
   entry 0 should be set to the address of IP, with the correct flag bits;
DT:
   entry 1023 should be set to the address of DP, with the correct flag bits;
IP:
   contains the process instructions.
DP:
   contains the process trapframe (the EIP in the trapframe = G1)

(The trapframe address of the virtual space process = G2 - trapframe size.)

Since the size of each page is 4KB, its address is always an interval of
4096 (in binary, it has 12 trailing zeroes). Only the first 20 bits in
a 32-bit byte address differ. Hence, only the first 20 bits in an entry
are used to indicate the real page address, the 12 attribute flags follow.
The important flags are listed below, from the rightmost bit:
   Present -- 0: not present, 1: present.
   R/O or R/W -- 0: read-only (e.g., instructions, constants), 1: read-writable.
   Access Privilege -- 0: only superuser can access, 1: user can access
   Cache Policy -- 0: write back, 1: write-through (if use cache)
   Cache Use -- 0: do not cache, 1: use cache
   Accessed -- 0: not accessed, 1: accessed (by MMU)
   Dirty -- 0: content not written, 1: written (since entry created)

We will only practice setting both the Present and R/W flags to 1.

The target PC would reboot when there's an error or conflict in or
between a flag against the current runtime condition: page not
present (most common), an instruction writes to a R/O page, the
process is in the user mode but tries to access into the kernel
space (to read/write), etc.

It is advised that you use GDB to verify the contents of the
translation entries toward the end of VForkSR().

The ExitSR and WaitSR will be modified to handle side effects using
virtual space: during ExitSR it will need to set the MMU to return
to the use of the kernel directory (KDir) as the calling process
ends since its directory page is recycled (can be erased). Further,
during WaitSR(), in order to get into a ZOMBIE process' virtual
space to get its exit code, the MMU will need to be switched to use
the process' directory temporarily to access its trapframe.


Memory Page Management

Our OS will need to bookkeep the usage of DRAM memory pages.
With constants: PAGE_MAX=100, PAGE_SIZE=4096, DRAM_START=0xe00000,
declare: page_t page[PAGE_MAX], and
page_t has:              // each 4KB DRAM page has these info:
   int pid;              // process using it (initially NONE)
   union {               // 3 names for the same data
      unsigned addr;     // its byte addr: 0xe00000+i*page size
      char *content;     // use addr as ptr to content
      unsigned entry[];  // use addr as addr of array
   }
The page[] will be initialized during the bootstrap.


Deliverables

Source files as usual, other restrictions and requirements of not conducting
plagiarism as usual (see the deiverable description in previous phases).


Questions

Where in your programs the get_cr3() and set_cr3() functions are used?

What are the type of page faults that may occur as the MMU looks into
a translation entry during address translation?

How to write code to trigger page faults so it will need new pages
allocated during runtime? How should your OS handle this? Wouldn't
the recovery of a page fault requires rolling back the instruction
pointer to retry the intruction?

What is the pragmatic purpose of the 2-level address translation?

What do the UNIX "pagedaemon" and "swapper" processes do?

How can our OS maintain a healthy proportion of DRAM space not used?
How can LRU counts be implemented? How to indicate a page was swapped
out in the swap space (not brand new)?

Can translation tables also be subject to swapped in and out with a
"swap disk partition" like normal process pages?

The MMU has TLB to cache table entries. The instruction and data are
cached separately. Should the search for a cache hit be based on a
virtual or real address?

How to create a service for shared memory pages (IPC: shared memory)?
