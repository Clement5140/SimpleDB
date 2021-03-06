#ifndef __TABLE_H__
#define __TABLE_H__

#include "../constants.h"
#include "Compare.h"
#include "Index.h"

extern bool initMode;

struct Check {
    int col;
    int offset;
    OpType op;
    RelType rel;
};

struct ForeignKey {
    unsigned int col;
    unsigned int foreign_table_id;
    unsigned int foreign_col;
};

struct TableHead {
    int8_t columnTot, primaryCount, checkTot, foreignKeyTot;
    int pageTot, recordByte, dataArrUsed;
    unsigned int nextAvail, notNull, hasIndex, isPrimary;

    char columnName[MAX_COLUMN_SIZE][MAX_NAME_LEN];
    int columnOffset[MAX_COLUMN_SIZE];
    ColumnType columnType[MAX_COLUMN_SIZE];
    int columnLen[MAX_COLUMN_SIZE];
    int defaultOffset[MAX_COLUMN_SIZE];
    Check checkList[MAX_CHECK];
    ForeignKey foreignKeyList[MAX_FOREIGN_KEY];
    char dataArr[MAX_DATA_SIZE];
};

class Table {
    friend class Database;

    TableHead head;
    bool ready;
    int fileID, permID;
    char *buf;
    Index colIndex[MAX_COLUMN_SIZE];
    std::string tableName;

    Table();

    ~Table();

    void initTempRecord();

    void allocPage();

    void inverseFooter(const char *page, int idx);

    int getFooter(const char *page, int idx);

    void loadIndex();

    void storeIndex();

    void dropIndex();

    int getFastCmp(RID_t rid, int col);

    bool getIsNull(RID_t rid, int col);

    void eraseColIndex(RID_t rid, int col);

    void insertColIndex(RID_t rid, int col);

    void create(const char *tableName);

    void open(const char *tableName);

    void close();

    void drop();

    std::string genCheckError(int checkId);

    bool checkPrimary();

    std::string checkValueConstraint();

    std::string checkForeignKeyConstraint();

    std::string checkRecord();

public:
    std::string getTableName();

    void printSchema();

    bool hasIndex(int col);

    bool isPrimary(int col);

    RID_t getNext(RID_t rid);

    // return -1 if name exist, columnId otherwise
    // size: maxlen for varchar, outputwidth for int
    int addColumn(const char *name, ColumnType type, int size,
                  bool notNull, bool hasDefault, const char *data);

    void createIndex(int col);

    void dropIndex(int col);

    void setPrimary(int columnID);

    int getColumnCount();

    // return -1 if not found
    int getColumnID(const char *name);

    void clearTempRecord();

    // RelType is used to create Inset operation.
    // Dirty interface.
    // With `relation` RE_OR and same `col`, the result will OR together
    // others will AND together
    // data == 0 if data is null
    // Example:
    //   List: [1, >, 10, AND], [2, ==, 'a', OR], [2, ==, 'b', OR], [3, ==, 'c', OR]
    //   Result: col[1]>10 AND (col[2]=='a' OR col[2]=='b') AND col[3]=='c'
    // Example:
    //   List: [1, >, 10, AND], [2, ==, 'a', OR], [3, ==, 'c', OR], [2, ==, 'b', OR]
    //   Result: col[1]>10 AND col[2]=='a' AND col[3]=='c' AND col[2]=='b'

    void addCheck(int col, OpType op, char *data, RelType relation);

    void addForeignKeyConstraint(unsigned int col, unsigned int foreignTableId, unsigned int foreignColId);

    std::string setTempRecord(int col, const char *data);

    void setTempRecordNull(int col);

    // return value change. Urgly interface.
    // return "" if success.
    // return error description otherwise.
    std::string insertTempRecord();

    void dropRecord(RID_t rid);

    std::string loadRecordToTemp(RID_t rid, char *page, int offset);

    std::string modifyRecord(RID_t rid, int col, char *data);

    std::string modifyRecordNull(RID_t rid, int col);

    int getRecordBytes();

    char *getRecordTempPtr(RID_t rid);

    void getRecord(RID_t rid, char *buf);

    int getColumnOffset(int col);

    ColumnType getColumnType(int col);

    //return 0 when null
    //return value in tempbuf when rid = -1
    char *select(RID_t rid, int col);

    RID_t selectIndexLowerBound(int col, const char *data);

    RID_t selectIndexLowerBoundEqual(int col, const char *data);

    RID_t selectIndexLowerBoundNull(int col);

    RID_t selectIndexNext(int col);

    RID_t selectIndexNextEqual(int col);

    RID_t selectIndexUpperBound(int col, const char *data);

    RID_t selectIndexUpperBoundNull(int col);

    RID_t selectReveredIndexNext(int col);

    char *getColumnName(int col);

};

bool operator<(const IndexKey &a, const IndexKey &b);

#endif
