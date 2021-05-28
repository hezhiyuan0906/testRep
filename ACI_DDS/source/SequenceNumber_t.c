#include "../Include/GlobalDefine.h"

SequenceNumber_t SEQUENCENUMBER_UNKNOWN = { -1, 0 };    // RTPS保留的SequenceNumber_t值
SequenceNumber_t SEQUENCENUMBER_MINIMUM = { -1, 0 };    // 最小的SequenceNumber_t值
SequenceNumber_t SEQUENCENUMBER_MAXIMUM = { 0x7fffffff, 0xffffffff };    // 最小的SequenceNumber_t值
SequenceNumber_t SEQUENCENUMBER_START = { 0, 1 };
SequenceNumber_t SEQUENCENUMBER_ZERO = { 0, 0 };

static SequenceNumber_t SEQUENCENUMBER_PUB_WRITER = { 0, 0 };  /* 创建写入器时seqNum */
static SequenceNumber_t SEQUENCENUMBER_SUB_READER = { 0, 0 };  /* 创建读取器时seqNum */

/* 加减、判断大小函数 */
SequenceNumber_t Add_SequenceNumber(SequenceNumber_t left, SequenceNumber_t right)
{
    unsigned int resultLow = left.low + right.low;
    unsigned int resultHigh = left.high + right.high + (resultLow < left.low ? 1 : 0);
    SequenceNumber_t result = { resultHigh, resultLow };
    return result;
}
SequenceNumber_t Sub_SequenceNumber(SequenceNumber_t left, SequenceNumber_t right)
{
	unsigned int resultLow = (left.low >= right.low) ? (left.low - right.low) : (right.low - left.low);
    int resultHigh = left.low >= right.low ? left.high : left.high - 1;
	resultHigh = (left.high >= right.high) ? (left.high - right.high) : (-(right.high - left.high));
    SequenceNumber_t result = { resultHigh, resultLow };
    return result;
}

SequenceNumber_t GetPubWriterSequenceNumber()
{
    SEQUENCENUMBER_PUB_WRITER = Add_SequenceNumber(SEQUENCENUMBER_PUB_WRITER, SEQUENCENUMBER_START);
    return SEQUENCENUMBER_PUB_WRITER;
}

SequenceNumber_t GetSubReaderSequenceNumber()
{
    SEQUENCENUMBER_SUB_READER = Add_SequenceNumber(SEQUENCENUMBER_SUB_READER, SEQUENCENUMBER_START);
    return SEQUENCENUMBER_SUB_READER;
}