#ifndef PROCESSMANAGER_H_
#define PROCESSMANAGER_H_
#include "processmanager.h"
#endif 
#include "copyright.h"
PCB::PCB(){
  void* object=(void*)(new int);
}

ProcessTable::ProcessTable(int size){
     bitmap = new BitMap(size);
     Pcb = new PCB[size];
     pm_lock = new Lock("pm lock");
     FIFOlist = new List;

}

int
ProcessTable::Alloc(void *object){
     pm_lock->Acquire();
     int pid = bitmap->Find() + 1;
     Pcb[pid].object = object;
     pm_lock->Release();
     return pid;
}

void* 
ProcessTable::Get(int index){
     void *pcb;
     pm_lock->Acquire();
     pcb = Pcb[index].object;
     pm_lock->Release();
     return pcb;
}

void 
ProcessTable::Release(int index){
      pm_lock->Acquire();
      bitmap->Clear(index-1);
      pm_lock->Release();
}
void
ProcessTable::PageStore(TranslationEntry *pte)
{
//DEBUG('t', "Putting thread %s on ready list.\n", thread->getName());
FIFOlist->Append((void *)pte);
}

TranslationEntry *
ProcessTable::Evict()
{
return (TranslationEntry *)FIFOlist->Remove();
}

/* PROCESSMANAGER_H_*/
