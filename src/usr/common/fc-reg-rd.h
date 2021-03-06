#ifndef __FC_REG_RD_H__
#define __FC_REG_RD_H__


typedef enum  {
	REG_FCPROBE_ALL_RESET = 0x0,        //bit:0
	REG_FCPROBE_SOFT_RESET = 0x0,       //bit:1
	REG_FCPROBE_HW_VERSION = 0x4,
	REG_FCPROBE_SPEED_SEL = 0x8,
	REG_FCPROBE_IMQ_BASE_HADDR = 0xc,
	REG_FCPROBE_IMQ_BASE_LADDR = 0x10,
	REG_FCPROBE_IMQ_LENGTH = 0x14,
	REG_FCPROBE_IMQ_CONS_INDEX = 0x18,
	REG_FCPROBE_IMQ_PROD_INDEX_HADDR = 0x1c,
	REG_FCPROBE_IMQ_PROD_INDEX_LADDR = 0x20,
	REG_FCPROBE_IMQ_PROD_INDEX = 0x24,
	REG_FCPROBE_IFQ0_Base_HAddr = 0x28,
	REG_FCPROBE_IFQ0_Base_LAddr = 0x2c,
	REG_FCPROBE_IFQ0_Length = 0x30,
	REG_FCPROBE_IFQ0_Cons_Index = 0x34,
	REG_FCPROBE_IFQ0_Prod_Index_HAddr = 0x38,
	REG_FCPROBE_IFQ0_Prod_Index_LAddr = 0x3c,
	REG_FCPROBE_IFQ0_Prod_Index = 0x40,
	REG_FCPROBE_IFQ1_Base_HAddr = 0x44,
	REG_FCPROBE_IFQ1_Base_LAddr = 0x48,
	REG_FCPROBE_IFQ1_Length = 0x4c,
	REG_FCPROBE_IFQ1_Cons_Index = 0x50,
	REG_FCPROBE_IFQ1_Prod_Index_HAddr = 0x54,
	REG_FCPROBE_IFQ1_Prod_Index_LAddr = 0x58,
	REG_FCPROBE_IFQ1_Prod_Index = 0x5c,
	REG_FCPROBE_PMQ_BASE_HADDR = 0x60,
	REG_FCPROBE_PMQ_BASE_LADDR = 0x64,
	REG_FCPROBE_PMQ_LENGTH = 0x68,
	REG_FCPROBE_PMQ_Cons_Index = 0x6c,
	REG_FCPROBE_PMQ_Prod_Index_HAddr = 0x70,
	REG_FCPROBE_PMQ_Prod_Index_LAddr = 0x74,
	REG_FCPROBE_PMQ_Prod_Index = 0x78,
	REG_FCPROBE_RT_TOV = 0x7c,
	REG_FCPROBE_OLS_TOV = 0x80,
	REG_FCPROBE_Collect_Interval = 0x84,
	REG_FCPROBE_FC_Paylaod_Size = 0x88,
	REG_FCPROBE_Current_LTimer = 0x8c,
	REG_FCPROBE_Current_HTimer = 0x90,
	REG_FCPROBE_Pcap_Magic = 0x94,
	REG_FCPROBE_Pcap_Version = 0x98,
	REG_FCPROBE_Pcap_Snap_Length = 0x9c,
	REG_FCPROBE_Pcap_Link_Type = 0xA0,
	REG_FCPROBE_LINK_ENABLE = 0xA4,
	REG_FCPROBE_DBG_0 = 0xC0,
	REG_FCPROBE_DBG_1 = 0xC4,
	REG_FCPROBE_DBG_2 = 0xC8,
	REG_FCPROBE_DBG_3 = 0xCC,
	REG_FCPROBE_DBG_4 = 0xD0,
	REG_FCPROBE_DBG_5 = 0xD4,
	REG_FCPROBE_DBG_6 = 0xD8,
	REG_FCPROBE_DBG_7 = 0xDC,
	REG_FCPROBE_DBG_8 = 0xE0,
	REG_FCPROBE_DBG_9 = 0xE4,
	REG_FCPROBE_DBG_10 = 0xE8,
	REG_FCPROBE_DBG_11 = 0xEC,
	REG_FCPROBE_DBG_12 = 0xF0,
	REG_FCPROBE_DBG_13 = 0xF4,
	REG_FCPROBE_DBG_14 = 0xF8,
	REG_FCPROBE_DBG_15 = 0xFC,
	REG_FCPROBE_INTERRUPT_ABLE = 0x1FC,
}FCProbe_registers;


#endif