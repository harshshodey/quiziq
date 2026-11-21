#pragma once
#include "quiz_engine.h"
#include <algorithm>
#include <cstdint>

// Platform-independent navigation. The Windows view and tests use the same rules.
class AppModel {
    std::uint64_t started=0,pausedAt=0,pausedMs=0;
public:
    std::vector<Topic> topics=makeTopics();
    QuizSession session;
    int page=0,topic=0,review=0,dialog=0,elapsed=0;
    bool missedOnly=false,closeRequested=false;
    int count() const {return int(topics.at(topic).questions.size());}
    int score() const {return session.score(topics.at(topic).questions);}
    int percent() const {return session.percent(topics.at(topic).questions);}
    int seconds(std::uint64_t now) const {
        auto end=dialog?pausedAt:now;
        return end>=started+pausedMs?int((end-started-pausedMs)/1000):0;
    }
    void pause(std::uint64_t now,bool closing=false) {
        if(page!=1) return;
        if(!dialog) pausedAt=now;
        dialog=closing?2:1;
    }
    int adjacentReview(int direction) const {
        if(direction!=1 && direction!=-1)return -1;
        for(int i=review+direction;i>=0&&i<count();i+=direction)
            if(!missedOnly || session.answers[i]!=topics[topic].questions[i].correct) return i;
        return -1;
    }
    bool action(int id,std::uint64_t now) {
        if(dialog) {
            if(id==21) {pausedMs+=now-pausedAt;dialog=0;return true;}
            if(id==22) {if(dialog==2)closeRequested=true;else page=0;dialog=0;return true;}
            return false;
        }
        if(page==0 && id>=100 && id<100+int(topics.size())) {topic=id-100;return true;}
        if((page==0&&id==1)||(page==2&&id==5)) {
            if(count()==0) return false;
            session=QuizSession(count());started=now;pausedMs=0;elapsed=0;page=1;review=0;missedOnly=false;return true;
        }
        if(page==1) {
            if(id>=10&&id<14) {session.select(id-10);return true;}
            if(id==2 && session.canAdvance()) {
                if(session.current==count()-1) {if(!session.complete())return false;elapsed=seconds(now);page=2;return true;}
                return session.next();
            }
            if(id==3)return session.previous();
            if(id==20) {pause(now);return true;}
            return false;
        }
        if(page==2) {
            if(id==6){page=0;return true;}
            if(id==4){review=0;missedOnly=false;page=3;return true;}
            if(id>=200&&id<200+count()){review=id-200;missedOnly=false;page=3;return true;}
            return false;
        }
        if(page==3) {
            if(id==9){page=2;return true;}
            if(id==7||id==8){int next=adjacentReview(id==7?-1:1);if(next<0)return false;review=next;return true;}
            if(id==15 && score()<count()) {
                missedOnly=!missedOnly;
                if(missedOnly && session.answers[review]==topics[topic].questions[review].correct) {
                    review=-1;review=adjacentReview(1);
                }
                return true;
            }
        }
        return false;
    }
};

inline int nextFocus(const std::vector<int>& ids,int current,bool reverse) {
    if(ids.empty())return -1;
    auto it=std::find(ids.begin(),ids.end(),current);
    if(it==ids.end())return reverse?ids.back():ids.front();
    int index=int(it-ids.begin()),n=int(ids.size());
    return ids[(index+(reverse?-1:1)+n)%n];
}
