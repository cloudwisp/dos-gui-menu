#ifndef GameMus_cpp
#define GameMus_cpp

class GameMusic {

private:
    static MIDI *music;
public:


    static void PlaySong(char *file, int repeat){
        if (music){
            //clear existing song
            stop_midi();
            destroy_midi(music);
        }
        music = load_midi(file);
        play_midi(music,repeat);
    }

    static void Stop(){
        stop_midi();
    }

};

MIDI *GameMusic::music = NULL;

#endif // GameMus_cpp
