#include "module.h"

Module::Module(int module_number, int button_pin, AudioPlaySdWav* play_sd, AudioMixer4* mixer_left, AudioMixer4* mixer_right, TwoWire* twi, Encoder* knob)
{
    this->module_number = module_number;
    this->button_pin = button_pin;
    this->play_sd = play_sd;
    this->mixer_left = mixer_left;
    this->mixer_right = mixer_right;
    this->twi = twi;
    this->knob = knob;
}

void Module::init(Track** tracks_in)
{
    this->tracks = tracks_in;
    // init button
    button = OneButton(button_pin, true, true);
    button.attachClick(handle_single, this);
    // button.attachDoubleClick(handle_double, this);
    button.attachDuringLongPress(handle_hold, this);

    // init display
    set_mp_addr();
    display = Adafruit_SSD1306(MODULE_SCREEN_WIDTH, MODULE_SCREEN_HEIGHT, twi, OLED_RESET);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay(); // clear the buffer

    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.print("Started!");

    display.display(); // display the empty screen
}

void Module::update(int new_scene_index)
{
    // a scene has not been selected yet
    if (new_scene_index < 0) {
        return;
    }

    // new scene checks
    if (new_scene_index != scene_index) {
        scene_index = new_scene_index;
        skip = false;
        play_flag = false;
        loop_flag = false;

        // check if the filename is actually populated, min 5 characters for a valid file
        if (strlen(tracks[scene_index][module_number].filename) < 5) {
            skip = true;
            display_empty();
            return;
        }
        // check if the filename is actually on the SD card
        if (!SD.exists(tracks[scene_index][module_number].filename)) {
            skip = true;
            display_file_not_found();
            return;
        }

        // update the track duration
        // need to play briefly since lengthMillis only returns while playing
        play_sd->play(tracks[scene_index][module_number].filename);
        // TODO, this seems hacky and might be bad downstream
        while (!play_sd->isPlaying()) { } // block until the wav header is parsed, otherwise lengthMillis returns nothing
        track_duration = play_sd->lengthMillis();
        play_sd->stop();

        // TODO the loop flag causes tracks to start playing immediately, even when play is set to false

        // set flags
        if (tracks[scene_index][module_number].play) {
            play_flag = true;
        }
        // // set the play flag if we should be looping
        // if (tracks[scene_index][module_number].loop) {
        //     loop_flag = true;
        // }
        return;
    }

    // previously we determined that the filename given was trash, so just skip
    if (skip) {
        return;
    }

    // second time through the loop, start playing
    if (play_flag) {
        play_flag = false;
        play_sd->play(tracks[scene_index][module_number].filename);
    }

    // update buttons
    button.tick();

    // restart if the track finished and we want to loop
    if (tracks[scene_index][module_number].loop) {
        if (loop_flag && play_sd->isStopped()) {
            play_sd->play(tracks[scene_index][module_number].filename);
        }
    }

    // update gains
    // read new position
    long new_position = knob->read();
    // if it's changed
    if (new_position != old_position) {
        // get the difference as an integer
        long diff_int = old_position - new_position;
        // reset old position
        old_position = new_position;
        // map the difference into the range of gain
        gain_diff = double(map_float(diff_int, enc_int_min, enc_int_max, GAIN_MIN, GAIN_MAX));
    }

    mixer_left->gain(module_number % 4, tracks[scene_index][module_number].gain);
    mixer_right->gain(module_number % 4, tracks[scene_index][module_number].gain);

    update_display();
}

// set the multiplexer address to the current module
void Module::set_mp_addr()
{
    twi->beginTransmission(TCA_ADDR);
    twi->write(1 << module_number);
    twi->endTransmission();
}

// main display update function
void Module::update_display()
{
    set_mp_addr();
    display.clearDisplay(); // clear the buffer
    // track name, centered
    // rescale text size if name is too long to fit as size 2, can currently fit 10 chars before newline with size 2, 21 chars on a single line with size 1, will wrap if over that for a max total of 42
    if (strlen(tracks[scene_index][module_number].name) > 10) {
        display.setTextSize(1);
    } else {
        display.setTextSize(2);
    }
    display.setTextColor(SSD1306_WHITE);
    int16_t text_x, text_y;
    uint16_t text_w, text_h;
    display.getTextBounds(tracks[scene_index][module_number].name, 0, 0, &text_x, &text_y, &text_w, &text_h);
    display.setCursor((MODULE_SCREEN_WIDTH - text_w) / 2, (MODULE_SCREEN_HEIGHT / 2 - text_h) / 2);
    display.print(tracks[scene_index][module_number].name);

    // track length, count down when playing
    display.setTextSize(1);
    display.setCursor(6, 24);
    display.print(millis_to_hhmmss(track_duration - play_sd->positionMillis()));

    // draw track progress
    rect_progress_bar(54, 24, 74, 8, play_sd->positionMillis(), track_duration);

    // draw gain bar
    volume_display(26, 16, 72, 6, tracks[scene_index][module_number].gain, GAIN_MAX);

    // gain number
    display.setCursor(104, 16);
    display.printf("G%0.1f", tracks[scene_index][module_number].gain);

    // loop
    display.drawChar(6, 16, 0x4C, SSD1306_WHITE, SSD1306_BLACK, 1); // draw L
    if (tracks[scene_index][module_number].loop) {
        display.drawChar(0, 16, 0x2B, SSD1306_WHITE, SSD1306_BLACK, 1); // draw + if yes
    } else {
        display.drawChar(0, 16, 0x2D, SSD1306_WHITE, SSD1306_BLACK, 1); // draw - if no
    }

    // play
    display.drawChar(18, 16, 0x50, SSD1306_WHITE, SSD1306_BLACK, 1); // draw P
    if (tracks[scene_index][module_number].play) {
        display.drawChar(12, 16, 0x2B, SSD1306_WHITE, SSD1306_BLACK, 1); // draw + if yes
    } else {
        display.drawChar(12, 16, 0x2D, SSD1306_WHITE, SSD1306_BLACK, 1); // draw - if no
    }

    // play/pause indicator
    if (play_sd->isPlaying()) {
        // pause indicator is just two lines
        display.drawLine(0, 25, 0, 29, SSD1306_WHITE);
        display.drawLine(3, 25, 3, 29, SSD1306_WHITE);
    } else {
        // play indicator, neat sideways arrow
        display.drawChar(0, 24, 0x10, SSD1306_WHITE, SSD1306_BLACK, 1);
    }

    display.display();
}

// display an empty screen
void Module::display_empty()
{
    set_mp_addr();
    display.clearDisplay();
    display.display();
}

// display a file not found error
void Module::display_file_not_found()
{
    set_mp_addr();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println(tracks[scene_index][module_number].filename);
    display.println();
    display.setTextSize(2);
    display.print("Not found");
    display.display();
}

// the functions below are a super fun hack to get the onebutton library working within this class
// https://forum.arduino.cc/t/onebutton-library-in-wrapper-class/681944/9
void Module::handle_single(void* ptr)
{
    Module* module_ptr = (Module*)ptr;
    module_ptr->process_single();
}

void Module::handle_double(void* ptr)
{
    Module* module_ptr = (Module*)ptr;
    module_ptr->process_double();
}

void Module::handle_hold(void* ptr)
{
    Module* module_ptr = (Module*)ptr;
    module_ptr->process_hold();
}

// these three actually do stuff

// process a single click event (play / pause, restart control)
void Module::process_single()
{
    Serial.printf("%d module clicked!\n", module_number);
    Serial.printf("Playing: %s\n", tracks[scene_index][module_number].filename);
    // track state can either be: playing, paused, or stopped
    // based on state decide how it should change

    // If loop: play/pause
    if (tracks[scene_index][module_number].loop) {
        // if stopped it's the first time through and we need to play
        if (play_sd->isStopped()) {
            play_sd->play(tracks[scene_index][module_number].filename);
            loop_flag = true;
            return;
        }
        // otherwise we can toggle
        play_sd->togglePlayPause();
        return;
    }

    // if paused start playing
    if (play_sd->isPaused()) {
        play_sd->togglePlayPause();
        return;
    }

    // If stopped start playing
    if (play_sd->isStopped()) {
        play_sd->play(tracks[scene_index][module_number].filename);
        return;
    }

    // the last possible state is playing, so stop playing
    play_sd->stop();
}

// process a double click event (toggle loop setting)
void Module::process_double()
{
    tracks[scene_index][module_number].loop = !tracks[scene_index][module_number].loop;
}

// process a hold event (adjust gain of current module)
void Module::process_hold()
{
    // set the track gain
    tracks[scene_index][module_number].gain = tracks[scene_index][module_number].gain + gain_diff;
    // bound the gain
    if (tracks[scene_index][module_number].gain >= GAIN_MAX)
        tracks[scene_index][module_number].gain = GAIN_MAX;
    else if (tracks[scene_index][module_number].gain <= GAIN_MIN)
        tracks[scene_index][module_number].gain = GAIN_MIN;
    // reset the difference
    gain_diff = 0;
}

// stop playing
void Module::stop()
{
    // if looping toggle
    if (tracks[scene_index][module_number].loop) {
        if (play_sd->isPlaying()) {
            play_sd->togglePlayPause();
            return;
        }
    }
    // if paused toggle
    if (play_sd->isPaused()) {
        return;
    }
    // otherwise just stop
    play_sd->stop();
}

// convert milliseconds to a string formatted as HH:MM:SS
String Module::millis_to_hhmmss(uint32_t time_millis)
{
    char buffer[9];
    int time_ss = (time_millis / 1000) % 60;
    int time_mm = (time_millis / (1000 * 60)) % 60;
    int time_hh = (time_millis / (1000 * 60 * 60)) % 24;
    sprintf(buffer, "%02d:%02d:%02d", time_hh, time_mm, time_ss);
    return String(buffer);
}

// a rectangular bar that fills up from left to right depending on progress and duration
void Module::rect_progress_bar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t progress, uint32_t duration)
{
    display.drawRect(x, y, w, h, SSD1306_WHITE);
    int pos = (w - 2) * (double(progress) / double(duration));
    display.fillRect(x + 1, y + 1, pos, h - 2, SSD1306_WHITE);
}

// a horizontal line with edge and center ticks, and a cicle showing the current value
void Module::volume_display(uint16_t x, uint16_t y, uint16_t w, uint16_t h, double gain, double max)
{
    int r = h / 2; // radius of position marker
    int pos = (w - 2 * r) * (gain / max); // position of the marker on the line
    display.drawLine(x, y + h / 2, x + w, y + h / 2, SSD1306_WHITE); // horizontal line
    display.drawLine(x, y, x, y + h, SSD1306_WHITE); // left vertical
    display.drawLine(x + w, y, x + w, y + h, SSD1306_WHITE); // right vertical
    display.drawLine(x + w / 2, y, x + w / 2, y + h, SSD1306_WHITE); // center vertical
    display.fillCircle(x + pos + r, y + h / 2, r, SSD1306_WHITE); // position marker
}
