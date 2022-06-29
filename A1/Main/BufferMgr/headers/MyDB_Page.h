//
// Created by 王天珏 on 1/25/22.
//

#ifndef A1_MYDB_PAGE_H
#define A1_MYDB_PAGE_H

#include "MyDB_PageHandle.h"
#include "MyDB_BufferManager.h"
#include "MyDB_Table.h"
using namespace std;
class MyDB_Page;
class MyDB_BufferManager;
class MyDB_Page{
public:
    MyDB_Page(MyDB_BufferManager* bm,string storageLoc,size_t offset,MyDB_TablePtr tablePtr);
//    MyDB_Page(MyDB_BufferManager* bm);
    void *getBytes ();

    void increaseRefCnt();
    void decreaseRefCnt();
private:
    MyDB_BufferManager* bm;
    char* mem;
//    bool isPinned;
    bool isDirty;
    bool isAnonymous;
//    bool isInMem;
    string storageLoc;
    size_t offset;
    int refCnt;
    MyDB_TablePtr tablePtr;
    friend class MyDB_BufferManager;
    friend class MyDB_PageHandleBase;
};
#endif //A1_MYDB_PAGE_H
