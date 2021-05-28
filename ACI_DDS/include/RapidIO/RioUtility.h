#pragma once
/**
*
* Copyright (c) . All rights reserved.
*
* 说明：
*   Tsi721 RapidIO接口封装
*
*/
/************************************************************************/
/*						LINUX_RIO_REGS                                  */
/************************************************************************/
/*
* In RapidIO, each device has a 16MB configuration space that is
* accessed via maintenance transactions.  Portions of configuration
* space are standardized and/or reserved.
*/
#define RIO_MAINT_SPACE_SZ	0x1000000 /* 16MB of RapidIO mainenance space */

#define RIO_DEV_ID_CAR		0x00	/* [I] Device Identity CAR */
#define RIO_DEV_INFO_CAR	0x04	/* [I] Device Information CAR */
#define RIO_ASM_ID_CAR		0x08	/* [I] Assembly Identity CAR */
#define  RIO_ASM_ID_MASK		0xffff0000	/* [I] Asm ID Mask */
#define  RIO_ASM_VEN_ID_MASK		0x0000ffff	/* [I] Asm Vend Mask */

#define RIO_ASM_INFO_CAR	0x0c	/* [I] Assembly Information CAR */
#define  RIO_ASM_REV_MASK		0xffff0000	/* [I] Asm Rev Mask */
#define  RIO_EXT_FTR_PTR_MASK		0x0000ffff	/* [I] EF_PTR Mask */

#define RIO_PEF_CAR		0x10	/* [I] Processing Element Features CAR */
#define  RIO_PEF_BRIDGE			0x80000000	/* [I] Bridge */
#define  RIO_PEF_MEMORY			0x40000000	/* [I] MMIO */
#define  RIO_PEF_PROCESSOR		0x20000000	/* [I] Processor */
#define  RIO_PEF_SWITCH			0x10000000	/* [I] Switch */
#define  RIO_PEF_MULTIPORT		0x08000000	/* [VI, 2.1] Multiport */
#define  RIO_PEF_INB_MBOX		0x00f00000	/* [II, <= 1.2] Mailboxes */
#define  RIO_PEF_INB_MBOX0		0x00800000	/* [II, <= 1.2] Mailbox 0 */
#define  RIO_PEF_INB_MBOX1		0x00400000	/* [II, <= 1.2] Mailbox 1 */
#define  RIO_PEF_INB_MBOX2		0x00200000	/* [II, <= 1.2] Mailbox 2 */
#define  RIO_PEF_INB_MBOX3		0x00100000	/* [II, <= 1.2] Mailbox 3 */
#define  RIO_PEF_INB_DOORBELL		0x00080000	/* [II, <= 1.2] Doorbells */
#define  RIO_PEF_EXT_RT			0x00000200	/* [III, 1.3] Extended route table support */
#define  RIO_PEF_STD_RT			0x00000100	/* [III, 1.3] Standard route table support */
#define  RIO_PEF_CTLS			0x00000010	/* [III] CTLS */
#define  RIO_PEF_EXT_FEATURES		0x00000008	/* [I] EFT_PTR valid */
#define  RIO_PEF_ADDR_66		0x00000004	/* [I] 66 bits */
#define  RIO_PEF_ADDR_50		0x00000002	/* [I] 50 bits */
#define  RIO_PEF_ADDR_34		0x00000001	/* [I] 34 bits */

#define RIO_SWP_INFO_CAR	0x14	/* [I] Switch Port Information CAR */
#define  RIO_SWP_INFO_PORT_TOTAL_MASK	0x0000ff00	/* [I] Total number of ports */
#define  RIO_SWP_INFO_PORT_NUM_MASK	0x000000ff	/* [I] Maintenance transaction port number */
#define  RIO_GET_TOTAL_PORTS(x)		((x & RIO_SWP_INFO_PORT_TOTAL_MASK) >> 8)
#define  RIO_GET_PORT_NUM(x)		(x & RIO_SWP_INFO_PORT_NUM_MASK)

#define RIO_SRC_OPS_CAR		0x18	/* [I] Source Operations CAR */
#define  RIO_SRC_OPS_READ		0x00008000	/* [I] Read op */
#define  RIO_SRC_OPS_WRITE		0x00004000	/* [I] Write op */
#define  RIO_SRC_OPS_STREAM_WRITE	0x00002000	/* [I] Str-write op */
#define  RIO_SRC_OPS_WRITE_RESPONSE	0x00001000	/* [I] Write/resp op */
#define  RIO_SRC_OPS_DATA_MSG		0x00000800	/* [II] Data msg op */
#define  RIO_SRC_OPS_DOORBELL		0x00000400	/* [II] Doorbell op */
#define  RIO_SRC_OPS_ATOMIC_TST_SWP	0x00000100	/* [I] Atomic TAS op */
#define  RIO_SRC_OPS_ATOMIC_INC		0x00000080	/* [I] Atomic inc op */
#define  RIO_SRC_OPS_ATOMIC_DEC		0x00000040	/* [I] Atomic dec op */
#define  RIO_SRC_OPS_ATOMIC_SET		0x00000020	/* [I] Atomic set op */
#define  RIO_SRC_OPS_ATOMIC_CLR		0x00000010	/* [I] Atomic clr op */
#define  RIO_SRC_OPS_PORT_WRITE		0x00000004	/* [I] Port-write op */

#define RIO_DST_OPS_CAR		0x1c	/* Destination Operations CAR */
#define  RIO_DST_OPS_READ		0x00008000	/* [I] Read op */
#define  RIO_DST_OPS_WRITE		0x00004000	/* [I] Write op */
#define  RIO_DST_OPS_STREAM_WRITE	0x00002000	/* [I] Str-write op */
#define  RIO_DST_OPS_WRITE_RESPONSE	0x00001000	/* [I] Write/resp op */
#define  RIO_DST_OPS_DATA_MSG		0x00000800	/* [II] Data msg op */
#define  RIO_DST_OPS_DOORBELL		0x00000400	/* [II] Doorbell op */
#define  RIO_DST_OPS_ATOMIC_TST_SWP	0x00000100	/* [I] Atomic TAS op */
#define  RIO_DST_OPS_ATOMIC_INC		0x00000080	/* [I] Atomic inc op */
#define  RIO_DST_OPS_ATOMIC_DEC		0x00000040	/* [I] Atomic dec op */
#define  RIO_DST_OPS_ATOMIC_SET		0x00000020	/* [I] Atomic set op */
#define  RIO_DST_OPS_ATOMIC_CLR		0x00000010	/* [I] Atomic clr op */
#define  RIO_DST_OPS_PORT_WRITE		0x00000004	/* [I] Port-write op */

#define  RIO_OPS_READ			0x00008000	/* [I] Read op */
#define  RIO_OPS_WRITE			0x00004000	/* [I] Write op */
#define  RIO_OPS_STREAM_WRITE		0x00002000	/* [I] Str-write op */
#define  RIO_OPS_WRITE_RESPONSE		0x00001000	/* [I] Write/resp op */
#define  RIO_OPS_DATA_MSG		0x00000800	/* [II] Data msg op */
#define  RIO_OPS_DOORBELL		0x00000400	/* [II] Doorbell op */
#define  RIO_OPS_ATOMIC_TST_SWP		0x00000100	/* [I] Atomic TAS op */
#define  RIO_OPS_ATOMIC_INC		0x00000080	/* [I] Atomic inc op */
#define  RIO_OPS_ATOMIC_DEC		0x00000040	/* [I] Atomic dec op */
#define  RIO_OPS_ATOMIC_SET		0x00000020	/* [I] Atomic set op */
#define  RIO_OPS_ATOMIC_CLR		0x00000010	/* [I] Atomic clr op */
#define  RIO_OPS_PORT_WRITE		0x00000004	/* [I] Port-write op */

/* 0x20-0x30 *//* Reserved */

#define	RIO_SWITCH_RT_LIMIT	0x34	/* [III, 1.3] Switch Route Table Destination ID Limit CAR */
#define	 RIO_RT_MAX_DESTID		0x0000ffff

#define RIO_MBOX_CSR		0x40	/* [II, <= 1.2] Mailbox CSR */
#define  RIO_MBOX0_AVAIL		0x80000000	/* [II] Mbox 0 avail */
#define  RIO_MBOX0_FULL			0x40000000	/* [II] Mbox 0 full */
#define  RIO_MBOX0_EMPTY		0x20000000	/* [II] Mbox 0 empty */
#define  RIO_MBOX0_BUSY			0x10000000	/* [II] Mbox 0 busy */
#define  RIO_MBOX0_FAIL			0x08000000	/* [II] Mbox 0 fail */
#define  RIO_MBOX0_ERROR		0x04000000	/* [II] Mbox 0 error */
#define  RIO_MBOX1_AVAIL		0x00800000	/* [II] Mbox 1 avail */
#define  RIO_MBOX1_FULL			0x00200000	/* [II] Mbox 1 full */
#define  RIO_MBOX1_EMPTY		0x00200000	/* [II] Mbox 1 empty */
#define  RIO_MBOX1_BUSY			0x00100000	/* [II] Mbox 1 busy */
#define  RIO_MBOX1_FAIL			0x00080000	/* [II] Mbox 1 fail */
#define  RIO_MBOX1_ERROR		0x00040000	/* [II] Mbox 1 error */
#define  RIO_MBOX2_AVAIL		0x00008000	/* [II] Mbox 2 avail */
#define  RIO_MBOX2_FULL			0x00004000	/* [II] Mbox 2 full */
#define  RIO_MBOX2_EMPTY		0x00002000	/* [II] Mbox 2 empty */
#define  RIO_MBOX2_BUSY			0x00001000	/* [II] Mbox 2 busy */
#define  RIO_MBOX2_FAIL			0x00000800	/* [II] Mbox 2 fail */
#define  RIO_MBOX2_ERROR		0x00000400	/* [II] Mbox 2 error */
#define  RIO_MBOX3_AVAIL		0x00000080	/* [II] Mbox 3 avail */
#define  RIO_MBOX3_FULL			0x00000040	/* [II] Mbox 3 full */
#define  RIO_MBOX3_EMPTY		0x00000020	/* [II] Mbox 3 empty */
#define  RIO_MBOX3_BUSY			0x00000010	/* [II] Mbox 3 busy */
#define  RIO_MBOX3_FAIL			0x00000008	/* [II] Mbox 3 fail */
#define  RIO_MBOX3_ERROR		0x00000004	/* [II] Mbox 3 error */

#define RIO_WRITE_PORT_CSR	0x44	/* [I, <= 1.2] Write Port CSR */
#define RIO_DOORBELL_CSR	0x44	/* [II, <= 1.2] Doorbell CSR */
#define  RIO_DOORBELL_AVAIL		0x80000000	/* [II] Doorbell avail */
#define  RIO_DOORBELL_FULL		0x40000000	/* [II] Doorbell full */
#define  RIO_DOORBELL_EMPTY		0x20000000	/* [II] Doorbell empty */
#define  RIO_DOORBELL_BUSY		0x10000000	/* [II] Doorbell busy */
#define  RIO_DOORBELL_FAILED		0x08000000	/* [II] Doorbell failed */
#define  RIO_DOORBELL_ERROR		0x04000000	/* [II] Doorbell error */
#define  RIO_WRITE_PORT_AVAILABLE	0x00000080	/* [I] Write Port Available */
#define  RIO_WRITE_PORT_FULL		0x00000040	/* [I] Write Port Full */
#define  RIO_WRITE_PORT_EMPTY		0x00000020	/* [I] Write Port Empty */
#define  RIO_WRITE_PORT_BUSY		0x00000010	/* [I] Write Port Busy */
#define  RIO_WRITE_PORT_FAILED		0x00000008	/* [I] Write Port Failed */
#define  RIO_WRITE_PORT_ERROR		0x00000004	/* [I] Write Port Error */

/* 0x48 *//* Reserved */

#define RIO_PELL_CTRL_CSR	0x4c	/* [I] PE Logical Layer Control CSR */
#define   RIO_PELL_ADDR_66		0x00000004	/* [I] 66-bit addr */
#define   RIO_PELL_ADDR_50		0x00000002	/* [I] 50-bit addr */
#define   RIO_PELL_ADDR_34		0x00000001	/* [I] 34-bit addr */

/* 0x50-0x54 *//* Reserved */

#define RIO_LCSH_BA		0x58	/* [I] LCS High Base Address */
#define RIO_LCSL_BA		0x5c	/* [I] LCS Base Address */

#define RIO_DID_CSR		0x60	/* [III] Base Device ID CSR */

/* 0x64 *//* Reserved */

#define RIO_HOST_DID_LOCK_CSR	0x68	/* [III] Host Base Device ID Lock CSR */
#define RIO_COMPONENT_TAG_CSR	0x6c	/* [III] Component Tag CSR */

#define RIO_STD_RTE_CONF_DESTID_SEL_CSR	0x70
#define  RIO_STD_RTE_CONF_EXTCFGEN		0x80000000
#define RIO_STD_RTE_CONF_PORT_SEL_CSR	0x74
#define RIO_STD_RTE_DEFAULT_PORT	0x78

/* 0x7c-0xf8 *//* Reserved */
/* 0x100-0xfff8 *//* [I] Extended Features Space */
/* 0x10000-0xfffff8 *//* [I] Implementation-defined Space */

/*
* Extended Features Space is a configuration space area where
* functionality is mapped into extended feature blocks via a
* singly linked list of extended feature pointers (EFT_PTR).
*
* Each extended feature block can be identified/located in
* Extended Features Space by walking the extended feature
* list starting with the Extended Feature Pointer located
* in the Assembly Information CAR.
*
* Extended Feature Blocks (EFBs) are identified with an assigned
* EFB ID. Extended feature block offsets in the definitions are
* relative to the offset of the EFB within the  Extended Features
* Space.
*/

/* Helper macros to parse the Extended Feature Block header */
#define RIO_EFB_PTR_MASK	0xffff0000
#define RIO_EFB_ID_MASK		0x0000ffff
#define RIO_GET_BLOCK_PTR(x)	((x & RIO_EFB_PTR_MASK) >> 16)
#define RIO_GET_BLOCK_ID(x)	(x & RIO_EFB_ID_MASK)

/* Extended Feature Block IDs */
#define RIO_EFB_PAR_EP_ID	0x0001	/* [IV] LP/LVDS EP Devices */
#define RIO_EFB_PAR_EP_REC_ID	0x0002	/* [IV] LP/LVDS EP Recovery Devices */
#define RIO_EFB_PAR_EP_FREE_ID	0x0003	/* [IV] LP/LVDS EP Free Devices */
#define RIO_EFB_SER_EP_ID_V13P	0x0001	/* [VI] LP/Serial EP Devices, RapidIO Spec ver 1.3 and above */
#define RIO_EFB_SER_EP_REC_ID_V13P	0x0002	/* [VI] LP/Serial EP Recovery Devices, RapidIO Spec ver 1.3 and above */
#define RIO_EFB_SER_EP_FREE_ID_V13P	0x0003	/* [VI] LP/Serial EP Free Devices, RapidIO Spec ver 1.3 and above */
#define RIO_EFB_SER_EP_ID	0x0004	/* [VI] LP/Serial EP Devices */
#define RIO_EFB_SER_EP_REC_ID	0x0005	/* [VI] LP/Serial EP Recovery Devices */
#define RIO_EFB_SER_EP_FREE_ID	0x0006	/* [VI] LP/Serial EP Free Devices */
#define RIO_EFB_SER_EP_FREC_ID	0x0009  /* [VI] LP/Serial EP Free Recovery Devices */
#define RIO_EFB_ERR_MGMNT	0x0007  /* [VIII] Error Management Extensions */

/*
* Physical 8/16 LP-LVDS
* ID=0x0001, Generic End Point Devices
* ID=0x0002, Generic End Point Devices, software assisted recovery option
* ID=0x0003, Generic End Point Free Devices
*
* Physical LP-Serial
* ID=0x0004, Generic End Point Devices
* ID=0x0005, Generic End Point Devices, software assisted recovery option
* ID=0x0006, Generic End Point Free Devices
*/
#define RIO_PORT_MNT_HEADER		0x0000
#define RIO_PORT_REQ_CTL_CSR		0x0020
#define RIO_PORT_RSP_CTL_CSR		0x0024	/* 0x0001/0x0002 */
#define RIO_PORT_LINKTO_CTL_CSR		0x0020	/* Serial */
#define RIO_PORT_RSPTO_CTL_CSR		0x0024	/* Serial */
#define RIO_PORT_GEN_CTL_CSR		0x003c
#define  RIO_PORT_GEN_HOST		0x80000000
#define  RIO_PORT_GEN_MASTER		0x40000000
#define  RIO_PORT_GEN_DISCOVERED	0x20000000
#define RIO_PORT_N_MNT_REQ_CSR(x)	(0x0040 + x*0x20)	/* 0x0002 */
#define  RIO_MNT_REQ_CMD_RD		0x03	/* Reset-device command */
#define  RIO_MNT_REQ_CMD_IS		0x04	/* Input-status command */
#define RIO_PORT_N_MNT_RSP_CSR(x)	(0x0044 + x*0x20)	/* 0x0002 */
#define  RIO_PORT_N_MNT_RSP_RVAL	0x80000000 /* Response Valid */
#define  RIO_PORT_N_MNT_RSP_ASTAT	0x000007e0 /* ackID Status */
#define  RIO_PORT_N_MNT_RSP_LSTAT	0x0000001f /* Link Status */
#define RIO_PORT_N_ACK_STS_CSR(x)	(0x0048 + x*0x20)	/* 0x0002 */
#define  RIO_PORT_N_ACK_CLEAR		0x80000000
#define  RIO_PORT_N_ACK_INBOUND		0x3f000000
#define  RIO_PORT_N_ACK_OUTSTAND	0x00003f00
#define  RIO_PORT_N_ACK_OUTBOUND	0x0000003f
#define RIO_PORT_N_ERR_STS_CSR(x)	(0x0058 + x*0x20)
#define  RIO_PORT_N_ERR_STS_PW_OUT_ES	0x00010000 /* Output Error-stopped */
#define  RIO_PORT_N_ERR_STS_PW_INP_ES	0x00000100 /* Input Error-stopped */
#define  RIO_PORT_N_ERR_STS_PW_PEND	0x00000010 /* Port-Write Pending */
#define  RIO_PORT_N_ERR_STS_PORT_ERR	0x00000004
#define  RIO_PORT_N_ERR_STS_PORT_OK	0x00000002
#define  RIO_PORT_N_ERR_STS_PORT_UNINIT	0x00000001
#define RIO_PORT_N_CTL_CSR(x)		(0x005c + x*0x20)
#define  RIO_PORT_N_CTL_PWIDTH		0xc0000000
#define  RIO_PORT_N_CTL_PWIDTH_1	0x00000000
#define  RIO_PORT_N_CTL_PWIDTH_4	0x40000000
#define  RIO_PORT_N_CTL_P_TYP_SER	0x00000001
#define  RIO_PORT_N_CTL_LOCKOUT		0x00000002
#define  RIO_PORT_N_CTL_EN_RX_SER	0x00200000
#define  RIO_PORT_N_CTL_EN_TX_SER	0x00400000
#define  RIO_PORT_N_CTL_EN_RX_PAR	0x08000000
#define  RIO_PORT_N_CTL_EN_TX_PAR	0x40000000

/*
* Error Management Extensions (RapidIO 1.3+, Part 8)
*
* Extended Features Block ID=0x0007
*/

/* General EM Registers (Common for all Ports) */

#define RIO_EM_EFB_HEADER	0x000	/* Error Management Extensions Block Header */
#define RIO_EM_LTL_ERR_DETECT	0x008	/* Logical/Transport Layer Error Detect CSR */
#define RIO_EM_LTL_ERR_EN	0x00c	/* Logical/Transport Layer Error Enable CSR */
#define  REM_LTL_ERR_ILLTRAN		0x08000000 /* Illegal Transaction decode */
#define  REM_LTL_ERR_UNSOLR		0x00800000 /* Unsolicited Response */
#define  REM_LTL_ERR_UNSUPTR		0x00400000 /* Unsupported Transaction */
#define  REM_LTL_ERR_IMPSPEC		0x000000ff /* Implementation Specific */
#define RIO_EM_LTL_HIADDR_CAP	0x010	/* Logical/Transport Layer High Address Capture CSR */
#define RIO_EM_LTL_ADDR_CAP	0x014	/* Logical/Transport Layer Address Capture CSR */
#define RIO_EM_LTL_DEVID_CAP	0x018	/* Logical/Transport Layer Device ID Capture CSR */
#define RIO_EM_LTL_CTRL_CAP	0x01c	/* Logical/Transport Layer Control Capture CSR */
#define RIO_EM_PW_TGT_DEVID	0x028	/* Port-write Target deviceID CSR */
#define RIO_EM_PKT_TTL		0x02c	/* Packet Time-to-live CSR */

/* Per-Port EM Registers */

#define RIO_EM_PN_ERR_DETECT(x)	(0x040 + x*0x40) /* Port N Error Detect CSR */
#define  REM_PED_IMPL_SPEC		0x80000000
#define  REM_PED_LINK_TO		0x00000001
#define RIO_EM_PN_ERRRATE_EN(x) (0x044 + x*0x40) /* Port N Error Rate Enable CSR */
#define RIO_EM_PN_ATTRIB_CAP(x)	(0x048 + x*0x40) /* Port N Attributes Capture CSR */
#define RIO_EM_PN_PKT_CAP_0(x)	(0x04c + x*0x40) /* Port N Packet/Control Symbol Capture 0 CSR */
#define RIO_EM_PN_PKT_CAP_1(x)	(0x050 + x*0x40) /* Port N Packet Capture 1 CSR */
#define RIO_EM_PN_PKT_CAP_2(x)	(0x054 + x*0x40) /* Port N Packet Capture 2 CSR */
#define RIO_EM_PN_PKT_CAP_3(x)	(0x058 + x*0x40) /* Port N Packet Capture 3 CSR */
#define RIO_EM_PN_ERRRATE(x)	(0x068 + x*0x40) /* Port N Error Rate CSR */
#define RIO_EM_PN_ERRRATE_TR(x) (0x06c + x*0x40) /* Port N Error Rate Threshold CSR */

/************************************************************************/
/*                        寄存器地址                                    */
/************************************************************************/
#define RIO_SWITCH_PORT_ERR_STAT_CSR(n)     (0x000158 + 0x20 * (n))
//#define RIO_DEV_ID_CAR              (0x000000)
#define RIO_SWITCH_PORT_INF_CAR     (0x000014)
//#define RIO_COMPONENT_TAG_CSR       (0x00006C)

#define RIO_SWITCH_RTE_DESTID       (0x000070)
#define RIO_SWITCH_RTE_PORT         (0x000074)
#define RIO_SWITCH_RTE_DEFAULT_PORT (0x000078)
#define RIO_SWITCH_PORT_CTL1(n)     (0x00015c + 0x20 * (n))
#define RIO_SWITCH_RTE_PORT_SEL     (0x00010070)
#define RIO_PORT_GEN_CTRL_CSR       (0x00013C)
#define RIO_PORT_N_ERR_STAT_CSR     (0x000158)

#define RIO_SWITCH_EM_PW_TGTID      (0x1028) // Port-Write Target Device ID CSR (in CPS1432)

#define CPS1432_DEV_ID              0x03750038

#define LOCAL_RTE_CONF_DESTID_SEL	0x010070
#define LOCAL_RTE_CONF_DESTID_SEL_PSEL	0x0000001f

#define IDT_LT_ERR_REPORT_EN	0x03100c

#define IDT_PORT_ERR_REPORT_EN(n)	(0x031044 + (n)*0x40)
#define IDT_PORT_ERR_REPORT_EN_BC	0x03ff04

#define IDT_PORT_ISERR_REPORT_EN(n)	(0x03104C + (n)*0x40)
#define IDT_PORT_ISERR_REPORT_EN_BC	0x03ff0c
#define IDT_PORT_INIT_TX_ACQUIRED	0x00000020

#define IDT_LANE_ERR_REPORT_EN(n)	(0x038010 + (n)*0x100)
#define IDT_LANE_ERR_REPORT_EN_BC	0x03ff10

#define IDT_DEV_CTRL_1		0xf2000c
#define IDT_DEV_CTRL_1_GENPW		0x02000000
#define IDT_DEV_CTRL_1_PRSTBEH		0x00000001

#define IDT_CFGBLK_ERR_CAPTURE_EN	0x020008
#define IDT_CFGBLK_ERR_REPORT		0xf20014
#define IDT_CFGBLK_ERR_REPORT_GENPW		0x00000002

#define IDT_AUX_PORT_ERR_CAP_EN	0x020000
#define IDT_AUX_ERR_REPORT_EN	0xf20018
#define IDT_AUX_PORT_ERR_LOG_I2C	0x00000002
#define IDT_AUX_PORT_ERR_LOG_JTAG	0x00000001

#define	IDT_ISLTL_ADDRESS_CAP	0x021014

#define IDT_RIO_DOMAIN		0xf20020
#define IDT_RIO_DOMAIN_MASK		0x000000ff

#define IDT_PW_INFO_CSR		0xf20024

#define IDT_SOFT_RESET		0xf20040
#define IDT_SOFT_RESET_REQ		0x00030097

#define IDT_I2C_MCTRL		0xf20050
#define IDT_I2C_MCTRL_GENPW		0x04000000

#define IDT_JTAG_CTRL		0xf2005c
#define IDT_JTAG_CTRL_GENPW		0x00000002

#define IDT_LANE_CTRL(n)	(0xff8000 + (n)*0x100)
#define IDT_LANE_CTRL_BC	0xffff00
#define IDT_LANE_CTRL_GENPW		0x00200000
#define IDT_LANE_DFE_1_BC	0xffff18
#define IDT_LANE_DFE_2_BC	0xffff1c

#define IDT_PORT_OPS(n)		(0xf40004 + (n)*0x100)
#define IDT_PORT_OPS_GENPW		0x08000000
#define IDT_PORT_OPS_PL_ELOG		0x00000040
#define IDT_PORT_OPS_LL_ELOG		0x00000020
#define IDT_PORT_OPS_LT_ELOG		0x00000010
#define IDT_PORT_OPS_BC		0xf4ff04

#define IDT_PORT_ISERR_DET(n)	(0xf40008 + (n)*0x100)

#define IDT_ERR_CAP		0xfd0000
#define IDT_ERR_CAP_LOG_OVERWR		0x00000004

#define IDT_ERR_RD		0xfd0004

#define IDT_DEFAULT_ROUTE	0xde
#define IDT_NO_ROUTE		0xdf

#define RIO_INVALID_ROUTE	0xff	/* Indicates that a route table
entry is invalid (no route
exists for the device ID) */
/************************************************************************/
/*                        常量宏定义                                    */
/************************************************************************/
//#define DMA_WIN_ADDR_RANGE 0x800000 /* 2的23次方，配置窗口时低地址范围2的32次方，用低地址就足够了 */
#define TSI721_SRIO_ADDR_BASE    	0x00000000
#define MAX_RIO_DEV_NUM				16						/* Rapidio设备数量上限 */
#define MAX_RIO_WIN_NUM				8						/* Rapidio窗口数量上限 */

#define DMA_OUTB_BUF_SIZE			(512 *1024)  //max dma size at present. 发送数据长度限制
#define WIN_BUF_SIZE				(DMA_OUTB_BUF_SIZE * 2)  //max dma size at present. 发送数据长度限制
#define INBOOD_DMA_SIZE				(WIN_BUF_SIZE * MAX_RIO_WIN_NUM) /* 不管多少个窗口，最多只能到 8M */
//#define DMA_WIN_BUF_SIZE			DMA_OUTB_BUF_SIZE * 2  // 8个窗口,每个窗口大小. 窗口大小(32KB - 16GB)，Size of window in bytes (32KB - 16GB)

#define MAX_TASKLIST_SIZE			8192 /* 任务队列深度 */

/* 门铃 */
#define DB_TYPE_BACK				0xffff    // 第一位标识类型，1数据长度，0返回
#define DB_TYPE_DATA				0x8000    // 第一位标识类型，10数据长度高，0返回


/************************************************************************/
/*                        变量定义                                    */
/************************************************************************/
extern int RIO_DEV_NUM;/* 打开的设备号 */
extern DWORD RIO_HOST_ID;
extern int RIO_DEVICE_LOCATION;
extern DWORD RIO_NODE_ARRAY[MAX_RIO_DEV_NUM];
extern int NUM_OF_RIO_NODE;

/************************************************************************/
/*                        数据结构定义                                   */
/************************************************************************/
typedef struct _NodeDestAddr
{
	unsigned int baseAddr;// 节点映射rio空间基地址
	unsigned int addrOffset; // 节点映射rio空间当前偏移量
}NodeDestAddr;
/* 地址计算辅助结构 */
typedef struct _NodeMapSheet
{
	NodeDestAddr m_nodeDestList[MAX_RIO_DEV_NUM];
}NodeMapSheet;
/* 数据节点 */
typedef struct _RioDataNode
{
	unsigned int s_uDataSize;
	char *s_pDataBuf;
}RioDataNode;
/************************************************************************/
/*                        内部接口                                       */
/************************************************************************/
int DestIDToLocation(DWORD destid);
DWORD LocationToDestID(int location);
/* 读取配置 */
void InitRioConfig();
/* 任务队列初始化 */
void RioTaskListInit();
/* 任务队列析构 */
void RioTaskListUninit();
/* 偏移计算辅助数据结构初始化 */
//int NodeMapInitail(NodeMapSheet *nodeSheet, DWORD *destIdArr, int baseAddr);
/* 偏移计算辅助数据结构初始化 */
int NodeSendMapInitail(NodeMapSheet *nodeSheet);
int NodeRecvMapInitail(NodeMapSheet *nodeSheet);
// 获取当前offset位置并计算是否够追加，返回可以使用的offset位置，并修改移动原offset
//int GetTargetOffsetAddr(NodeMapSheet *nodeSheet, const DWORD f_destId, const int dataLen, unsigned int *dmaAddr);
// 获取目标设备的窗口首地址
int GetTargetBaseAddr(NodeMapSheet *nodeSheet, const int location, unsigned int *dmaAddr);

/* 插入任务数据 */
void InsertRioData(char * f_buf, unsigned int f_dataLen);

/* 内部辅助函数 */
int OpenDeveice();
int ConfigWin(DWORD addr, DWORD size, int id);
void ClearDoorBell();

/* 发送数据之后，需要等到回应门铃才能继续发 */
void InitRioSendLock();
void RioSendLock(int location);;
void RioSendUnLock(DWORD destID);
/************************************************************************/
/*                        外部接口                                       */
/************************************************************************/
void RioSendBufInitial();
void RioSendBufUnInitial();
/* RapidIO网络初始化 */
int InitRioNetWork();
/* 组播发送数据接口*/
void MulticastSend(char *f_dataBuf, int f_iDataSize);
/* 发送数据接口*/
int UnicastSend(int location, char *f_dataBuf, int f_iDataSize);
/* 获取数据接口 */
unsigned int GetRioData(char *f_recvBuf);
/* 退出RapidIO网络 */
int ExitRioNetWork();