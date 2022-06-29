
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include "MyDB_Page.h"
#include "MyDB_PageHandle.h"
#include <string>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
    MyDB_PageHandle existPageHandle=getExistPage(whichTable, i);
    if(existPageHandle)
    {
        existPageHandle->getBytes();
        return existPageHandle;
    }
    MyDB_PageHandle ret=createPage(whichTable,i);
    ret->getBytes();
    return ret;
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    MyDB_PageHandle ret=createPage();
    ret->getBytes();
    return ret;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    MyDB_PageHandle ret=getPage(whichTable,i);
    pinPage(ret->pagePtr);
	return ret;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    MyDB_PageHandle ret=getPage();
    pinPage(ret->pagePtr);
    return ret;
}

MyDB_PageHandle MyDB_BufferManager :: getExistPage (MyDB_TablePtr whichTable, long i)
{
    auto found=lookUpTable.find({whichTable,i});
    if(found==lookUpTable.end())return nullptr;
    MyDB_PageHandle ret= make_shared<MyDB_PageHandleBase>(found->second);
    return ret;
}

MyDB_PageHandle MyDB_BufferManager :: createPage(MyDB_TablePtr whichTable, long i)
{
    MyDB_Page* pagePtr=new MyDB_Page(this,whichTable->getStorageLoc(),i,whichTable);
    MyDB_PageHandle ret= make_shared<MyDB_PageHandleBase>(pagePtr);
    lookUpTable[{whichTable,i}]=pagePtr;
    return ret;
}

MyDB_PageHandle MyDB_BufferManager :: createPage()
{
    MyDB_Page* pagePtr=new MyDB_Page(this,tempFile,getIdleSpaceInTempStorage(), nullptr);
    MyDB_PageHandle ret= make_shared<MyDB_PageHandleBase>(pagePtr);
    return ret;
}

size_t MyDB_BufferManager :: getIdleSpaceInTempStorage()
{
    if(idleSpaceInTempFile.empty())
    {
        idleSpaceInTempFile.push_back(idleSpaceCnt);
        idleSpaceCnt+=1;
    }
    size_t ret=idleSpaceInTempFile.back();
    idleSpaceInTempFile.pop_back();
    return ret;
}
void* MyDB_BufferManager :: getPageInLRU(MyDB_Page* pagePtr)
{
    // search in pinned pages
    auto foundPin = pinnedPage.find(pagePtr);
    if(foundPin!=pinnedPage.end())
    {
        return foundPin->second;
    }
    //search in LRU
    auto found=LRUHashmap.find(pagePtr);
    if(found==LRUHashmap.end())
    {
        return putPageInLRU(pagePtr);
    }
    // already in LRU
    auto oldNodeIter=found->second;
    pair<char*,MyDB_Page*> newNode=*oldNodeIter;
    LRUList.erase(oldNodeIter);
    LRUList.push_front(newNode);
    LRUHashmap[pagePtr]=LRUList.begin();
    return newNode.first;
}
void* MyDB_BufferManager :: putPageInLRU(MyDB_Page* pagePtr)
{
    if(idleList.empty())
    {
        auto node=prev(LRUList.end()); // assert LRUList.size()>=1
        char* mem=node->first;
        MyDB_Page* evictPage=node->second;

        //evict page
        writePageToDisk(evictPage);
        LRUList.erase(node);
        LRUHashmap.erase(evictPage);
        if(evictPage->refCnt==0)
        {
            // assert(evictPage->isAnonymous==false);
            lookUpTable.erase({evictPage->tablePtr,evictPage->offset});
            delete evictPage;
        }

        LRUList.push_front({mem,pagePtr});
        pagePtr->mem=mem;
        LRUHashmap[pagePtr]=LRUList.begin();
    }
    else
    {
        auto node=idleList.begin();
        LRUList.push_front({*node,pagePtr});
        pagePtr->mem=*node;
        LRUHashmap[pagePtr]=LRUList.begin();
        idleList.erase(node);
    }
    readPageFromDisk(pagePtr);
    return LRUList.begin()->first;
}

void MyDB_BufferManager :: readPageFromDisk(MyDB_Page* pagePtr)
{
    int fd=open(pagePtr->storageLoc.c_str(), O_CREAT|O_FSYNC|O_RDWR,0666);
    lseek(fd,pagePtr->offset*pageSize,SEEK_SET);
    read(fd,pagePtr->mem,pageSize);
    close(fd);
}

void MyDB_BufferManager :: writePageToDisk(MyDB_Page* pagePtr)
{
    if(!pagePtr->isDirty)return;
    int fd=open(pagePtr->storageLoc.c_str(), O_CREAT|O_FSYNC|O_RDWR,0666);
    lseek(fd,pagePtr->offset*pageSize,SEEK_SET);
    write(fd,pagePtr->mem,pageSize);
    close(fd);
}

void MyDB_BufferManager :: pinPage(MyDB_Page* pagePtr)
// assert this page is in LRU
{
    if(pinnedPage.find(pagePtr)!=pinnedPage.end())return;
    auto found=LRUHashmap.find(pagePtr);
//    assert(found!=LRUHashmap.end());
    pinnedPage[pagePtr]=found->second->first;
    LRUList.erase(found->second);
    LRUHashmap.erase(found);
}

void MyDB_BufferManager :: unpinPage(MyDB_Page* pagePtr)
{
    auto found=pinnedPage.find(pagePtr);
    if(found==pinnedPage.end())return;
//    assert(found!=pinnedPage.end());
    LRUList.push_front({found->second,found->first});
    LRUHashmap[pagePtr]=LRUList.begin();
    pinnedPage.erase(found);
}

void MyDB_BufferManager :: releasePage(MyDB_Page* pagePtr)
{
    // assert(pagePtr->isAnonymous);
    idleSpaceInTempFile.push_back(pagePtr->offset);
    auto found=LRUHashmap.find(pagePtr);
    if(found==LRUHashmap.end())return;
    idleList.push_back(found->second->first);
    LRUList.erase(found->second);
    LRUHashmap.erase(found);
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    unpinPage(unpinMe->pagePtr);
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    this->pageSize=pageSize;
    this->numPages=numPages;
    this->tempFile=tempFile;
    size_t memSize=pageSize*numPages;
    char* mem=(char*)malloc(memSize);
    beginAt=mem;
    for(size_t i=0;i<numPages;++i)
        idleList.push_back(mem+pageSize*i);
    idleSpaceCnt=0;
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
    for(auto p:LRUList)
    {
        writePageToDisk(p.second);
        delete p.second;
    }
    for(auto p:pinnedPage)
    {
        writePageToDisk(p.first);
        delete p.first;
    }
    free(beginAt);
    beginAt=nullptr;
}
	
#endif


