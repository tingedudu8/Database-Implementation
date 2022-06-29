//
// Created by mql on 2022/2/6.
//

#include "MyDB_TableRecIterator.h"

MyDB_TableRecIterator ::MyDB_TableRecIterator(MyDB_TableReaderWriter& parent,MyDB_RecordPtr recordPtr):parent(parent)
{
    this->recordPtr=recordPtr;
    MyDB_PageReaderWriter prw=parent[0];
    riPtr=prw.getIterator(this->recordPtr);
    pageNum=0;
}

bool MyDB_TableRecIterator ::hasNext()
{
    while(!riPtr->hasNext())
    {
        ++pageNum;
        if(pageNum>parent.tablePtr->lastPage())return false;
        riPtr=parent[pageNum].getIterator(recordPtr);
    }
    return true;
}

void MyDB_TableRecIterator::getNext()
{
    while(!riPtr->hasNext())
    {
        ++pageNum;
        riPtr=parent[pageNum].getIterator(recordPtr);
    }
    riPtr->getNext();
}