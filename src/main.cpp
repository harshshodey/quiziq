#ifndef UNICODE
#define UNICODE
#endif
#define _UNICODE
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <shlobj.h>
#include <commdlg.h>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <memory>
#include "app_model.h"
#include "score_store.h"
#include "literary_shelf.h"
using namespace Gdiplus;
constexpr int W=1200,H=800;
const Color INK(29,32,57),MUTED(100,106,129),PURPLE(114,80,235),PALE(241,237,255),BG(247,248,252),WHITE(255,255,255),LINE(231,233,241),GREEN(23,125,99),RED(188,62,86);
struct Hit {RectF box; int id;};
std::vector<Hit> hits;
AppModel app;
LiteraryShelf shelf;
std::wstring shelfNotice;
auto& topics=app.topics;
auto& session=app.session;
int &page=app.page,&topic=app.topic,&review=app.review,&elapsed=app.elapsed;
HWND hwnd;
int hover=-1,focus=-1,pressed=-1;
BestScores best{-1,-1,-1};
bool bestSaved=true;
std::wstring resultNotice;
float scale=1,ox=0,oy=0;
std::filesystem::path bestFile;
std::unique_ptr<Bitmap> backBuffer;
std::wstring num(int x){return std::to_wstring(x);}
std::wstring clockText(int sec){std::wstringstream o;o<<std::setfill(L'0')<<std::setw(2)<<sec/60<<L":"<<std::setw(2)<<sec%60;return o.str();}
void repaint(){InvalidateRect(hwnd,nullptr,FALSE);}
void syncDiscoveries(){shelf.discovered.set(contextualNote(page,topic,session.current,review));if(page==0)shelf.discovered.set(0);if(page==2)shelf.discovered.set(7);if(page==3)shelf.discovered.set(12);if(app.dialog&&!shelf.open)shelf.discovered.set(9);}
void closeShelf(){if(shelf.autoResume)app.action(21,GetTickCount64());shelf.open=false;shelf.autoResume=false;shelfNotice.clear();}
void focusRing(Graphics&g,float x,float y,float w,float h,int id){if(focus==id){Pen p(PURPLE,2);g.DrawRectangle(&p,x-3,y-3,w+6,h+6);}}
void roundRect(Graphics& g,float x,float y,float w,float h,Color c,float r=16,Color border=Color(0,0,0,0)) {
    GraphicsPath p; float d=r*2;
    p.AddArc(x,y,d,d,180,90);p.AddArc(x+w-d,y,d,d,270,90);p.AddArc(x+w-d,y+h-d,d,d,0,90);p.AddArc(x,y+h-d,d,d,90,90);p.CloseFigure();
    SolidBrush b(c);g.FillPath(&b,&p);if(border.GetA()){Pen pen(border,1);g.DrawPath(&pen,&p);}
}
void text(Graphics& g,std::wstring s,float x,float y,float w,float h,float size=14,Color c=INK,bool bold=false,int align=0) {
    FontFamily family(L"Segoe UI");Font f(&family,size,bold?FontStyleBold:FontStyleRegular,UnitPixel);SolidBrush b(c);
    StringFormat fmt;fmt.SetLineAlignment(StringAlignmentCenter);fmt.SetAlignment(align==1?StringAlignmentCenter:align==2?StringAlignmentFar:StringAlignmentNear);
    g.DrawString(s.c_str(),-1,&f,RectF(x,y,w,h),&fmt,&b);
}
void line(Graphics&g,float x,float y,float x2,float y2,Color c=LINE){Pen p(c,1);g.DrawLine(&p,x,y,x2,y2);}
void button(Graphics&g,int id,std::wstring label,float x,float y,float w,float h,bool primary=false,bool enabled=true){
    Color fill=primary?PURPLE:WHITE; if(!enabled)fill=Color(224,225,235);else if(hover==id)fill=primary?Color(94,62,212):PALE;
    roundRect(g,x,y,w,h,fill,12,primary?Color(0,0,0,0):LINE);
    text(g,label,x+10,y,w-20,h,14,!enabled?MUTED:primary?WHITE:INK,true,1);
    if(enabled){hits.push_back({RectF(x,y,w,h),id});focusRing(g,x,y,w,h,id);}
}
void chip(Graphics&g,std::wstring s,float x,float y,float w,Color bg=PALE,Color fg=PURPLE){roundRect(g,x,y,w,28,bg,14);text(g,s,x,y,w,28,11,fg,true,1);}
void saveBest(){auto disk=loadScores(bestFile);for(size_t i=0;i<best.size();i++)best[i]=std::max(best[i],disk[i]);best[topic]=std::max(best[topic],app.percent());bestSaved=saveScores(bestFile,best);}
void sidebar(Graphics&g){
    SolidBrush b(INK);g.FillRectangle(&b,0,0,224,H);
    roundRect(g,28,32,38,38,PURPLE,12);text(g,L"Q",28,31,38,38,25,WHITE,true,1);text(g,L"quiz",77,28,78,44,28,WHITE,true);text(g,L"iq",145,28,58,44,28,Color(176,153,255),true);
    text(g,L"A LITTLE SMARTER, EVERY DAY",28,86,180,20,9,Color(147,151,176),true);
    text(g,L"YOUR WORKSPACE",28,150,180,24,10,Color(131,136,164),true);
    roundRect(g,16,190,192,49,Color(54,47,88),10);roundRect(g,16,204,3,21,Color(173,145,255),1);
    text(g,L"◈",31,190,28,49,22,Color(184,161,255));text(g,L"Quiz room",69,190,125,49,14,WHITE,true);
    text(g,L"01",32,264,28,26,12,Color(132,137,165));text(g,L"Choose a topic",70,264,130,26,13,page==0?WHITE:Color(132,137,165));
    text(g,L"02",32,309,28,26,12,Color(132,137,165));text(g,L"Test your knowledge",70,309,145,26,13,page==1?WHITE:Color(132,137,165));
    text(g,L"03",32,354,28,26,12,Color(132,137,165));text(g,L"See your progress",70,354,145,26,13,page>=2?WHITE:Color(132,137,165));
    const auto& note=literaryNotes()[contextualNote(page,topic,session.current,review)];
    text(g,L"THIS VISIT: "+num(int(shelf.found()))+L" / 16 FOUND",28,410,182,24,10,Color(176,153,255),true);
    roundRect(g,20,456,184,260,hover==30?Color(49,48,83):Color(39,43,70),14);
    text(g,L"FROM THE BOOKSHELF",36,471,152,25,10,Color(176,153,255),true);
    text(g,L"“"+note.text+L"”",36,505,152,125,14,WHITE);
    text(g,note.author,36,638,152,28,12,Color(199,194,224),true);
    text(g,L"Explore the shelf  →",36,679,152,24,12,Color(199,184,255),true);
    hits.push_back({RectF(20,456,184,260),30});focusRing(g,20,456,184,260,30);
    line(g,24,734,200,734,Color(56,60,86));roundRect(g,26,751,28,28,Color(63,66,98),14);text(g,L"Y",26,751,28,28,12,WHITE,true,1);text(g,L"Your learning space",64,751,150,28,11,Color(174,179,202));
}
void top(Graphics&g,std::wstring section){text(g,L"WORKSPACE  /  QUIZ ROOM",272,27,600,28,10,MUTED,true);chip(g,L"●  OFFLINE & READY",1000,28,150,Color(231,245,239),GREEN);line(g,272,81,1152,81);text(g,section,272,100,600,20,11,PURPLE,true);}
void home(Graphics&g){
    top(g,L"LET CURIOSITY LEAD");
    text(g,L"Curiouser and\ncuriouser!",272,139,620,109,43,INK,true);
    text(g,L"— Lewis Carroll · Alice’s Adventures in Wonderland",275,254,650,24,12,PURPLE);
    text(g,L"Pick a topic, take the challenge, and see what you know.",274,291,650,30,16,MUTED);
    // Decorative orbit made entirely from vector drawing primitives.
    Pen orbit(Color(221,214,249),1.5);g.DrawEllipse(&orbit,957,148,162,162);g.DrawEllipse(&orbit,976,167,124,124);
    roundRect(g,995,186,86,86,PURPLE,26);text(g,L"?",995,186,86,86,56,WHITE,true,1);
    roundRect(g,1099,180,16,16,Color(244,190,83),8);roundRect(g,962,271,12,12,Color(77,182,157),6);
    hits.push_back({RectF(995,186,86,86),32});focusRing(g,995,186,86,86,32);
    if(hover==32||focus==32)text(g,L"Open a literary note",939,317,200,24,11,PURPLE,true,1);
    text(g,L"Find your next challenge",272,352,650,36,23,INK,true);text(g,L"03 TOPICS  ·  30 QUESTIONS",929,354,223,32,10,MUTED,true,2);
    for(int i=0;i<3;i++){
        float x=272+i*298;bool sel=topic==i;
        roundRect(g,x,412,282,221,hover==100+i&&!sel?Color(250,248,255):WHITE,18,sel?PURPLE:LINE);
        roundRect(g,x+20,434,48,48,i==0?PALE:i==1?Color(231,246,240):Color(255,244,222),13);
        text(g,topics[i].symbol,x+20,434,48,48,i==0?16:22,i==0?PURPLE:i==1?GREEN:Color(181,125,32),true,1);
        if(sel)chip(g,L"SELECTED",x+169,443,92);
        text(g,topics[i].title,x+20,497,245,31,21,INK,true);
        text(g,topics[i].subtitle,x+20,533,245,44,12,MUTED);
        text(g,L"10 questions  ·  No time limit",x+20,587,245,23,11,MUTED);
        hits.push_back({RectF(x,412,282,221),100+i});
        focusRing(g,x,412,282,221,100+i);
    }
    text(g,L"Personal best: "+(best[topic]>=0?num(best[topic])+L"%":L"No completed rounds yet"),274,667,550,30,13,MUTED);
    button(g,1,L"Start challenge   →",914,662,238,54,true);
    text(g,L"10 questions. One point per correct answer. No negative marking.",274,735,850,24,12,MUTED);
}
void quiz(Graphics&g){
    auto&q=topics[topic].questions[session.current];int n=session.current;
    top(g,L"THE CHALLENGE  /  "+topics[topic].title);
    text(g,topics[topic].title,272,141,680,48,32,INK,true);
    chip(g,L"◷  "+clockText(app.seconds(GetTickCount64())),1020,151,132);
    text(g,L"Question "+num(n+1)+L" of 10",272,208,400,28,14,INK,true);text(g,num(session.answered())+L" answered",940,208,212,28,12,MUTED,false,2);
    roundRect(g,272,249,880,6,LINE,3);roundRect(g,272,249,880.f*(n+1)/10,6,PURPLE,3);
    roundRect(g,272,282,880,359,WHITE,18,LINE);
    text(g,L"QUESTION "+std::wstring(n+1<10?L"0":L"")+num(n+1),300,302,500,24,10,PURPLE,true);
    text(g,q.prompt,300,336,810,70,25,INK,true);
    for(int a=0;a<4;a++){
        float x=300+(a%2)*422,y=430+(a/2)*91;bool selected=session.answers[n]==a;
        roundRect(g,x,y,402,72,selected?PALE:hover==10+a?Color(249,248,255):WHITE,12,selected?PURPLE:LINE);
        roundRect(g,x+14,y+17,36,36,selected?PURPLE:BG,10);
        text(g,std::wstring(1,wchar_t(L'A'+a)),x+14,y+17,36,36,13,selected?WHITE:MUTED,true,1);
        text(g,q.options[a],x+66,y+9,317,54,15,selected?PURPLE:INK,selected);
        hits.push_back({RectF(x,y,402,72),10+a});
        focusRing(g,x,y,402,72,10+a);
    }
    button(g,3,L"←  Previous",272,670,150,50,false,n>0);
    button(g,20,L"Pause / Esc",438,670,151,50);
    button(g,2,n==9?L"See my results   →":L"Next question   →",922,670,230,50,true,session.answers[n]>=0);
    text(g,L"Use 1–4 to select  ·  Enter to continue  ·  ← to go back",430,738,722,24,12,MUTED,false,2);
    text(g,session.answers[n]>=0?L"Answer selected. Enter to continue.":L"Choose an answer to continue.",272,738,330,24,12,MUTED);
}
void metric(Graphics&g,std::wstring label,std::wstring value,std::wstring hint,float x,float y,Color accent){
    roundRect(g,x,y,253,118,WHITE,16,LINE);roundRect(g,x+20,y+20,6,6,accent,3);
    text(g,label,x+36,y+12,195,23,11,MUTED,true);text(g,value,x+20,y+40,215,39,28,INK,true);text(g,hint,x+20,y+83,220,22,11,MUTED);
}
void results(Graphics&g){
    int score=session.score(topics[topic].questions),pct=session.percent(topics[topic].questions);
    top(g,L"CHALLENGE COMPLETE  /  "+topics[topic].title);
    text(g,pct>=80?L"That's a strong finish.":pct>=50?L"You're making progress.":L"A fresh place to grow.",272,139,850,55,38,INK,true);
    text(g,L"“Training is everything.” — Mark Twain · Pudd’nhead Wilson",274,205,850,30,14,MUTED);
    roundRect(g,272,269,334,254,INK,20);
    Pen track(Color(62,64,93),12),arc(Color(176,151,255),12);arc.SetStartCap(LineCapRound);arc.SetEndCap(LineCapRound);
    g.DrawEllipse(&track,373,293,132,132);if(pct)g.DrawArc(&arc,373,293,132,132,-90,3.6f*pct);
    text(g,num(pct)+L"%",373,321,132,57,40,WHITE,true,1);text(g,L"ACCURACY",373,376,132,22,9,Color(178,181,205),true,1);
    text(g,num(score)+L" out of 10 correct",292,444,294,30,20,WHITE,true,1);text(g,L"“Words, words, words.” — Shakespeare",292,480,294,22,11,Color(174,177,202),false,1);
    metric(g,L"CORRECT ANSWERS",num(score)+L" / 10",L"+1 point for each correct answer",626,269,GREEN);
    metric(g,L"TO REVISIT",num(10-score),score==10?L"A clean sweep. Nicely done!":L"New things to learn in your review",899,269,RED);
    metric(g,L"TIME TAKEN",clockText(elapsed),L"Your pace. No time pressure.",626,405,PURPLE);
    metric(g,L"PERSONAL BEST",num(best[topic])+L"%",bestSaved?L"Saved on this device":L"Session only: couldn't save on disk",899,405,Color(214,159,58));
    roundRect(g,272,547,880,102,WHITE,16,LINE);text(g,L"Your answer trail",294,561,260,28,15,INK,true);
    text(g,L"✓ Correct",900,561,115,28,11,GREEN,true);text(g,L"× Revisit",1020,561,110,28,11,RED,true,2);
    for(int i=0;i<10;i++){bool ok=session.answers[i]==topics[topic].questions[i].correct;float x=294.f+i*54;roundRect(g,x,601,38,30,ok?Color(230,246,239):Color(253,235,239),9,hover==200+i?PURPLE:Color(0,0,0,0));text(g,num(i+1)+(ok?L" ✓":L" ×"),x,601,38,30,11,ok?GREEN:RED,true,1);hits.push_back({RectF(x,601,38,30),200+i});focusRing(g,x,601,38,30,200+i);}
    text(g,L"Click a number to explore the answer.",855,599,275,30,11,MUTED,false,2);
    button(g,4,L"Review answers   →",272,680,243,53,true);button(g,5,L"Try again",531,680,153,53);
    button(g,16,L"Save results image",700,680,202,53);
    button(g,6,L"Explore topics",985,680,167,53);
    text(g,resultNotice.empty()?L"Literary notes include their author, original work, and source. Explore the shelf in the sidebar.":resultNotice,272,751,880,29,11,MUTED,false,1);
}
void reviewPage(Graphics&g){
    const auto&q=topics[topic].questions[review];bool ok=session.answers[review]==q.correct;
    top(g,L"ANSWER REVIEW  /  "+topics[topic].title);
    text(g,L"A closer look.",272,142,700,52,36,INK,true);text(g,L"“Words, words, words.” — William Shakespeare · Hamlet",274,206,850,30,14,MUTED);
    button(g,15,app.missedOnly?L"✓  Missed answers only":L"Show missed answers",927,151,225,42,false,app.score()<app.count());
    roundRect(g,272,269,880,363,WHITE,18,LINE);
    chip(g,L"QUESTION "+num(review+1)+L" / 10",296,289,150);chip(g,ok?L"CORRECT":L"TO REVISIT",1000,289,128,ok?Color(230,246,239):Color(253,235,239),ok?GREEN:RED);
    text(g,q.prompt,296,334,820,70,25,INK,true);
    text(g,L"YOUR ANSWER",296,425,378,24,10,MUTED,true);text(g,L"CORRECT ANSWER",718,425,400,24,10,GREEN,true);
    text(g,q.options[session.answers[review]],296,456,380,35,18,ok?GREEN:RED,true);text(g,q.options[q.correct],718,456,390,35,18,GREEN,true);
    roundRect(g,296,520,832,87,PALE,12);text(g,q.explanation,316,532,790,62,14,INK);
    button(g,7,L"←  Previous",272,667,167,51,false,app.adjacentReview(-1)>=0);button(g,9,L"Back to results",620,667,184,51);button(g,8,L"Next answer   →",972,667,180,51,true,app.adjacentReview(1)>=0);
    text(g,L"Use ← and → to explore answers. Escape returns to your results.",272,748,880,25,12,MUTED,false,1);
}
void pauseDialog(Graphics&g){
    SolidBrush shade(Color(155,20,23,42));g.FillRectangle(&shade,0,0,W,H);
    hits.clear();roundRect(g,381,184,556,415,WHITE,22);
    chip(g,L"QUIZ PAUSED",411,208,145);
    text(g,app.dialog==2?L"Leave this challenge?":L"Your quiz is paused.",411,253,496,48,30,INK,true);
    text(g,L"The timer is stopped. Resume to keep your answers.\nLeaving ends this round without saving its progress.",411,308,496,60,14,MUTED);
    roundRect(g,411,384,496,119,PALE,12);
    text(g,L"“"+literaryNotes()[9].text+L"”",427,395,464,66,16,INK);
    text(g,L"Jerome K. Jerome · Three Men in a Boat, chapter XV",427,467,464,23,11,PURPLE);
    button(g,21,L"Resume challenge",411,525,237,51,true);
    button(g,22,app.dialog==2?L"Exit QuizIQ":L"Leave for topics",664,525,243,51);
}
void literaryDialog(Graphics&g){
    SolidBrush shade(Color(166,20,23,42));g.FillRectangle(&shade,0,0,W,H);hits.clear();
    const auto&note=literaryNotes()[shelf.index];
    roundRect(g,325,125,730,555,WHITE,24);
    chip(g,L"LITERARY SHELF",357,152,159);
    text(g,num(int(shelf.found()))+L" / 16 found this visit",792,152,230,28,12,MUTED,true,2);
    text(g,L"Literary note "+num(int(shelf.index+1))+L" / 16",357,197,575,32,22,INK,true);
    roundRect(g,357,247,666,150,PALE,15);
    text(g,L"“"+note.text+L"”",379,265,622,115,26,INK,true);
    text(g,note.author,357,413,666,29,20,INK,true);
    text(g,note.work,357,448,666,28,15,PURPLE,true);
    text(g,note.location,357,484,666,30,12,MUTED);
    text(g,shelfNotice.empty()?L"Verbatim excerpt · Source: Standard Ebooks · Original human-authored text":shelfNotice,357,528,666,31,11,MUTED);
    button(g,40,L"←",357,584,47,48);
    button(g,41,L"→",414,584,47,48);
    button(g,44,L"Copy quote",480,584,133,48);
    button(g,43,L"Read the source",627,584,179,48);
    button(g,42,L"Back to quiz room",820,584,203,48,true);
    text(g,L"← / → Browse     Esc Return     Opening this shelf pauses an active quiz.",357,643,666,20,11,MUTED,false,1);
}
void draw(Graphics&g,int width,int height){
    g.Clear(BG);scale=std::min(width/float(W),height/float(H));ox=(width-W*scale)/2;oy=(height-H*scale)/2;g.TranslateTransform(ox,oy);g.ScaleTransform(scale,scale);
    g.SetSmoothingMode(SmoothingModeAntiAlias);g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);hits.clear();sidebar(g);
    if(page==0)home(g);else if(page==1)quiz(g);else if(page==2)results(g);else reviewPage(g);
    if(shelf.open)literaryDialog(g);else if(app.dialog)pauseDialog(g);
}
void copyLiteraryNote(){
    const auto&note=literaryNotes()[shelf.index];
    std::wstring value=L"“"+note.text+L"”\r\n— "+note.author+L", "+note.work+L"\r\n"+note.location+L"\r\n"+note.url;
    HGLOBAL block=GlobalAlloc(GMEM_MOVEABLE,(value.size()+1)*sizeof(wchar_t));
    if(!block){shelfNotice=L"Couldn't copy the quote. Please try again.";return;}
    auto data=static_cast<wchar_t*>(GlobalLock(block));
    if(!data){GlobalFree(block);shelfNotice=L"Couldn't copy the quote. Please try again.";return;}
    memcpy(data,value.c_str(),(value.size()+1)*sizeof(wchar_t));GlobalUnlock(block);
    bool copied=false;if(OpenClipboard(hwnd)){if(EmptyClipboard()&&SetClipboardData(CF_UNICODETEXT,block))copied=true;CloseClipboard();}
    if(!copied)GlobalFree(block);
    shelfNotice=copied?L"Copied with the author, original work, and source link.":L"Clipboard is busy. Please try again.";
}
void saveResultImage(){
    wchar_t name[32768]=L"QuizIQ-results.png";
    OPENFILENAME ofn={};ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=hwnd;ofn.lpstrFilter=L"PNG image (*.png)\0*.png\0";ofn.lpstrFile=name;ofn.nMaxFile=32768;ofn.lpstrDefExt=L"png";ofn.lpstrTitle=L"Save your QuizIQ results";ofn.Flags=OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;
    if(!GetSaveFileName(&ofn)){if(CommDlgExtendedError())resultNotice=L"Couldn't open the save dialog. Please try again.";return;}
    auto savedHits=hits;float savedScale=scale,savedOx=ox,savedOy=oy;int savedFocus=focus,savedHover=hover;
    focus=hover=-1;Bitmap bitmap(W,H,PixelFormat32bppARGB);
    {Graphics g(&bitmap);draw(g,W,H);}
    hits=savedHits;scale=savedScale;ox=savedOx;oy=savedOy;focus=savedFocus;hover=savedHover;
    // The PNG encoder CLSID is defined by Windows GDI+.
    CLSID png={0x557cf406,0x1a04,0x11d3,{0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e}};
    resultNotice=bitmap.Save(name,&png,nullptr)==Ok?L"Results image saved. Ready to share!":L"Couldn't save the image. Try another folder.";
}
void action(int id){
    // Never dispatch an action from a previous screen or a disabled control.
    if(std::none_of(hits.begin(),hits.end(),[id](const Hit&b){return b.id==id;}))return;
    int before=page;
    if(id==30||id==32){shelf.discover(id==32?2:contextualNote(page,topic,session.current,review));shelf.autoResume=page==1&&!app.dialog;if(shelf.autoResume)app.pause(GetTickCount64());shelf.open=true;shelfNotice.clear();}
    else if(shelf.open){
        if(id==40||id==41){shelf.step(id==40?-1:1);shelfNotice.clear();}
        else if(id==42)closeShelf();
        else if(id==44)copyLiteraryNote();
        else if(id==43){auto result=(INT_PTR)ShellExecuteW(hwnd,L"open",literaryNotes()[shelf.index].url.c_str(),nullptr,nullptr,SW_SHOWNORMAL);shelfNotice=result>32?L"Source opened in your browser.":L"Browser unavailable. Source links are also in quote-sources.json.";}
        else return;
    }
    else if(id==16 && page==2)saveResultImage();
    else if(!app.action(id,GetTickCount64()))return;
    syncDiscoveries();
    if(before==1 && page==2){resultNotice.clear();saveBest();}
    if(app.closeRequested){DestroyWindow(hwnd);return;}
    focus=-1;
    hover=pressed=-1;hits.clear();repaint();UpdateWindow(hwnd);
}
int hitAt(LPARAM l){float x=(GET_X_LPARAM(l)-ox)/scale,y=(GET_Y_LPARAM(l)-oy)/scale;for(auto&b:hits)if(b.box.Contains(x,y))return b.id;return -1;}
LRESULT CALLBACK proc(HWND h,UINT msg,WPARAM w,LPARAM l){
    switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_GETMINMAXINFO: {auto m=(MINMAXINFO*)l;RECT area;SystemParametersInfo(SPI_GETWORKAREA,0,&area,0);m->ptMinTrackSize={std::min(960L,area.right-area.left),std::min(690L,area.bottom-area.top)};return 0;}
    case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);RECT r;GetClientRect(h,&r);if(r.right>0&&r.bottom>0){if(!backBuffer||backBuffer->GetWidth()!=UINT(r.right)||backBuffer->GetHeight()!=UINT(r.bottom))backBuffer=std::make_unique<Bitmap>(r.right,r.bottom,PixelFormat32bppARGB);{Graphics g(backBuffer.get());draw(g,r.right,r.bottom);}Graphics screen(dc);screen.DrawImage(backBuffer.get(),0,0);}EndPaint(h,&ps);return 0;}
    case WM_SIZE:if(w==SIZE_MINIMIZED&&page==1&&!app.dialog){app.pause(GetTickCount64());syncDiscoveries();focus=-1;hits.clear();}repaint();return 0;
    case WM_TIMER:if(page==1&&!app.dialog)repaint();return 0;
    case WM_MOUSEMOVE:{TRACKMOUSEEVENT track={sizeof(track),TME_LEAVE,h,0};TrackMouseEvent(&track);int next=hitAt(l);if(next!=hover){hover=next;repaint();}SetCursor(LoadCursor(nullptr,next>=0?IDC_HAND:IDC_ARROW));return 0;}
    case WM_MOUSELEAVE:hover=-1;repaint();return 0;
    case WM_LBUTTONDOWN:pressed=hitAt(l);SetCapture(h);return 0;
    case WM_LBUTTONUP:{int id=hitAt(l),down=pressed;pressed=-1;ReleaseCapture();if(id>=0&&id==down)action(id);return 0;}
    case WM_CAPTURECHANGED:pressed=-1;return 0;
    case WM_KILLFOCUS:focus=-1;pressed=-1;repaint();return 0;
    case WM_CLOSE:if(shelf.open)closeShelf();if(page==1){app.pause(GetTickCount64(),true);syncDiscoveries();focus=-1;hits.clear();repaint();UpdateWindow(h);}else DestroyWindow(h);return 0;
    case WM_KEYDOWN:
      // One physical press performs one action, even when Windows repeats a held key.
      if(l&(1LL<<30))return 0;
      if(w==VK_TAB){std::vector<int> ids;for(auto&b:hits)ids.push_back(b.id);focus=nextFocus(ids,focus,GetKeyState(VK_SHIFT)<0);repaint();return 0;}
      if(shelf.open){if(w==VK_ESCAPE)action(42);else if(w==VK_LEFT)action(40);else if(w==VK_RIGHT)action(41);else if((w==VK_RETURN||w==VK_SPACE)&&focus>=0)action(focus);return 0;}
      if(w==VK_ESCAPE){if(app.dialog)action(21);else if(page==1)action(20);else if(page==3)action(9);return 0;}
      if(w==VK_RETURN||w==VK_SPACE){if(focus>=0)action(focus);else if(w==VK_RETURN){if(app.dialog)action(21);else if(page==0)action(1);else if(page==1)action(2);}return 0;}
      if(app.dialog)return 0;
      if(w==VK_F1){action(30);return 0;}
      if(page==0&&w>='1'&&w<='3')action(100+int(w-'1'));
      if(page==1){if(w>='1'&&w<='4')action(10+int(w-'1'));else if(w>=VK_NUMPAD1&&w<=VK_NUMPAD4)action(10+int(w-VK_NUMPAD1));else if(w==VK_LEFT)action(3);}
      if(page==3){if(w==VK_LEFT)action(7);else if(w==VK_RIGHT)action(8);else if(w==VK_ESCAPE)action(9);}return 0;
    case WM_DESTROY:KillTimer(h,1);PostQuitMessage(0);return 0;
    }return DefWindowProc(h,msg,w,l);
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR,int show){
    SetProcessDPIAware();ULONG_PTR token;GdiplusStartupInput input;if(GdiplusStartup(&token,&input,nullptr)!=Ok)return 1;
    PWSTR local=nullptr;if(SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData,0,nullptr,&local))){std::filesystem::path dir=std::filesystem::path(local)/L"QuizIQ";CoTaskMemFree(local);std::error_code ec;std::filesystem::create_directories(dir,ec);if(!ec){bestFile=dir/L"best-scores.txt";best=loadScores(bestFile);}}
    WNDCLASS wc={};wc.hInstance=instance;wc.lpfnWndProc=proc;wc.lpszClassName=L"QuizIQWindow";wc.hCursor=LoadCursor(nullptr,IDC_ARROW);wc.hIcon=LoadIcon(nullptr,IDI_APPLICATION);RegisterClass(&wc);
    RECT rect={0,0,W,H};AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);
    RECT area;SystemParametersInfo(SPI_GETWORKAREA,0,&area,0);
    int width=std::min(rect.right-rect.left,area.right-area.left),height=std::min(rect.bottom-rect.top,area.bottom-area.top);
    hwnd=CreateWindow(wc.lpszClassName,L"QuizIQ — A little smarter, every day",WS_OVERLAPPEDWINDOW,area.left+(area.right-area.left-width)/2,area.top+(area.bottom-area.top-height)/2,width,height,nullptr,nullptr,instance,nullptr);
    if(!hwnd){GdiplusShutdown(token);return 1;}syncDiscoveries();SetTimer(hwnd,1,1000,nullptr);ShowWindow(hwnd,show);UpdateWindow(hwnd);
    MSG msg;while(GetMessage(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessage(&msg);}backBuffer.reset();GdiplusShutdown(token);return 0;
}
