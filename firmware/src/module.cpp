#include "module.h"

Module::Module(int module_number, int button_pin, Track* tracks, AudioPlaySdWav* play_sd, AudioMixer4* mixer, TwoWire* twi)
{
    this->module_number = module_number;
    this->button_pin = button_pin;
    this->tracks = tracks;
    this->play_sd = play_sd;
    this->mixer = mixer;
    this->twi = twi;
    init();
}

void Module::init()
{
    // init button
    // button = Bounce(button_pin, INPUT_PULLUP);
    // button.interval(10);

    button = OneButton(button_pin, true, true);
    button.attachClick(handle_single, this);
    button.attachDoubleClick(handle_double, this);
    button.attachDuringLongPress(handle_hold, this);

    // init display
    set_mp_addr();
    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, twi, OLED_RESET);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay(); // clear the buffer
    display.display(); // display the empty screen
}

void Module::update(int new_scene_index)
{
    // new scene checks
    // if new scene is not scene, set scene to new scene
    // if track.play set player = true
    // start playing
    // set new duration
    if (new_scene_index != scene_index) {
        scene_index = new_scene_index;
        // update the static duration since lengthMillis only returns while playing
        play_sd->play(tracks[scene_index].filename);
        // TODO, this seems hacky and might be bad downstream
        while (!play_sd->isPlaying()) { } // block until the wav header is parsed, otherwise lengthMillis returns nothing
        track_duration = play_sd->lengthMillis();
        play_sd->stop();
    }

    // TODO: finish audio logic

    // update buttons
    button.tick();

    // restart if the track finished and we want to loop
    if (tracks[scene_index].loop) {
        if (player && !play_sd->isPlaying()) {
            play_sd->play(tracks[scene_index].filename);
        }
    }

    // update module display
    scene_index = new_scene_index;
    update_display();
}

// set the multiplexer address to the current module
void Module::set_mp_addr()
{
    twi->beginTransmission(TCA_ADDR);
    twi->write(1 << module_number);
    twi->endTransmission();
}

void Module::update_display()
{
    set_mp_addr();
    display.clearDisplay(); // clear the buffer
    // track name, centered
    // TODO: rescale text size if name is too long to fit as size 2, can currently fit 10 chars before newline
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    int16_t text_x, text_y;
    uint16_t text_w, text_h;
    display.getTextBounds(tracks[scene_index].name, 0, 0, &text_x, &text_y, &text_w, &text_h);
    display.setCursor((SCREEN_WIDTH - text_w) / 2, 0);
    display.print(tracks[scene_index].name);
    // track length, count down when playing
    display.setTextSize(1);
    display.setCursor(6, 24);
    display.print(millis_to_hhmmss(track_duration - play_sd->positionMillis()));
    // draw track progress
    rect_progress_bar(54, 24, 74, 8, play_sd->positionMillis(), track_duration);
    // draw gain bar
    volume_display(26, 16, 72, 6, tracks[scene_index].gain, 2);
    // gain number
    display.setCursor(104, 16);
    char gain_buff[16];
    sprintf(gain_buff, "G%0.1f", tracks[scene_index].gain);
    display.print(gain_buff);

    // loop
    display.drawChar(6, 16, 0x4C, SSD1306_WHITE, SSD1306_BLACK, 1); // draw L
    if (tracks[scene_index].loop) {
        display.drawChar(0, 16, 0x2B, SSD1306_WHITE, SSD1306_BLACK, 1); // draw + if yes
    } else {
        display.drawChar(0, 16, 0x2D, SSD1306_WHITE, SSD1306_BLACK, 1); // draw - if no
    }
    // play
    display.drawChar(18, 16, 0x50, SSD1306_WHITE, SSD1306_BLACK, 1); // draw P
    if (tracks[scene_index].play) {
        display.drawChar(12, 16, 0x2B, SSD1306_WHITE, SSD1306_BLACK, 1); // draw + if yes
    } else {
        display.drawChar(12, 16, 0x2D, SSD1306_WHITE, SSD1306_BLACK, 1); // draw - if no
    }
    // play/pause indicator
    if (play_sd->isPlaying()) {
        // pause indicator
        display.drawLine(0, 25, 0, 29, SSD1306_WHITE);
        display.drawLine(3, 25, 3, 29, SSD1306_WHITE);
    } else {
        // play indicator
        display.drawChar(0, 24, 0x10, SSD1306_WHITE, SSD1306_BLACK, 1);
    }

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

// these three actuall do stuff
void Module::process_single()
{
    Serial.println("click!");

    // If loop: play/pause
    if (tracks[scene_index].loop && play_sd->isPaused()) {
        Serial.println("play/pause");
        play_sd->togglePlayPause();
        return;
    }

    // If stopped or playing: play from start
    if (play_sd->isStopped() || play_sd->isPlaying()) {
        play_sd->play(tracks[scene_index].filename);
        player = true;
    }
}

// on double click invert loop setting
void Module::process_double()
{
    Serial.println("double click!");
    tracks[scene_index].loop = !tracks[scene_index].loop;
}

void Module::process_hold()
{
    Serial.println("holding!");
}

String Module::millis_to_hhmmss(uint32_t time_millis)
{
    char buffer[16];
    int time_ss = (time_millis / 1000) % 60;
    int time_mm = (time_millis / (1000 * 60)) % 60;
    int time_hh = (time_millis / (1000 * 60 * 60)) % 24;
    sprintf(buffer, "%02d:%02d:%02d", time_hh, time_mm, time_ss);
    return String(buffer);
}

void Module::rect_progress_bar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t progress, uint32_t duration)
{
    display.drawRect(x, y, w, h, SSD1306_WHITE);
    int pos = (w - 2) * (double(progress) / double(duration));
    display.fillRect(x + 1, y + 1, pos, h - 2, SSD1306_WHITE);
}

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
