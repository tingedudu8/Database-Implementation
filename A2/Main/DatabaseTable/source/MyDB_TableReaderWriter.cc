
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_PageHeader.h"
#include <fcntl.h>
using namespace std;

void MyDB_TableReaderWriter :: initializePage(MyDB_TablePtr tablePtr,long offset)
{
    MyDB_PageHandle handle=bmPtr->getPage(tablePtr,offset);
    Header* header=(Header*)handle->getBytes();
    header->sumOfRecordSize=0;
    header->pageType=MyDB_PageType::RegularPage;
    handle->wroteBytes();
}

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
    tablePtr=forMe;
    bmPtr=myBuffer;
    int lastPage=tablePtr->lastPage();
    if(lastPage==-1)
    {
        tablePtr->setLastPage(0);
        initializePage(tablePtr,0);
    }
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t offset) {
    //assert(pageNum<tablePtr->lastPage());
    int lastPage=tablePtr->lastPage();
    if(offset > lastPage)
    {
        for(int i=lastPage+1; i <= offset; ++i)
            initializePage(tablePtr,i);
        tablePtr->setLastPage(offset);
    }
    MyDB_PageReaderWriter ret(bmPtr->getPage(tablePtr, offset),bmPtr);
	return ret;
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
    MyDB_RecordPtr ret(new MyDB_Record(tablePtr->getSchema()));
	return ret;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
    MyDB_PageReaderWriter ret(bmPtr->getPage(tablePtr, tablePtr->lastPage()),bmPtr);
	return ret;
}


void MyDB_TableReaderWriter :: append (MyDB_RecordPtr recordPtr) {
    int lastPage=tablePtr->lastPage();
    MyDB_PageReaderWriter prw(bmPtr->getPage(tablePtr, lastPage),bmPtr);
    if(!prw.append(recordPtr))
    {
        ++lastPage;
        tablePtr->setLastPage(lastPage);
        initializePage(tablePtr,lastPage);
        MyDB_PageReaderWriter prw1(bmPtr->getPage(tablePtr, lastPage),bmPtr);
        // bool res=prw1.append(recordPtr);
        //assert(res);
        prw1.append(recordPtr);
    }
}

void MyDB_TableReaderWriter :: loadFromTextFile (string filename) {
    ifstream inputFile(filename);
    string line;
    MyDB_RecordPtr recordPtr=getEmptyRecord ();
    while(getline(inputFile,line))
    {
        recordPtr->fromString(line);
        append(recordPtr);
    }
    inputFile.close();
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr recordPtr) {
    MyDB_RecordIteratorPtr ret(new MyDB_TableRecIterator(*this,recordPtr));
	return ret;
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string filename) {
    ofstream outputFile(filename);

    MyDB_RecordPtr recordPtr=getEmptyRecord();
    MyDB_RecordIteratorPtr riPtr= getIterator(recordPtr);
    while(riPtr->hasNext())
    {
        riPtr->getNext();
        outputFile<<recordPtr<<"\n";
    }
    outputFile.close();
}

#endif

