#ifndef __FCPROBE_IMQ_H__
#define __FCPROBE_IMQ_H__





typedef union {
	uint32_t u32;
	struct {
		uint32_t cm_type:8;
		uint32_t param_select:8;
		uint32_t event_handle:12;
		uint32_t reserved:4;
	}s;
}fcprobe_imq_word0_t;

typedef union {
	uint32_t u32;
	struct {
		uint32_t ifq_offset:12;
		uint32_t reserved:20;
	};
}fcprobe_imq_word1_t;


typedef union {
	uint32_t u32;
	struct {
		uint32_t reserved:32;
	};
}fcprobe_imq_word2_t;


typedef union {
	uint32_t u32;
	struct {
		uint32_t reserved:32;
	};
}fcprobe_imq_word3_t;

typedef struct {
	fcprobe_imq_word0_t word0;
	fcprobe_imq_word1_t word1;
	fcprobe_imq_word2_t word2;
	fcprobe_imq_word3_t word3;
}fcprobe_imq_entry_t;


#define IMQ_CMC_TYPE_IFQ0 0x0
#define IMQ_CMC_TYPE_IFQ1 0x1
#define IMQ_CMC_TYPE_PMQ  0x2


#define FCPROBE_IMQ_ENTRY_SIZE  sizeof(fcprobe_imq_entry_t)
#define FCPROBE_IMQ_ENTRY_NUM   2048//4096



typedef struct {
	uint64_t idx_r;
	uint64_t idx_w;
	fcprobe_imq_entry_t imq_entry[FCPROBE_IMQ_ENTRY_NUM];
}fcprobe_imq_t;


#define FCPROBE_IMQ_SIZE  sizeof(fcprobe_imq_t)


#endif
