#pragma once




Adafruit_ST7789* main_display;
OneButton knob_button;
Encoder* knob;






// onebutton update functions
void handle_single();
void handle_double();
void handle_hold();

// config reading functions
int size_of_toml_table(toml_table_t* table);
void print_track(Track input_track);
Track table_to_track(toml_table_t* input_table, double default_gain, bool default_loop, bool default_play);
void load_config();

// display functions
void update_main_display();

/*
void display_setup();

void display_text_test();

void display_encoder_test();
*/