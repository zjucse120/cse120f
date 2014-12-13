// addrspace.h
//	Data structures to keep track of executing user programs
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "noff.h"
#include "stats.h"

#define UserStackSize		1024 	// increase this as necessary!

extern Statistics *stats;
class BackingStore;

class AddrSpace {
public:
    int Initialize(OpenFile *executable);
    bool DemandSpace(OpenFile *executable, int badvpn);
    OpenFile *Executable;
    NoffHeader noffH;
    void MarkPage(int badvpn); 
     AddrSpace(OpenFile *executable);	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space
	
    int Translate(int virtaddr);
    bool ReadMem(int addr, int size, int *value);
    void InitRegisters();		// Initialize user-level CPU registers,
    // before jumping to user code
    unsigned int numberPages() {return numPages;}
    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch
    unsigned int numPages;		// Number of pages in the virtual
    //unsigned int size;
    // address space
   // NoffHeader noffH;
    TranslationEntry* GetPTE(int pageNum){
	return &pageTable[pageNum];}
    int GetPTEpageNum(TranslationEntry *PTE);
    int GetpageNum();
    void newPageTable();
    void Evict();
private:
    TranslationEntry *pageTable;	// Assume linear page table translation
    // for now!
    TranslationEntry *newTable;
    BackingStore *bstore;
 
};


class BackingStore{

public:

BackingStore(AddrSpace *as);

void PageOut(TranslationEntry *pte);

void PageIn(TranslationEntry *pte);
     
AddrSpace* space;

private:
       FileSystem* BSFile;
       char filename[16];
       OpenFile *BSExec;

};



#endif // ADDRSPACE_H
