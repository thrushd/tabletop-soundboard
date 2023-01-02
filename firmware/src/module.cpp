#include "module.h"

Module::Module(int module_number, int button_pin, Track *tracks, AudioPlaySdWav *play_sd, AudioMixer4 *mixer, TwoWire *twi) {
    this->module_number = module_number;
    this->button_pin = button_pin;
    this->tracks = tracks;
    this->play_sd = play_sd;
    this->mixer = mixer;
    this->twi = twi;
    init();
}

void Module::init(){
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

void Module::update(int new_scene_index) {
    // new scene checks
    // if new scene is not scene, set scene to new scene
        // if track.play set player = true
        // start playing

    // update buttons
    button.tick();
    
    // restart if the track finished and we want to loop
    if (tracks[scene_index].loop) {
        if (player && !play_sd->isPlaying()){
            play_sd->play(tracks[scene_index].filename);
        }
    }

    // update module display
    scene_index = new_scene_index;
    update_display();
}

// set the multiplexer address to the current module
void Module::set_mp_addr() {
    twi->beginTransmission(TCA_ADDR);
    twi->write(1 << module_number);
    twi->endTransmission();
}

void Module::update_display() {
    set_mp_addr();
    display.clearDisplay(); // clear the buffer
    // track name
    display.setTextSize(2); // big text
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0,0); // Start at top-left corner
    display.println(tracks[scene_index].name); // write track name

    display.display(); // display the empty screen
}

// the functions below are a super fun hack to get the onebutton library working within this class
// https://forum.arduino.cc/t/onebutton-library-in-wrapper-class/681944/9
void Module::handle_single(void *ptr) {
    Module *module_ptr = (Module *)ptr;
    module_ptr->process_single();
}

void Module::handle_double(void *ptr) {
    Module *module_ptr = (Module *)ptr;
    module_ptr->process_double();
}

void Module::handle_hold(void *ptr) {
    Module *module_ptr = (Module *)ptr;
    module_ptr->process_hold();
}

// these three actuall do stuff
void Module::process_single(){
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
void Module::process_double(){
    Serial.println("double click!");
    tracks[scene_index].loop = !tracks[scene_index].loop;
}

void Module::process_hold(){
    Serial.println("holding!");
    
}
