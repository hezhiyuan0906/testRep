#include "../Include/GlobalDefine.h"

/*
* 保留的Duration_t值
*/
const int DURATION_INFINITE_SEC = 0x7fffffff;    // 无穷秒
const int DURATION_ZERO_SEC = 0x00000000;    // 0秒
const unsigned int DURATION_INFINITE_NSEC = 0xffffffff;// 无穷纳秒
const unsigned int DURATION_ZERO_NSEC = 0x00000000;    // 0纳秒

// 协议用到的一些Duration_t的默认值
// 无效的值
const Duration_t INVALID_DURATION = {-1, 0};

// HB延迟响应时间（发送ACKNACK），100ms
const Duration_t DEFALUT_HB_RESP_DELAY = {0, 100 * 1000 * 1000};

// HB抑制，防止过快地收到HB, 0s
const Duration_t DEFALUT_HB_SUPPRESSION = {0, 100 * 1000 * 1000};

// 写入HistoryCache的最大延迟时间,100ms
const Duration_t DEFALUT_MAX_BLOCKING_TIME = {0, 100 * 1000 * 1000};

// 发送HB的周期
const Duration_t DEFALUT_HB_PERIOD = {0, 1000 * 1000 * 1000};

// ACKNACK的响应延迟时间,200ms
const Duration_t DEFALUT_NACK_RESP_DELAY = {0, 200 * 1000 * 1000};

// NACK抑制，SENT变成Unacknowledged状态(协议有问题)，10ms
const Duration_t DEFALUT_NACK_SUPPRESSION = {0, 100 * 1000 * 1000};

// 发送Participant心跳时间
const Duration_t DEFALUT_UPDATE_PARTICIPANT = {30, 0};

// Participant过期时间,15s
const Duration_t DEFALUT_PARTICIPANT_LEASE = {100, 0};

// ParticipantReader检查更新Participant存活信息的间隔
const Duration_t DEFALUT_CHECK_PARTICIPANT_STALE = {1, 0};

// Writer刷新响应超时周期
const Duration_t DEFALUT_REFRESH_UNACK = {0, 10 * 1000 * 1000};

// 时间常量
const Time_t TIME_ZERO = {0, 0};
const Time_t TIME_INVALID = {-1, 0xffffffff};
const Time_t TIME_INFINITE = {0x7fffffff, 0xffffffff};

/* TODO 添加获取当前时间实现 */
Time_t GetNowTime()
{
    Time_t t = { 0, 0 };
    t.sec = (int)time(NULL);
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	t.nanosec = sys.wMilliseconds * 1000 * 1000;
    return t;
}

/* 加减操作 */
Duration_t Add_Duration(Duration_t left, Duration_t right)
{
    EX_UINT32 resNanosec = left.nanosec + right.nanosec;
    EX_INT32 resSec = left.sec + right.sec;
    if (resNanosec < right.nanosec)
    {
        ++resSec;
    }
    /* 是否超过上限 */
    if (resSec < left.sec)
    {
        return TIME_INFINITE;
    }
    Duration_t result = { resSec, resNanosec };
    return result;
}
Duration_t Sub_Duration(Duration_t left, Duration_t  right)
{
    
    EX_INT32 resSec = left.sec - right.sec;
    EX_UINT32 resNanosec;
    if (left.nanosec >= right.nanosec)
    {
        resNanosec = left.nanosec - right.nanosec;

    }
    else
    {
        resNanosec = 1000000000 - right.nanosec + left.nanosec;
        --resSec;
    }

    /* TODO 判断是否超过下限 */

    Duration_t result = { resSec, resNanosec };
    return result;
}

BOOL Compare_Duration(Duration_t left, Duration_t  right)
{
	if (left.sec > right.sec)
	{
		return TRUE;
	}
	else if (left.sec < right.sec)
	{
		return FALSE;
	}
	else
	{
		if (left.nanosec > right.nanosec)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

BOOL Duration_IsEqual(Duration_t left, Duration_t  right)
{
    if (left.sec == right.sec && left.nanosec == right.nanosec)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}