#pragma once
#include "literary_notes.h"
#include <bitset>

// Notes stay stable while reading; only navigation reveals another one.
struct LiteraryShelf {
    size_t index=0;
    std::bitset<LITERARY_NOTE_COUNT> discovered;
    bool open=false,autoResume=false;
    void discover(size_t i){index=i%LITERARY_NOTE_COUNT;discovered.set(index);}
    void step(int direction){discover((index+LITERARY_NOTE_COUNT+(direction<0?-1:1))%LITERARY_NOTE_COUNT);}
    size_t found() const{return discovered.count();}
};

inline size_t contextualNote(int page,int topic,int question,int review){
    if(page==0)return size_t(topic==0?10:topic==1?14:6);
    if(page==1)return size_t(question+topic*5+1)%LITERARY_NOTE_COUNT;
    if(page==2)return size_t(topic==0?7:topic==1?14:15);
    return size_t(review+topic*3+3)%LITERARY_NOTE_COUNT;
}
