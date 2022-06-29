
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageHandle.h"
void MyDB_PageReaderWriter :: clear () {
    Header* header=(Header*)handle->getBytes();
    header->sumOfRecordSize=0;
    header->pageType=MyDB_PageType::RegularPage;
    handle->wroteBytes();
}

MyDB_PageType MyDB_PageReaderWriter :: getType () {
    Header* header=(Header*)handle->getBytes();
	return header->pageType;
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr recordPtr) {
    MyDB_RecordIteratorPtr ret(new MyDB_PageRecIterator(*this,recordPtr));
	return ret;
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType type) {
    Header* header=(Header*)handle->getBytes();
    header->pageType=type;
    handle->wroteBytes();
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr recordPtr) {
    int size=recordPtr->getBinarySize();
    Header* header=(Header*)handle->getBytes();
    int resultSize=header->recordBeginAt+header->sumOfRecordSize+size-(char*)header;
    if(resultSize>pageSize)return false;
    char* writeTo=header->recordBeginAt+header->sumOfRecordSize;
    recordPtr->toBinary(writeTo);
    header->sumOfRecordSize+=size;
    handle->wroteBytes();
	return true;
}

MyDB_PageReaderWriter ::MyDB_PageReaderWriter(MyDB_PageHandle handle, MyDB_BufferManagerPtr bmPtr)
{
    this->handle=handle;
    pageSize=bmPtr->getPageSize();
//        header=(Header*)handle->getBytes();
}

#endif
