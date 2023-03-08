#ifndef _DS_SCREEN_H_
#define _DS_SCREEN_H_


void init_screen_interface(void);
void ds_screen_init();
void ds_screen_clean_white();

void ds_screen_partial_display(unsigned int x_start,unsigned int y_start,void partial_new(void),unsigned int PART_COLUMN,unsigned int PART_LINE);

void ds_screen_partial_display_bydata(unsigned int x_start,unsigned int y_start,
                                        void partial_new(const uint8_t *data),const uint8_t *new_data,
                                        unsigned int PART_COLUMN,unsigned int PART_LINE);

void ds_screen_full_display(void pic_display(void));
void ds_screen_full_display_data(const uint8_t *data);
void ds_screen_full_display_bydata(void display_func(const uint8_t *data),const uint8_t *data);


void ds_screen_partial_data_init();
void ds_screen_partial_data_add(unsigned int x_start,unsigned int x_end,unsigned int y_start,unsigned int y_end ,const uint8_t *data);
void ds_screen_partial_data_copy();

void ds_screen_partial_test(void);
void test_SSD1681(void);

void refresh_part();
void deep_sleep();

void ds_screen_wordcup_backgroud();

#endif

