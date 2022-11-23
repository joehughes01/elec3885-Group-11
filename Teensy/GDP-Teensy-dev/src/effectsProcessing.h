#ifndef EFFECTSPROCESSING_H
#define EFFECTSPROCESSING_H


class Effect{
    public:
    //public functions
    void setEffect();
    void sampleSend();
    void setParams();
    float sampleReturn();
    private:
    int currentFXindex;

};

#endif