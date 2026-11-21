// Exercise the actual drawing code offscreen; no window, user input, or user data.
#define wWinMain quizApplicationEntry
#include "../src/main.cpp"
#undef wWinMain
#include <set>
#include <iostream>

int renderChecks=0;
void require(bool ok,const char* why){++renderChecks;if(!ok)throw std::runtime_error(why);}
bool has(int id){return std::any_of(hits.begin(),hits.end(),[id](const Hit&h){return h.id==id;});}
void render(int width,int height){
    Bitmap bitmap(width,height,PixelFormat32bppARGB);Graphics g(&bitmap);
    draw(g,width,height);
    require(g.GetLastStatus()==Ok,"GDI+ rendering status");
    std::set<int> ids;
    for(const auto& h:hits){
        require(ids.insert(h.id).second,"unique visible action ids");
        require(h.box.X>=0&&h.box.Y>=0&&h.box.GetRight()<=W&&h.box.GetBottom()<=H,"control within design bounds");
        require(h.box.X*scale+ox>=0&&h.box.Y*scale+oy>=0&&h.box.GetRight()*scale+ox<=width+0.01&&h.box.GetBottom()*scale+oy<=height+0.01,"control reachable at scaled size");
    }
    for(size_t i=0;i<hits.size();i++)for(size_t j=i+1;j<hits.size();j++)
        require(!hits[i].box.IntersectsWith(hits[j].box),"interactive controls do not overlap");
}
int main(){
    ULONG_PTR token;GdiplusStartupInput input;
    if(GdiplusStartup(&token,&input,nullptr)!=Ok)return 1;
    int result=0;
    try{
        for(auto size:{std::pair{1200,800},std::pair{944,651},std::pair{1600,900}}){
            app=AppModel();shelf=LiteraryShelf();syncDiscoveries();
            app.action(1,GetTickCount64());syncDiscoveries();shelf.open=true;app.pause(GetTickCount64());syncDiscoveries();
            require(!shelf.discovered.test(9),"shelf does not count the hidden pause quote as discovered");
            shelf.open=false;syncDiscoveries();require(shelf.discovered.test(9),"visible pause quote counts as discovered");
            app=AppModel();shelf=LiteraryShelf();focus=-1;hover=-1;best={80,80,80};
            render(size.first,size.second);
            require(hits.size()==6&&has(1)&&has(100)&&has(102)&&has(30)&&has(32),"home actions and literary discoveries");
            for(size_t i=0;i<LITERARY_NOTE_COUNT;i++){
                shelf.open=true;shelf.discover(i);focus=40;
                render(size.first,size.second);
                require(hits.size()==5&&has(40)&&has(41)&&has(42)&&has(43)&&has(44),"shelf action isolation");
                require(!has(1)&&!has(100)&&!has(30),"shelf blocks background actions");
                // Check every excerpt and its provenance fit their real layout boxes.
                Bitmap scratch(1,1);Graphics measure(&scratch);
                const auto& n=literaryNotes()[i];FontFamily family(L"Segoe UI");Font font(&family,26,FontStyleBold,UnitPixel);StringFormat fmt;
                RectF bounds;int fitted=0,lines=0;
                auto quote=L"“"+n.text+L"”";
                measure.MeasureString(quote.c_str(),-1,&font,RectF(0,0,622,115),&fmt,&bounds,&fitted,&lines);
                require(fitted==int(quote.size()),"full shelf quote fits without clipping");
                Font small(&family,14,FontStyleRegular,UnitPixel);quote=L"“"+n.text+L"”";
                measure.MeasureString(quote.c_str(),-1,&small,RectF(0,0,152,125),&fmt,&bounds,&fitted,&lines);
                require(fitted==int(quote.size()),"full sidebar quote fits without clipping");
            }
            shelf.open=false;
            for(int t=0;t<3;t++)for(int target:{0,5,8,10}){
                app=AppModel();app.action(100+t,0);app.action(1,1000);
                for(int i=0;i<10;i++){
                    focus=10;render(size.first,size.second);
                    require(!has(2)&&has(20)&&has(3)==(i>0),"unanswered controls and previous boundary");
                    app.pause(2000+i*1000);render(size.first,size.second);
                    require(hits.size()==2&&has(21)&&has(22)&&!has(10)&&!has(2),"pause hides all background actions");
                    app.action(21,2000+i*1000);
                    int answer=topics[t].questions[i].correct;
                    if(i>=target)answer=(answer+1)%4;
                    app.action(10+answer,2000+i*1000);render(size.first,size.second);
                    require(has(2),"selected answer enables continue");
                    app.action(2,2000+i*1000);
                }
                for(int id=200;id<210;id++){focus=id;render(size.first,size.second);require(has(id),"answer trail focus reachable");}
                require(has(4)&&has(5)&&has(6)&&has(16)&&!has(2),"results actions and export");
                std::vector<int> order;for(const auto&h:hits)order.push_back(h.id);
                require(nextFocus(order,5,false)==16&&nextFocus(order,16,false)==6,"results Tab order follows left-to-right button order");
                app.action(4,0);render(size.first,size.second);
                require(!has(7)&&has(8)&&has(9)&&has(15)==(target<10),"review boundaries and perfect filter disabled");
                if(target<10){app.action(15,0);render(size.first,size.second);require(!has(7)&&has(8)==(target<9),"missed-only navigation");}
                // The same shelf overlay must isolate controls on results and review.
                shelf.open=true;render(size.first,size.second);require(hits.size()==5&&!has(9)&&!has(8),"review shelf isolation");shelf.open=false;
            }
        }
        std::cout<<"PASS: "<<renderChecks<<" view checks; 3 sizes, all topics, results, literary notes, full quote fitting, focus, bounds and modal isolation.\n";
    }catch(const std::exception& e){std::cerr<<"FAILED: "<<e.what()<<'\n';result=1;}
    GdiplusShutdown(token);return result;
}
