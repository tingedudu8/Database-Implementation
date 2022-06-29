//
// Created by mql on 2022/2/6.
//
#include "MyDB_PageRecIterator.h"
MyDB_PageRecIterator:: MyDB_PageRecIterator(MyDB_PageReaderWriter& parent, MyDB_RecordPtr recordPtr)
{
    handle=parent.handle;
    this->recordPtr=recordPtr;
    offset=0;
}
void MyDB_PageRecIterator::getNext ()
{
    char* page=(char*)handle->getBytes();
    Header* header=(Header*)page;
    char* target=header->recordBeginAt+offset;
    recordPtr->fromBinary(target);
    offset+=recordPtr->getBinarySize();
}
bool MyDB_PageRecIterator::hasNext()
{
    char* page=(char*)handle->getBytes();
    Header* header=(Header*)page;
    return offset<header->sumOfRecordSize;
}