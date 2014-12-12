#include "copyright.h"
#ifndef _BACKINGSTORE_H_
#define _BACKINGSTORE_H_
#include "backingstore.h"
#endif
#include "system.h"


BackingStore::BackingStore(AddrSpace *as)
{   int numpages;
    space = as;
    int pid = 123;  // string which will contain the number
    sprintf ( filename, "%d", pid); // %d makes the result be a decimal integer
    BSFile = new FileSystem(true);
    numpages = space->GetpageNum();
    ASSERT(BSFile->Create(filename,numpages*PageSize));
}


void
BackingStore::PageOut(TranslationEntry *pte)
{
   
    int pagenum;
    int phys_addr;
    pagenum = space->GetPTEpageNum(pte);
    phys_addr = space->Translate(pagenum*PageSize);
    OpenFile *BSExec = BSFile->Open(filename);
    BSExec->WriteAt(&machine->mainMemory[phys_addr], PageSize, pagenum*PageSize);
}


void
BackingStore::PageIn(TranslationEntry *pte)
{
     int pagenum;
     int phys_addr;
     pagenum = space->GetPTEpageNum(pte);
     phys_addr = space->Translate(pagenum*PageSize);
     OpenFile *BSExec = BSFile->Open(filename);
     BSExec->ReadAt(&machine->mainMemory[phys_addr], PageSize, pagenum*PageSize);
}
