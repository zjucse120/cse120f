// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"


//#ifndef _MEMORYMANAGER_H_
//#define _MEMORYMANAGER_H_
#include "memorymanager.h"
//#endif
//#include "memorymanager.h"
#ifdef HOST_SPARC
#include <strings.h>
//#include "memorymanager.h"
#endif


extern MemoryManager *mmu;
//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
//AddrSpace::Initialize 
//
//---------------------------------------------------------------------
int
AddrSpace::Initialize(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int i;
    unsigned int size;
    int code_file_off, code_virt_addr, code_size, code_size_load;

    int data_file_off, data_virt_addr, data_size, data_size_load;


    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
            (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
           + UserStackSize;	// we need to increase the size
    // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

  //  ASSERT(numPages <= NumPhysPages);		// check we're not trying
    // to run anything too big --
    // at least until we have
    // virtual memory
   
   DEBUG('a', "Initializing address space, num pages %d, size %d\n",
          numPages, size);

//if the numPages needed larger than NumePages can be used?
   if ((int)numPages >( mmu->NumPagesCanBeUsed())){
	printf("No enough memory!\n");
	return false;
	}

// first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
        //pageTable[i].physicalPage = i;
        pageTable[i].physicalPage = mmu->AllocPage();// now, allocate the phys page
        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
        // a separate page, we could set its
        // pages to be read-only
    }

// zero out the entire address space, to zero the unitialized data segment
// and the stack segment
   // bzero(machine->mainMemory, size);
   for (i = 0; i < numPages; i++) {
	memset(&(machine->mainMemory[pageTable[i].physicalPage*PageSize]),0,sizeof(PageSize));
	
   }

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
              noffH.code.virtualAddr, noffH.code.size);
      //  executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
      //                     noffH.code.size, noffH.code.inFileAddr);
      code_size_load = noffH.code.size;
      code_size = 0;
      code_file_off = noffH.code.inFileAddr;
      code_virt_addr = noffH.code.virtualAddr;
	//if the address begin at the middle of the page
      if ((noffH.code.virtualAddr % PageSize) != 0){
    	     
	        //code_file_off = noffH.code.inFileAddr;
		//code_virt_addr = noffH.code.virtualAddr;	     
		code_size = PageSize - noffH.code.virtualAddr % PageSize;
	     if (code_size > code_size_load)
                code_size = code_size_load;

		code_size_load = noffH.code.size - code_size;		
		executable->ReadAt(&(machine->mainMemory[Translate(code_virt_addr)]), code_size, code_file_off);
		}

	//if the address both begin and end at boundries
     while (code_size_load >= PageSize ){
		code_file_off = code_file_off + code_size ;
		code_virt_addr = code_virt_addr + code_size ;
		code_size = PageSize;		
		executable->ReadAt(&(machine->mainMemory[Translate(code_virt_addr)]), code_size, code_file_off);
                code_size_load = code_size_load - code_size;	
	        }	
	   
	 //if the address does not end at the boundry
     if (code_size_load > 0){
		code_file_off = code_file_off + code_size;
		code_virt_addr = code_virt_addr + code_size;
		code_size = code_size_load;				
		executable->ReadAt(&(machine->mainMemory[Translate(code_virt_addr)]), code_size, code_file_off);
	}                
    }

	//load data segments
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);           
      data_size_load = noffH.initData.size;
      data_size = 0;
      data_file_off = noffH.initData.inFileAddr;
      data_virt_addr = noffH.initData.virtualAddr;

      if ((noffH.initData.virtualAddr % PageSize) != 0){    	     
		data_size = PageSize - noffH.initData.virtualAddr % PageSize;

	     if (data_size > data_size_load)
                data_size = data_size_load;

		data_size_load = noffH.initData.size - data_size;		
		executable->ReadAt(&(machine->mainMemory[Translate(data_virt_addr)]), data_size, data_file_off);
		}

	
     while (data_size_load >= PageSize ){
		data_file_off = data_file_off + data_size ;
		data_virt_addr = data_virt_addr + data_size ;
		data_size = PageSize;		
		executable->ReadAt(&(machine->mainMemory[Translate(data_virt_addr)]), data_size, data_file_off);
                data_size_load = data_size_load - data_size;	
	        }	
	   
	 
     if (data_size_load > 0){
		data_file_off = data_file_off + data_size;
		data_virt_addr = data_virt_addr + data_size;
		data_size = data_size_load;
		executable->ReadAt(&(machine->mainMemory[Translate(data_virt_addr)]), data_size, data_file_off);
	}         

}
  return true;
}
	
//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{   
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{    
    unsigned int i;
    for (i = 0; i < numPages; i++){
	mmu->FreePage(pageTable[i].physicalPage);
    }
    delete [] pageTable;

}

//----------------------------------------------------------------------
//translate the virtualAddress to the physicalAddress
//----------------------------------------------------------------------
int
AddrSpace::Translate(int virtAddr)
{
    int physAddr;
    unsigned vpn,offset;
    TranslationEntry *entry;
    unsigned int pageFrame;

    vpn = (unsigned) virtAddr / PageSize;
    offset = (unsigned) virtAddr % PageSize;
    if (vpn >= numPages) {
            return AddressErrorException;
       } else if (!pageTable[vpn].valid) {
            return PageFaultException;
        }
        entry = &pageTable[vpn];

    pageFrame = entry->physicalPage;
    if (pageFrame >= NumPhysPages) {
        DEBUG('a', "*** frame %d > %d!\n", pageFrame, NumPhysPages);
        return BusErrorException;
    }
    entry->use = TRUE;		// set the use, dirty bits
	
    physAddr = pageFrame * PageSize + offset;
        	
    return physAddr;

}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}


