#include "copyright.h"
#include "bitmap.h"
#include "synch.h"
#include "list.h"
class PCB{
public:  
  PCB();  
  void* object;
};

class ProcessTable{
public:
      ProcessTable(int size);
     
      int Alloc(void *object);
       
      void  *Get(int index);
      
      void Release(int index);
      
      void PageStore(TranslationEntry *pte);

      TranslationEntry * Evict(); 

      PCB *Pcb;
     
      List *FIFOlist;
private:
      BitMap *bitmap;
      Lock *pm_lock;
};


