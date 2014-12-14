#include "copyright.h"
#include "bitmap.h"
#include "synch.h"
#include "list.h"
class VPN;
class MemoryStore;
class MemoryManager{
public:
     MemoryManager(int numPages);

     int AllocPage();

     void FreePage(int physPageNum);

     bool PageIsAllocated(int physPageNum);
     
     int NumPagesCanBeUsed();

private:
     BitMap* bitmap;
     int Num_Pages;
     Lock* MM_lock;
};

class VPN{
public:
   VPN();
   TranslationEntry *pte;
   int Counter;
   BackingStore *bs;
private:

};

class MemoryStore{
public:
    MemoryStore(int phypageNum);
    void Store(int ppn, TranslationEntry* PTE, BackingStore* bstore);
    TranslationEntry* ReturnPTE_FIFO();
    TranslationEntry* ReturnPTE_Rand();
    TranslationEntry* ReturnPTE_LRU();
    BackingStore* ReturnBS(int ppn);
    VPN *Vpn;
    List *FIFOlist;

private:

};
