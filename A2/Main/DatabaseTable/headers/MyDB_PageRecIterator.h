//
// Created by mql on 2022/2/6.
//

#ifndef A2_MYDB_PAGERECITERATOR_H
#define A2_MYDB_PAGERECITERATOR_H
#include "MyDB_RecordIterator.h"
#include "MyDB_PageReaderWriter.h"

class MyDB_PageReaderWriter;
class MyDB_PageRecIterator: public MyDB_RecordIterator{
public:
    MyDB_PageRecIterator(MyDB_PageReaderWriter& parent, MyDB_RecordPtr recordPtr);
    void getNext();
    bool hasNext();

private:
    MyDB_PageHandle handle;
    MyDB_RecordPtr recordPtr;
    size_t offset;
};
#endif //A2_MYDB_PAGERECITERATOR_H
