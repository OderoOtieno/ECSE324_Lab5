#ifndef __HPS_TIM
#define __HPS_TIM

	typedef enum {
		TIM0 = 0x00000001,
		TIM1 = 0x00000002,
		TIM2 = 0x00000004,
		TIM3 = 0x00000008
	}	HPS_TIM_t;

	typedef struct {
		HPS_TIM_t tim;
		int timeout; // in usec
		int LD_en;
		int INT_en;
		int enable;
		int current_time; // in usec
	}	HPS_TIM_config_t;
	
	extern void HPS_TIM_config_ASM(HPS_TIM_config_t *param);
	extern void HPS_TIM_clear_INT_ASM(HPS_TIM_t tim);


    HPS_TIM_config_t *timer_config(HPS_TIM_t tim, int timeout, int LD_en, int INT_en, int enable) {
        HPS_TIM_config_t timer;
        timer.tim = tim;
        timer.timeout = timeout;
        timer.LD_en = LD_en;
        timer.INT_en = INT_en;
        timer.enable = enable;

        return &timer;
    }

#endif
