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
#include "processmanager.h"
#include "memorymanager.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//char input[1] = {'1'};
//char* a = input;
int algorithm = 1;
extern MemoryManager *mmu;
extern ProcessTable *pt;
extern MemoryStore *ms;
class BackingStore;
//Statistics *stats;
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
AddrSpace::Initialize(OpenFile *executable,int pid)
{
    unsigned int i;
    unsigned int size;
    bstore =  new BackingStore(this, pid);


    Executable = executable;
    Executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
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
	//return false;
	}

// first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
        //pageTable[i].physicalPage = i;
        //pageTable[i].physicalPage = mmu->AllocPage();// now, allocate the phys page
        pageTable[i].valid = FALSE;
        pageTable[i].stored = FALSE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
        // a separate page, we could set its
        // pages to be read-only
    }

  return true;
}

    
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
bool
AddrSpace::DemandSpace(OpenFile *executable, int badvpn)
{
        
    int code_pn, code_virt_addr, code_file_off, code_size;
    int data_pn, data_file_off, data_virt_addr, data_size;

// first, set up the translation
    int n;
	n = mmu->AllocPage();
    if(n==-1){
        return false;
    }
    else{
	stats->numPageFaults ++ ;
        TranslationEntry * pte;
        pte = &pageTable[badvpn];
        pageTable[badvpn].physicalPage = n;// now, allocate the phys page
        ms->Store(n, pte, this->bstore);
              	
    }
    
   if(!pageTable[badvpn].stored){
    code_pn = (noffH.code.virtualAddr + noffH.code.size)/PageSize;
    data_pn = (noffH.initData.virtualAddr + noffH.initData.size)/PageSize;
    
   if((code_pn >= badvpn && noffH.code.size>0) || (data_pn >= badvpn && code_pn <= badvpn && noffH.initData.size>0)) 
        stats->numPageIns ++;
  
         DEBUG('a', "One pagein happened, %d total.\n",stats->numPageIns);	    	
         if(code_pn >= badvpn){
         if (noffH.code.size > 0) {	
	    //stats->numPageIns ++ ;
            DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
                    noffH.code.virtualAddr, noffH.code.size);
                
            //if the address begin at the middle of the page
            if (noffH.code.virtualAddr/PageSize == badvpn){    
		code_virt_addr = noffH.code.virtualAddr;                    
                code_size = PageSize - noffH.code.virtualAddr % PageSize;
                code_file_off = noffH.code.inFileAddr;    
                executable->ReadAt(&(machine->mainMemory[Translate(code_virt_addr)]), code_size, code_file_off);
            }
                
            //if the address both begin and end at boundries
            else if (code_pn > badvpn){
                //if(code_pn < data_pn || (badvpn>=data_pn)){
                code_file_off = noffH.code.inFileAddr + PageSize * badvpn - noffH.code.virtualAddr;
                code_virt_addr = PageSize * badvpn ;
                code_size = PageSize;
                executable->ReadAt(&(machine->mainMemory[Translate(code_virt_addr)]), code_size, code_file_off);
               // }
             }
                        
            //if the address does not end at the boundry
           else if (code_pn == badvpn){ 
                code_file_off = noffH.code.inFileAddr + PageSize * badvpn - noffH.code.virtualAddr;
                code_virt_addr = PageSize * badvpn ;
                code_size = (noffH.code.virtualAddr + noffH.code.size) % PageSize;
                executable->ReadAt(&(machine->mainMemory[Translate(code_virt_addr)]), code_size, code_file_off);
		memset(&(machine->mainMemory[pageTable[badvpn].physicalPage*PageSize + code_size]),0, sizeof(PageSize-code_size));
            }
        }
    }

    if((data_pn >= badvpn)&&(badvpn >= code_pn)){
        if (noffH.initData.size > 0) {		
            DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
                    noffH.initData.virtualAddr, noffH.initData.size);
                
            //if the address begin at the middle of the page
            if (noffH.initData.virtualAddr/PageSize == badvpn){    
		data_virt_addr = noffH.initData.virtualAddr;                    
                data_size = PageSize - noffH.initData.virtualAddr % PageSize;
                data_file_off = noffH.initData.inFileAddr;    
                executable->ReadAt(&(machine->mainMemory[Translate(data_virt_addr)]), data_size, data_file_off);
            }
                
            //if the address both begin and end at boundries
            else if (data_pn > badvpn){
                //if((data_pn < code_pn) || (badvpn >= code_pn)){    
                data_file_off = noffH.initData.inFileAddr + PageSize * badvpn - noffH.initData.virtualAddr;
                data_virt_addr = PageSize * badvpn ;
                data_size = PageSize;
                executable->ReadAt(&(machine->mainMemory[Translate(data_virt_addr)]), data_size, data_file_off);
                //}
             }
                
            //if the address does not end at the boundry
           else if (data_pn == badvpn){    
                data_file_off = noffH.initData.inFileAddr + PageSize * badvpn - noffH.initData.virtualAddr;
                data_virt_addr = PageSize * badvpn ;
                data_size = (noffH.initData.virtualAddr + noffH.initData.size) % PageSize;
                executable->ReadAt(&(machine->mainMemory[Translate(data_virt_addr)]), data_size, data_file_off);
		memset(&(machine->mainMemory[pageTable[badvpn].physicalPage*PageSize + data_size]),0, sizeof(PageSize-data_size));
            }
        }
    }
   
           else if (data_pn < badvpn){
         	memset(&(machine->mainMemory[pageTable[badvpn].physicalPage*PageSize]),0,sizeof(PageSize));
                   }

    }
    else {
	bstore->PageIn(&pageTable[badvpn]);
	stats->numPageIns ++ ;
        DEBUG('a', "One pagein happened, %d total.\n", stats->numPageIns);

	}
    return true;    
}

//
//
//----------------------------------------------------------------------
void
AddrSpace::MarkPage(int badvpn){
        pageTable[badvpn].valid = TRUE;
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
        if(pageTable[i].valid){
	mmu->FreePage(pageTable[i].physicalPage);
        }
    }
    delete [] pageTable;
    delete  Executable;
    bstore->~BackingStore();
  

}

//---------------------------------------------------------------------
//
//----------------------------------------------------------------------
bool
AddrSpace::ReadMem(int addr, int size, int *value)
{
    int data;
    ExceptionType exception;
    int physicalAddress;

    DEBUG('a', "Reading VA 0x%x, size %d\n", addr, size);

    exception = machine->Translate(addr, &physicalAddress, size, FALSE);
    if ((exception != NoException) && (exception != PageFaultException)) {
        machine->RaiseException(exception, addr);
        return FALSE;
    }
    switch (size) {
    case 1:
        data = machine->mainMemory[physicalAddress];
        *value = data;
        break;

    case 2:
        data = *(unsigned short *) &machine->mainMemory[physicalAddress];
        *value = ShortToHost(data);
        break;

    case 4:
        data = *(unsigned int *) &machine->mainMemory[physicalAddress];
        *value = WordToHost(data);
        break;

    default:
        ASSERT(FALSE);
    }

    DEBUG('a', "\tvalue read = %8.8x\n", *value);
    return (TRUE);
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

        entry = &pageTable[vpn];

    pageFrame = entry->physicalPage;

	
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

//---------------------------------
//return pageNum for PTE
//---------------------------------
int 
AddrSpace::GetPTEpageNum(TranslationEntry *PTE){
      unsigned int i;
      for(i = 0;i<numPages;i++)
      {

      if (PTE == &pageTable[i])
      return i;
      }
      return -1;
}
//------------------------------------
//return pageNum
//-------------------------------------
int 
AddrSpace::GetpageNum(){
     return numPages;
}
//-----------------------------------------
// evit page
//
//----------------------------------------
void
AddrSpace::Evict(){

    TranslationEntry *pte;

       int i;
        for(i = 0; i < NumPhysPages; i++){
	TranslationEntry *entry;
	   entry = ms->Vpn[i].pte;
        if(entry->use){
	   ms->Vpn[i].Counter = 0;
	   entry->use = FALSE;
	}
	   else ms->Vpn[i].Counter ++;
        }
    //switch(a[0]){
      switch(algorithm){
        case 1:
            pte = ms->ReturnPTE_Rand();
            break;
        case 2:
            pte = ms->ReturnPTE_FIFO();
            break;
        case 3:
            pte = ms->ReturnPTE_LRU();
            break;
        default:    
            pte = ms->ReturnPTE_Rand();
            break;
    }    pte->valid = FALSE;
    
    if(pte->dirty)
    {
      BackingStore *bs;
      bs = ms->ReturnBS(pte->physicalPage);
      bs->PageOut(pte);
      stats->numPageOuts ++;
      DEBUG('a', "One pageout happened, %d total.\n", stats->numPageOuts);
      printf("pageouts %d\n", stats->numPageOuts);
      pte->dirty = FALSE; 
      pte->stored = TRUE;   
    }
    mmu->FreePage(pte->physicalPage); 
    
}

BackingStore::BackingStore(AddrSpace *as, int pid)
{   int numpages;
    space = as;

    sprintf ( filename, "%d", pid); // %d makes the result be a decimal integer
    BSFile = new FileSystem(true);
    numpages = space->GetpageNum();
 
    ASSERT(BSFile->Create(filename,numpages*PageSize));
    BSExec = BSFile->Open(filename);

}

BackingStore::~BackingStore(){
   delete BSFile;
   delete BSExec;
}

void
BackingStore::PageOut(TranslationEntry *pte)
{
   
    int pagenum;
    int phys_addr;
    pagenum = space->GetPTEpageNum(pte);
    phys_addr = space->Translate(pagenum*PageSize);
    BSExec->WriteAt(&machine->mainMemory[phys_addr], PageSize, pagenum*PageSize);
}


void
BackingStore::PageIn(TranslationEntry *pte)
{
     int pagenum;
     int phys_addr;
     pagenum = space->GetPTEpageNum(pte);
     phys_addr = space->Translate(pagenum*PageSize);
     BSExec->ReadAt(&machine->mainMemory[phys_addr], PageSize, pagenum*PageSize);
}
