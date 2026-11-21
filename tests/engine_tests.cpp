#include "../src/app_model.h"
#include "../src/score_store.h"
#include <iostream>
#include <set>
#include <random>
#include <functional>

int checks=0;
void check(bool condition,const char* label) {
    ++checks;
    if(!condition)throw std::runtime_error(label);
}
void throws(const std::function<void()>& fn,const char* label){bool caught=false;try{fn();}catch(const std::exception&){caught=true;}check(caught,label);}
void play(AppModel& app,int correct,std::uint64_t time=1000){
    check(app.action(app.page==2?5:1,time),"start round");
    int size=app.count();
    for(int i=0;i<size;i++){
        check(!app.action(2,time),"unanswered cannot advance");
        int choice=app.topics[app.topic].questions[i].correct;
        if(i>=correct)choice=(choice+1)%4;
        check(app.action(10+choice,time+1000*(i+1)),"select answer");
        check(app.action(2,time+1000*(i+1)),"advance or finish");
    }
    check(app.page==2 && app.score()==correct,"completed result");
}
void passOne(){
    auto topics=makeTopics();check(topics.size()==3,"three topics");
    for(const auto& t:topics){
        check(t.questions.size()==10,"ten questions per topic");
        std::set<std::wstring> prompts;
        for(const auto& q:t.questions){
            check(!q.prompt.empty()&&!q.explanation.empty(),"content present");
            check(prompts.insert(q.prompt).second,"unique questions");
            check(q.correct>=0&&q.correct<4,"valid answer index");
            std::set<std::wstring> options(q.options.begin(),q.options.end());
            check(options.size()==4&&!options.count(L""),"distinct nonempty options");
        }
        for(int target=0;target<=10;target++){
            QuizSession s(10);check(s.answered()==0&&!s.complete(),"fresh state");
            check(!s.next()&&!s.previous(),"navigation at initial boundary");
            for(int i=0;i<10;i++){
                check(s.current==i,"current question");
                s.select(i<target?t.questions[i].correct:(t.questions[i].correct+1)%4);
                if(i<9)check(s.next(),"next selected question");
            }
            check(!s.next(),"last question boundary");
            check(s.complete()&&s.answered()==10,"all answered");
            check(s.score(t.questions)==target&&s.percent(t.questions)==target*10,"all score percentages");
            for(int i=8;i>=0;i--)check(s.previous()&&s.current==i,"backtracking");
            check(!s.previous(),"first boundary after backtracking");
            s.select(t.questions[0].correct);int expected=target==0?1:target;
            check(s.score(t.questions)==expected&&s.answered()==10,"revised answer counted once");
            throws([&]{s.select(-1);},"negative option rejected");
            throws([&]{s.select(4);},"out of range option rejected");
            throws([&]{s.percent({});},"mismatched empty bank rejected");
            s.current=-1;throws([&]{s.select(0);},"negative question rejected");
            check(!s.canAdvance(),"invalid current cannot advance");
        }
    }
    QuizSession empty(0);check(empty.percent({})==0&&!empty.complete()&&!empty.next()&&!empty.previous(),"zero question state");
    throws([&]{empty.select(0);},"empty selection rejected");
    std::cout<<"PASS 1: all question banks, every score 0-100%, revised answers, bounds and invalid input.\n";
}
void passTwo(){
    for(int topic=0;topic<3;topic++)for(int target=0;target<=10;target++){
        AppModel m;
        check(!m.action(4,0)&&!m.action(200,0)&&!m.action(5,0),"home rejects result actions");
        check(m.action(100+topic,0)&&m.topic==topic,"topic selection");
        play(m,target);
        check(m.elapsed==10&&m.percent()==target*10,"result timer and score");
        check(!m.action(2,90000)&&!m.action(10,90000)&&!m.action(1,90000),"results reject stale quiz actions");
        check(m.action(209,100000)&&m.review==9&&m.page==3,"trail opens exact question");
        check(!m.action(8,100000),"review end boundary");
        check(!m.action(210,100000)&&!m.action(5,100000),"review rejects offscreen actions");
        if(target<10){
            check(m.action(15,0)&&m.missedOnly,"enable missed filter");
            for(int i=8;i>=target;i--)check(m.action(7,0)&&m.review==i,"missed previous");
            check(!m.action(7,0)&&m.review==target,"first missed boundary");
            for(int i=target+1;i<10;i++)check(m.action(8,0)&&m.review==i,"missed next");
            check(!m.action(8,0),"last missed boundary");
            check(m.action(15,0)&&!m.missedOnly,"disable filter");
        }else check(!m.action(15,0),"perfect round cannot filter to empty set");
        check(m.action(9,0)&&m.page==2,"return to results");
        check(m.action(4,0)&&m.review==0&&!m.missedOnly,"all-answer review resets position");
        if(target<10)check(m.action(15,0)&&m.review==target,"filter jumps to first missed");
        check(m.action(9,0)&&m.action(5,200000),"retry");
        check(m.page==1&&m.session.current==0&&m.session.answered()==0&&m.seconds(200000)==0,"retry clears progress and timer");
        check(!m.action(100,0)&&!m.action(200,0)&&!m.action(6,0),"quiz blocks offscreen actions");
        check(m.action(10,201000),"selection before pause");
        m.pause(205500);check(m.dialog==1&&m.seconds(220000)==5,"pause freezes elapsed");
        check(!m.action(2,220000)&&!m.action(11,220000),"paused inputs blocked");
        check(m.action(21,225500)&&m.seconds(230000)==10,"resume excludes paused duration");
        check(m.session.answers[0]==0,"pause preserves answers");
        m.pause(231000);m.pause(240000,true);
        check(m.dialog==2&&m.seconds(300000)==11,"close from pause preserves frozen time");
        check(m.action(21,241000)&&!m.closeRequested,"cancel close");
        m.pause(242000);check(m.action(22,242001)&&m.page==0,"leave to topics");
        check(m.action(1,250000),"start again after leave");m.pause(251000,true);
        check(m.action(22,252000)&&m.closeRequested,"confirm close");
    }
    check(nextFocus({1,2,3},-1,true)==3,"Shift+Tab starts at last control");
    check(nextFocus({1,2,3},-1,false)==1,"Tab starts at first control");
    check(nextFocus({1,2,3},1,true)==3&&nextFocus({1,2,3},3,false)==1,"focus wraps in both directions");
    check(nextFocus({},-1,false)==-1&&nextFocus({5},5,true)==5,"empty and single focus targets");
    AppModel empty;empty.topics[0].questions.clear();check(!empty.action(1,0),"empty bank cannot start");
    for(unsigned seed:{17u,1024u,65537u}){
        AppModel m;std::mt19937 rng(seed);std::vector<int> actions{1,2,3,4,5,6,7,8,9,10,11,12,13,15,20,21,22,100,101,102,200,205,209,210,-1};
        for(int i=0;i<25000;i++){
            m.action(actions[rng()%actions.size()],1000+i*100);
            check(m.page>=0&&m.page<=3&&m.topic>=0&&m.topic<3,"stress screen range");
            if(m.page>0)check(m.session.current>=0&&m.session.current<m.count(),"stress question range");
            if(m.page>=2)check(m.session.complete()&&m.review>=0&&m.review<m.count(),"stress result invariant");
            if(m.closeRequested)m=AppModel();
        }
    }
    std::cout<<"PASS 2: all topics and results; guarded navigation, review filters, retry, pause/close, focus, 75,000 mixed actions.\n";
}
void passThree(const std::filesystem::path& root){
    std::filesystem::create_directories(root);
    auto file=root/L"scores.txt";
    BestScores fresh{-1,-1,-1};
    check(loadScores(file)==fresh,"missing score file");
    check(saveScores(file,{0,80,100}),"first atomic save");
    check(loadScores(file)==BestScores{0,80,100},"zero percent survives reload");
    check(saveScores(file,{100,90,100})&&loadScores(file)==BestScores{100,90,100},"replace existing scores");
    check(!saveScores(file,{101,90,100})&&loadScores(file)==BestScores{100,90,100},"invalid save preserves disk");
    check(!saveScores({},fresh),"missing path handled");
    check(!saveScores(root/L"missing"/L"scores.txt",fresh),"unwritable path handled");
    check(!saveScores(root,fresh)&&std::filesystem::is_directory(root),"replacement failure preserves target");
    for(const char* data:{"", "80", "80 90", "-2 0 0", "101 90 80", "a 90 80", "80 90 100 junk", "80 90 100 0", "9999999999999999999999999 0 0"}){
        {std::ofstream out(file);out<<data;}
        check(loadScores(file)==fresh,"damaged file rejected without fake high scores");
    }
    {std::ofstream out(file);out<<"20 40 60 ";}
    check(loadScores(file)==BestScores{20,40,60},"legacy score format supported");
    check(saveScores(file,{-1,0,100})&&loadScores(file)==BestScores{-1,0,100},"unplayed distinct from a zero score");
    for(const auto& entry:std::filesystem::directory_iterator(root))check(entry.path().extension()!=L".tmp","no abandoned temp files");
    std::filesystem::remove(file);std::filesystem::remove(root);
    std::cout<<"PASS 3: persistence, zero scores, legacy data, atomic replacement, malformed files, save failures and temp cleanup.\n";
}
int main(int argc,char** argv){
    try{
        passOne();passTwo();
        auto base=argc>1?std::filesystem::path(argv[1]):std::filesystem::temp_directory_path();
        passThree(base/("quiziq-tests-"+std::to_string(GetCurrentProcessId())));
        std::cout<<"ALL PASSED: "<<checks<<" checks (also active in Release/NDEBUG builds).\n";
        return 0;
    }catch(const std::exception& e){std::cerr<<"FAILED after "<<checks<<" checks: "<<e.what()<<'\n';return 1;}
}
