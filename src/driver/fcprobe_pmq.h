#ifndef __FCPROBE_PMQ_H__
#define __FCPROBE_PMQ_H__

typedef union {
	uint32_t u32;
	struct {
		uint32_t card_num:4;
		uint32_t channel_num:2;
		uint32_t type:2;
		uint32_t reserved:8;
		uint32_t sequence:16;
	};
}fcprobe_lpq_word0_t;


typedef struct {
	fcprobe_lpq_word0_t word0;                         //word0
	uint32_t reserved;
	uint32_t Timestamp;
	uint32_t loss_signal_event;
	uint32_t code_violations;
	uint32_t loss_sync_event;
	uint32_t lip_event;
	uint32_t nos_ols_events;
	uint32_t link_up_event;
	uint32_t frame_err;
	uint32_t el_service_frame;       //word10
	uint32_t fc_service_frame;
	uint32_t soff_frame;
	uint32_t basic_fc_ls;
	uint32_t fc_link_c_frame;
	uint32_t cc_state_frame;
	uint32_t other_bad_status_frame;
	uint32_t task_maga_frame;
	uint32_t logins;
	uint32_t logouts;
	uint32_t abts;      //word20
	uint32_t notifications;
	uint32_t rejects;
	uint32_t busy;
	uint32_t accepts;
	uint32_t loop_init_frames;
	uint32_t fps;
	uint32_t bps;
	uint32_t scsi_fps1;
	uint32_t scsi_bps1;
	uint32_t manage_fps1;   //word30
	uint32_t manage_bps1;
	uint32_t app_data_fps;
	uint32_t app_data_bps;
	uint32_t bps2;
	uint32_t total_capacity;
	uint32_t scsi_bps2;
	uint32_t total_capacity2;
	uint32_t manage_bps2;
	uint32_t total_capacity3;
	uint32_t other_fps;    //word40
	uint32_t other_bps;
	uint32_t other_bps2;
	uint32_t total_capacity4;
	uint32_t pending_exchange;
	uint32_t max_pending_exchange;
	uint32_t speed;
	uint32_t signal_state;
	uint32_t reserved1[16];                             //word48-63
}fcprobe_lpq_entry_t;



typedef union {
	uint32_t u32;
	struct {
		uint32_t card_num:4;
		uint32_t channel_num:2;
		uint32_t type:2;
		uint32_t reserved: 8;
		uint32_t sequence: 16;
	}s;
}fcprobe_spq_word0_t;



typedef struct {
	fcprobe_spq_word0_t word0;                         //word0
        uint32_t timestamp;
	uint32_t initiator_id;                             //word1
	uint32_t target_id;                                //word2
	uint32_t lun_low;
	uint32_t lun_high;
	uint32_t bps;
	uint32_t pps;
	uint32_t tmf;
	uint32_t obsf;
	uint32_t ccsf;
	uint32_t rc_issued;                                //word10
	uint32_t rc_comp;
	uint32_t min_rsize;
	uint32_t max_rsize;
	uint32_t rd_bps;
	uint32_t rd_pps;
	uint32_t min_cmd_to_first;
	uint32_t max_cmd_to_first;
	uint32_t total_cmd_to_first;
	uint32_t min_last_to_resp;
	uint32_t max_last_to_resp;                         //word20
	uint32_t total_last_to_resp;
	uint32_t min_read_exchange_comp;
	uint32_t max_read_exchange_comp;
	uint32_t total_read_exchange_comp;
	uint32_t write_cmd_issued;
	uint32_t write_cmd_comp;
	uint32_t min_wsize;
	uint32_t max_wsize;
	uint32_t wd_bps;
	uint32_t wd_pps;                                    //word30
	uint32_t min_cmd_to_trans_ready;
	uint32_t max_cmd_to_trans_ready;
	uint32_t total_cmd_to_trans_ready;
	uint32_t min_trans_ready_to_first;
	uint32_t max_trans_ready_to_first;
	uint32_t total_trans_ready_to_first;
	uint32_t min_last_data_to_resp;
	uint32_t max_last_data_to_resp;
	uint32_t total_last_data_to_resp;
	uint32_t min_write_exchange_comp;                   //word40
	uint32_t max_write_exchange_comp;
	uint32_t total_write_exchange_comp;
	uint32_t pending_exchanges;
	uint32_t min_pending_exchanges;
	uint32_t max_pending_exchanges;                     //word45
	uint32_t reserved[17];                              //word46-63
}fcprobe_spq_entry_t;


#define FCPROBE_PMQ_ENTRY_NUM  2048 //4096

#define FCPROBE_SPQ_ENTRY_SIZE  (64*4)


typedef struct {
	uint64_t idx_r;
	uint64_t idx_w;
	fcprobe_spq_entry_t spq_entry[FCPROBE_PMQ_ENTRY_NUM];
}fcprobe_spq_t;

#define FCPROBE_LPQ_ENTRY_SIZE  (64*4)


typedef struct {
	uint64_t idx_r;
	uint64_t idx_w;
	fcprobe_lpq_entry_t lpq_entry[FCPROBE_PMQ_ENTRY_NUM];
}fcprobe_lpq_t;


#define FCPROBE_LPQ_SIZE  sizeof(fcprobe_lpq_t)
#define FCPROBE_SPQ_SIZE  sizeof(fcprobe_spq_t)




typedef struct {
    uint64_t idx_r;
    uint64_t idx_w;
    union
    {
        fcprobe_spq_entry_t spq_entry[FCPROBE_PMQ_ENTRY_NUM];
        fcprobe_lpq_entry_t lpq_entry[FCPROBE_PMQ_ENTRY_NUM];
    };
}fcprobe_pmq_t;


#define FCPROBE_PMQ_SIZE  sizeof(fcprobe_pmq_t)




#endif
