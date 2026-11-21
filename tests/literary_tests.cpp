#include "../src/literary_shelf.h"
#include "../src/app_model.h"
#include <set>
#include <iostream>

int checks=0;
void check(bool ok,const char* why){++checks;if(!ok)throw std::runtime_error(why);}
int main(){
    try{
        check(literaryNotes().size()==16,"sixteen notes");std::set<std::wstring> texts,authors;
        for(const auto&n:literaryNotes()){
            check(!n.text.empty()&&texts.insert(n.text).second,"unique nonempty human excerpts");
            check(!n.author.empty()&&!n.work.empty()&&!n.location.empty(),"provenance supplied");
            check(n.url.find(L"https://standardebooks.org/ebooks/")==0,"primary text source URL");
            authors.insert(n.author);
        }
        check(authors.size()==6,"six credited authors");
        LiteraryShelf s;check(s.found()==0&&!s.open,"new shelf");
        s.discover(0);check(s.found()==1,"initial discovery");
        s.step(-1);check(s.index==15,"previous wraps");s.step(1);check(s.index==0,"next wraps");
        for(int i=0;i<32;i++){s.step(1);check(s.index<LITERARY_NOTE_COUNT,"shelf bounds");}
        check(s.found()==16,"all discoveries counted once");
        LiteraryShelf contextual;
        for(int page=0;page<4;page++)for(int topic=0;topic<3;topic++)for(int n=0;n<10;n++){
            auto index=contextualNote(page,topic,n,n);check(index<16,"context bounds");contextual.discover(index);
        }
        check(contextual.found()==16,"normal quiz exploration reveals the entire collection");
        AppModel app;app.action(1,1000);app.action(10,2000);app.pause(3000);
        check(app.seconds(60000)==2,"reading shelf freezes timer");
        check(!app.action(2,60000),"reading cannot advance background quiz");
        app.action(21,63000);check(app.seconds(64000)==3&&app.session.answers[0]==0,"reading resumes without losing answer or charging reading time");
        std::cout<<"PASS: "<<checks<<" literary tests; metadata, source URLs, wrap, discovery counts, contextual coverage and paused reading.\n";
    }catch(const std::exception&e){std::cerr<<"FAILED: "<<e.what()<<'\n';return 1;}
}
