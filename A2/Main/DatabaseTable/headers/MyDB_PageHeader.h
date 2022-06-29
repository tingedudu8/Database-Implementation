//
// Created by mql on 2022/2/6.
//

#ifndef A2_MYDB_PAGEHEADER_H
#define A2_MYDB_PAGEHEADER_H
#include <memory>
struct Header{
    size_t sumOfRecordSize;
    MyDB_PageType pageType;
    char recordBeginAt[0];
};
#endif //A2_MYDB_PAGEHEADER_H
