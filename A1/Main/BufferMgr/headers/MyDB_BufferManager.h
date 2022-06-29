
#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

#include "MyDB_Page.h"
#include "MyDB_PageHandle.h"
#include "MyDB_Table.h"
#include<map>
#include<list>
using namespace std;
class MyDB_PageHandleBase;
typedef shared_ptr <MyDB_PageHandleBase> MyDB_PageHandle;
class MyDB_Page;
class MyDB_BufferManager {

public:

	// THESE METHODS MUST APPEAR AND THE PROTOTYPES CANNOT CHANGE!

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle 
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular 
	// table
	MyDB_PageHandle getPage ();

	// gets the i^th page in the table whichTable... the only difference 
	// between this method and getPage (whichTable, i) is that the page will be
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pins the specified page
	void unpin (MyDB_PageHandle unpinMe);

	// creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize 
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile);
	
	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	~MyDB_BufferManager ();

    //LRU functions
    void* putPageInLRU(MyDB_Page* pagePtr);
    void* getPageInLRU(MyDB_Page* pagePtr);
    void pinPage(MyDB_Page* pagePtr);
    void unpinPage(MyDB_Page* pagePtr);
    void releasePage(MyDB_Page* pagePtr);

    // temp storage
    size_t getIdleSpaceInTempStorage();
    void readPageFromDisk(MyDB_Page* pagePtr);
    void writePageToDisk(MyDB_Page* pagePtr);
    // FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS

private:
    //page
    MyDB_PageHandle getExistPage (MyDB_TablePtr whichTable, long i);// null -> not exist
//    MyDB_PageHandle getExistPage ();// null -> not exist
    MyDB_PageHandle createPage(MyDB_TablePtr whichTable, long i);
    MyDB_PageHandle createPage();

    //LRU
    list<pair<char*,MyDB_Page*>> LRUList;
    list<char*> idleList;
    map<MyDB_Page*,list<pair<char*,MyDB_Page*>>::iterator> LRUHashmap;
    map<MyDB_Page*,char*> pinnedPage;

    //private varaiables
    size_t pageSize;
    size_t numPages;
    string tempFile; // for anonymous page
	char* beginAt;

    // temp storage for anonymous page on disk
    list<size_t> idleSpaceInTempFile;
    size_t idleSpaceCnt;

    map<pair<MyDB_TablePtr,long>,MyDB_Page*> lookUpTable;
	// YOUR STUFF HERE

};

#endif


