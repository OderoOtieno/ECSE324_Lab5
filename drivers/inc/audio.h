#ifndef __AUDIO
#define __AUDIO

  // READ LEFT AND RIGHT

//Read
	extern int audio_read_data_ASM(int *leftdata, int *rightdata);
	extern int audio_read_leftdata_ASM(int *data);
	extern int audio_read_rightdata_ASM(int *data);

//Write
	extern int audio_write_data_ASM(int leftdata, int rightdata);
	extern int audio_write_leftdata_ASM(int data);
	extern int audio_write_rightdata_ASM(int data);

//FIFO ASM, Clear
	extern void audio_enable_read_fifo_clear_ASM(void);
	extern void audio_enable_write_fifo_clear_ASM(void);
	extern void audio_disable_read_fifo_clear_ASM(void);
	extern void audio_disable_write_fifo_clear_ASM(void);

//Enable
	extern void audio_enable_read_int_ASM(void);
	extern void audio_enable_write_int_ASM(void);

//Disable
	extern void audio_disable_read_int_ASM(void);
	extern void audio_disable_write_int_ASM(void);

//Read WSLC AND WSRC
	extern int audio_read_wslc_ASM(void);
	extern int audio_read_wsrc_ASM(void);

//Read RALC AND RARC
	extern int audio_read_ralc_ASM(void);
	extern int audio_read_rarc_ASM(void);

#endif


///Main poll
#include "./drivers/inc/audio.h"

void VGA_draw_sound_ctl(int amplitude);
int generate_audio_signal(int sample, char notes_played[], double frequencies[], int amplitude, int sampling_freq, char b);
void VGA_draw_wave(int wave[], char notes[], double frequencies[], int amplitude);
int search_key(char keys[], char key);

