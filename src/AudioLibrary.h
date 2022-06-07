//
// Created by Anshuman Komawar on 6/5/22.
//

#ifndef LAB3_AUDIOLIBRARY_H
#define LAB3_AUDIOLIBRARY_H

#include <iostream>
#include "miniaudio.h"

using namespace std;

class AudioLibrary {
public:
    AudioLibrary() {
        *result = ma_engine_init(NULL, engine.get());
    }

    void playAudio(string audioFile) {
        *result = ma_engine_play_sound(engine.get(), ("../resources/audio/" + audioFile + ".wav").c_str(), NULL);
        logDetails();
    }

    void playAudioInfintely(string audioFile) {
        *result = ma_sound_init_from_file(engine.get(), ("../resources/audio/" + audioFile + ".wav").c_str(), 0, NULL, NULL, sound.get());
        ma_sound_start(sound.get());
        logDetails();
    }



private:
    shared_ptr<ma_result> result= make_shared<ma_result>();
    shared_ptr<ma_engine> engine = make_shared<ma_engine>();
    shared_ptr<ma_sound> sound = make_shared<ma_sound>();

    void logDetails() {
        printf("MINIAUDIO LOG: %s\n", ma_result_description(*result));
    }
};


#endif //LAB3_AUDIOLIBRARY_H
