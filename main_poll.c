#include "./drivers/inc/vga.h"
#include "./drivers/inc/ISRs.h"
#include "./drivers/inc/LEDs.h"
#include "./drivers/inc/audio.h"
#include "./drivers/inc/HPS_TIM.h"
#include "./drivers/inc/int_setup.h"
#include "./drivers/inc/wavetable.h"
#include "./drivers/inc/pushbuttons.h"
#include "./drivers/inc/ps2_keyboard.h"
#include "./drivers/inc/HEX_displays.h"
#include "./drivers/inc/slider_switches.h"


void VGA_draw_sound_ctl(int amplitude);
int generate_audio_signal(int sample, char notes_played[], double frequencies[], int amplitude, int sampling_freq, char b);
void VGA_draw_wave(int wave[], char notes[], double frequencies[], int amplitude);
int search_key(char keys[], char key);

int main() {
    
    VGA_clear_pixelbuff_ASM();
    VGA_clear_charbuff_ASM();

    int_setup(1, (int[]) {200});
    
    // keys status buffers and variables
    int accepted_keys[] = {0x1B, 0x1C, 0x23, 0x2B, 0x3B, 0x42, 0x4B, 0x4C};
    double frequencies[] = {146.832, 130.813, 164.814, 174.614, 195.998, 220.000, 246.942, 261.626};
    char notes_played[] = {0, 0, 0, 0, 0, 0, 0, 0};
    char *key_pressed;
    int rvalid = 0;
    char valid_key = 0;
    char break_code = 0;
    
    // vga display buffer;
    int display_buffer[320] = {0};
    
    // set up timers    
    HPS_TIM_config_t sound_timer;
    sound_timer.tim = TIM1;
    sound_timer.timeout = 20;
    sound_timer.LD_en = 1;
    sound_timer.INT_en = 1;
    sound_timer.enable = 1;
    HPS_TIM_config_ASM(&sound_timer);
    

    int sample_count = 0;
    int sampling_frequency = 48000;
    int amplitude = 30;
    int signal = 0;

    while(1) {
        
        // check for key pressed
        rvalid = read_ps2_data_ASM(key_pressed);
        if (rvalid) {
            
            // search the key
            //int key_idx = search_key(accepted_keys, *key_pressed);
            int key_idx = -1;
            int i=0;
            for (i=0; i<8;i++) {
                if (accepted_keys[i] == *key_pressed) {
                    key_idx = i;
                    break;
                }
            }
            
            // valid key
            if (key_idx >= 0) {
                
                if (break_code == 0) { 
                    // key pressed                
                    notes_played[key_idx] = 1;
                } else {
                    // key was released
                    break_code = 0;
                    notes_played[key_idx] = 0;
                }

                
                // display updated wave
                VGA_draw_wave(display_buffer, notes_played, frequencies, amplitude);
                VGA_draw_sound_ctl(amplitude);


            } else if (*key_pressed == 0xF0) {
                break_code = 1;
            } 

            // sound control
            else if (*key_pressed == 0x75) { // up arrow
                if (break_code == 1) {
                    break_code = 0;
                } else {
                    if (amplitude <= 95)
                        amplitude += 5;
                }
                VGA_draw_sound_ctl(amplitude);
            } else if (*key_pressed == 0x72) {  // down arrow
                if (break_code == 1) {
                    break_code = 0;
                } else {
                    if (amplitude >= 5)
                        amplitude -= 5;
                }
                VGA_draw_sound_ctl(amplitude);
            }
        }
    
        // sound timer interrupt
        if (hps_tim1_int_flag) {
            hps_tim1_int_flag = 0;
            
            // generate sound sample
            signal = generate_audio_signal(sample_count, notes_played, frequencies, amplitude, sampling_frequency, 1);
            
            // try to write to audio codec
            if (audio_write_data_ASM(signal, signal)) {
                sample_count++;
                if (sample_count >= 48000)
                    sample_count = 0;
            }
        }
    }
}

int search_key(char keys[], char key) {
    
    // simple linear search
    int key_idx = -1;
    int i=0;
    for (i=0; i<8;i++) {
        if (keys[i] == key) {
            key_idx = i;
            break;
        }
    }
    return key_idx;
}

int generate_audio_signal(int sample, char notes_played[], double frequencies[], int amplitude, int sampling_frequency, char b) {
    int signal = 0;
    int i;
    for (i=0; i<8; i++) {
        if (notes_played[i] > 0)  {

            // linear interpolation
            float time_sample = frequencies[i] * sample;
            int low_sample = (int) time_sample;
            int high_sample = low_sample + 1;
            float high_weight = time_sample - (float) low_sample;
            float low_weight = 1.0 - high_weight;

            if (b == 1)
                signal += amplitude * ((int) (low_weight * sine[low_sample%sampling_frequency]) + (int) (high_weight * sine[high_sample%sampling_frequency])); 
            else
                signal += ((int) (low_weight * sine[low_sample%sampling_frequency]) + (int) (high_weight * sine[high_sample%sampling_frequency]));
            /*
            int idx = (int)(frequencies[i] * sample) % sampling_freq;
            signal += amplitude * sine[idx];
            */
        }    
    }
    return signal;
}

void VGA_draw_wave(int wave[], char notes[], double frequencies[], int amplitude) {
    VGA_clear_pixelbuff_ASM();
    int i=0;
    for (i=0; i<320; i++) {
        int sig = generate_audio_signal(i, notes, frequencies, amplitude, 48000, 0);
        float scaled = (float) sig / 0x1000000;
        int y_offset = (int) (scaled * 50);

        if (y_offset > 100)
            y_offset = 100;
        if (y_offset < -100)
            y_offset = -100;

        VGA_draw_point_ASM(i, 120+y_offset, 0x0F08);
    }
}


void VGA_draw_sound_ctl(int amplitude) {
    float max = 320.0;
    int sound_bar_len = (int) (max * ((float) amplitude)/100.0);
    int i;
    for (i=0; i<320; i++) {
        if (i < sound_bar_len) {
            VGA_draw_point_ASM(i, 1, 0x0F08); // green
            VGA_draw_point_ASM(i, 2, 0x0F08); // green
            VGA_draw_point_ASM(i, 3, 0x0F08); // green
        }
        
        else {
            VGA_draw_point_ASM(i, 1, 0x0000);
            VGA_draw_point_ASM(i, 2, 0x0000); // green
            VGA_draw_point_ASM(i, 3, 0x0000); // green
        }
    }   
}
