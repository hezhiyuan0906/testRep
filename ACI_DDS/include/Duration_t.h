/*********************************************************************************
*  OpenST Basic tool library
*  Copyright (C) 2019 QiYuan
*
*
*  This program is security software
*  without permission you cannot use or distribute the software
*  otherwise, legal liability will be investigated
*
*  If you want to use the software, please contact the authorities to obtain a license
*  < http://www.iamqy.com.cn >.
*
*  @file     Duration.h
*  @brief    Duration
*  Details.
*
*  @author   Hunter
*  @email    1028251654@qq.com
*  @version  1.0
*  @date     2019/08/08
*  @license
*
*--------------------------------------------------------------------------------
*  Remark         : Description
*--------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>  | <operation> | <Description>
*--------------------------------------------------------------------------------
*  2019/08/08 | 1.0       | Hunter    | Create file |
*--------------------------------------------------------------------------------
*
*********************************************************************************/

#ifndef HIPERD_DURATION_T_H
#define HIPERD_DURATION_T_H

/**
 * 保留的Duration_t值
 */
extern const int            DURATION_INFINITE_SEC   ; /* 无穷秒    */
extern const int            DURATION_ZERO_SEC       ; /* 0秒       */
extern const unsigned int   DURATION_INFINITE_NSEC  ; /* 无穷纳秒  */
extern const unsigned int   DURATION_ZERO_NSEC      ; /* 0纳秒     */

/* Duration_t 结构体序列化成Parameter时占用空间的大小 需要考虑字节对齐*/
#define SIZE_OF_DURATION 8

typedef struct Duration_t
{
    EX_INT32    sec     ; /* 秒的值        */
    EX_UINT32   nanosec ; /* 毫微秒的值    */
}Duration_t;

/* 加减操作和比较宏 */
Duration_t Add_Duration(Duration_t left, Duration_t right);
Duration_t Sub_Duration(Duration_t left, Duration_t right);
BOOL Compare_Duration(Duration_t left, Duration_t  right);
BOOL Duration_IsEqual(Duration_t left, Duration_t  right);

#define ISEQUAL_DURATION(left,right) ((left.sec == right.sec)&&(left.nanosec == right.nanosec))

#define ISLESSTHEN_DURATION(left,right) ((left.sec < right.sec)||(left.sec == right.sec && left.nanosec < right.nanosec))

#define ISEQUAL_OR_LESSTHEN_DURATION(left,right) (ISEQUAL_DURATION(left,right) || ISLESSTHEN_DURATION(left,right))

#define ISGRATERTHEN_DURATION(left,right) (!ISEQUAL_OR_LESSTHEN_DURATION(left,right))

#define ISEQUAL_OR_GRATERTHEN_DURATION(left,right) (!ISLESSTHEN_DURATION(left,right))

/*
 以下为该结构编解码工具宏
*/
#define SERIALIZE_DURATION(duration , memBlock)      \
    do                                               \
    {                                                \
        PUT_INT(memBlock, duration.sec)              \
        PUT_UNSIGNED_INT(memBlock, duration.nanosec) \
    } while (0);

#define DESERIALIZE_DURATION(duration , memBlock)    \
    do                                               \
    {                                                \
        GET_INT(memBlock, duration.sec)              \
        GET_UNSIGNED_INT(memBlock, duration.nanosec) \
    } while (0);


#define DURATION_IS_EQUAL(leftDuration , rightDuration) \
    (## leftDuration ##.sec == ## rightDuration ##.sec) && \
    (## leftDuration ##.nanosec == ## rightDuration ##.nanosec)




// RTPS协议用到的一些Duration_t的默认值
extern const Duration_t INVALID_DURATION                ; /* 无效的值 */
extern const Duration_t DEFALUT_HB_RESP_DELAY           ; /* HB延迟响应时间（发送ACKNACK），100ms */
extern const Duration_t DEFALUT_HB_SUPPRESSION          ; /* HB抑制，防止过快地收到HB, 0s */
extern const Duration_t DEFALUT_MAX_BLOCKING_TIME       ; /* 写入HistoryCache的最大延迟时间,100ms */
extern const Duration_t DEFALUT_HB_PERIOD               ; /* 发送HB的周期 */
extern const Duration_t DEFALUT_NACK_RESP_DELAY         ; /* ACKNACK的响应延迟时间,200ms */
extern const Duration_t DEFALUT_NACK_SUPPRESSION        ; /* NACK抑制，SENT变成Unacknowledged状态(协议有问题)，10ms */
extern const Duration_t DEFALUT_UPDATE_PARTICIPANT      ; /* 发送Participant心跳时间 */
extern const Duration_t DEFALUT_PARTICIPANT_LEASE       ; /* Participant过期时间,15s */
extern const Duration_t DEFALUT_CHECK_PARTICIPANT_STALE ; /* ParticipantReader检查更新Participant存活信息的间隔 */
extern const Duration_t DEFALUT_REFRESH_UNACK           ; /* Writer刷新响应超时周期 */

// 参考IETF NTP定义，time = seconds+[fraction/(2^32)]
typedef Duration_t Time_t;
// 常量
extern const Time_t TIME_ZERO;
extern const Time_t TIME_INVALID;
extern const Time_t TIME_INFINITE;

// 静态的函数，返回调用该函数时的日历时间
Time_t GetNowTime();


#endif