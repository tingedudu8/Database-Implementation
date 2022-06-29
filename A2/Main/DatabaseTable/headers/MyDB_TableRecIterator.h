//
// Created by mql on 2022/2/6.
//

#ifndef A2_MYDB_TABLERECITERATOR_H
#define A2_MYDB_TABLERECITERATOR_H
#include "MyDB_RecordIterator.h"
#include "MyDB_PageRecIterator.h"
#include "MyDB_TableReaderWriter.h"
class MyDB_TableReaderWriter;
class MyDB_TableRecIterator: public MyDB_RecordIterator{
public:
    MyDB_TableRecIterator(MyDB_TableReaderWriter&,MyDB_RecordPtr recordPtr);
    void getNext();
    bool hasNext();
private:
    MyDB_TableReaderWriter& parent;
    MyDB_RecordIteratorPtr riPtr;
    MyDB_RecordPtr recordPtr;
    size_t pageNum;
};
#endif //A2_MYDB_TABLERECITERATOR_H
