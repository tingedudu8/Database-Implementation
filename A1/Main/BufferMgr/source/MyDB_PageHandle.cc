
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"
#include "MyDB_Page.h"
void *MyDB_PageHandleBase :: getBytes () {
    return pagePtr->getBytes();
//	return nullptr;
}

void MyDB_PageHandleBase :: wroteBytes () {
    pagePtr->isDirty=true;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
    pagePtr->decreaseRefCnt();
    if(pagePtr->refCnt==0)
    {
        pagePtr->bm->unpinPage(pagePtr);
        if(pagePtr->isAnonymous)
        {
            pagePtr->bm->releasePage(this->pagePtr);
            delete pagePtr;
        }
    }
}
MyDB_PageHandleBase :: MyDB_PageHandleBase(MyDB_Page* page){
    pagePtr=page;
    pagePtr->increaseRefCnt();
}
#endif

