//
// Created by 王天珏 on 1/25/22.
//

#ifndef PAGE_C
#define PAGE_C

#include "MyDB_Page.h"
#include <memory>
void* MyDB_Page :: getBytes ()
{
    return bm->getPageInLRU(this);
}

void MyDB_Page :: increaseRefCnt()
{
    ++refCnt;
}
void MyDB_Page :: decreaseRefCnt()
{
    --refCnt;
}

MyDB_Page :: MyDB_Page(MyDB_BufferManager* bm,string storageLoc,size_t offset,MyDB_TablePtr tablePtr)
{
    this->bm=bm;
    this->storageLoc=storageLoc;
    this->offset=offset;
    this->tablePtr=tablePtr;
    if(tablePtr)isAnonymous=false;
    else isAnonymous=true;
    //        mem=(char*)getBytes();
    //        isInMem=true;
    isDirty=false;
    refCnt=0;
    mem= nullptr;
}

#endif