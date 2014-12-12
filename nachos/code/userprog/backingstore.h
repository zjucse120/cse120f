#include "copyright.h"
//#include "translate.h"
#include "filesys.h"
#include "addrspace.h"

class BackingStore{

public:

BackingStore(AddrSpace *as);

void PageOut(TranslationEntry *pte);

void PageIn(TranslationEntry *pte);
     
AddrSpace* space;

private:
       FileSystem* BSFile;
       char filename[16];

};
