#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_
#include "memorymanager.h"
#endif
#include "copyright.h"
#include "system.h"
#include "sysdep.h"
class VPN;
class MemoryStore;
MemoryManager::MemoryManager(int numPages)
{
    Num_Pages = numPages;
    bitmap = new BitMap(Num_Pages);
    MM_lock = new Lock("MemoryManager_Lock");

}

int 
MemoryManager::AllocPage(){
    MM_lock->Acquire();
    int i;
    i = bitmap->Find();
    if(i>=0)
    bitmap->Mark(i);
    MM_lock->Release();
    return i;
}

void 
MemoryManager::FreePage(int physPageNum){
     MM_lock->Acquire();
     bitmap->Clear(physPageNum);
     MM_lock->Release();
}

bool 
MemoryManager::PageIsAllocated(int physPageNum){
    int i,p;
    i = physPageNum;
    MM_lock->Acquire();
    p = bitmap->Test(i);
    return p;
    MM_lock->Release();
}

int 
MemoryManager::NumPagesCanBeUsed(){
    int count;
    MM_lock->Acquire();
    count = bitmap->NumClear();
    MM_lock->Release();
    return count;
}

VPN::VPN(){

}


MemoryStore::MemoryStore(int phypageNum){
    Vpn = new VPN[phypageNum];
    FIFOlist = new List;
    
}

void MemoryStore::Store(int ppn, TranslationEntry* PTE, BackingStore* bstore){
     Vpn[ppn].pte = PTE;
     Vpn[ppn].bs = bstore;
     FIFOlist->Append((void*)Vpn[ppn].pte);       
}

TranslationEntry*
MemoryStore::ReturnPTE_FIFO(){
return (TranslationEntry *)FIFOlist->Remove();
}

TranslationEntry*
MemoryStore::ReturnPTE_Rand(){
     int randompage;
     randompage = Random() % NumPhysPages;
     return Vpn[randompage].pte;
}


BackingStore*
MemoryStore::ReturnBS(int ppn){
    return Vpn[ppn].bs;
}



