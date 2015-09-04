
// create the engine and output mix objects
void createEngine();
// create buffer queue audio player
void createBufferQueueAudioPlayer(int,int);
void set_play_callback(void (*callback)());
void AudioWrite(void *data,int size);
