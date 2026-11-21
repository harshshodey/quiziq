#pragma once
#include <array>
#include <string>
#include <vector>
#include <stdexcept>

struct Question {
    std::wstring prompt;
    std::array<std::wstring, 4> options;
    int correct;
    std::wstring explanation;
};
struct Topic { std::wstring title, subtitle, symbol; std::vector<Question> questions; };
inline std::vector<Topic> makeTopics() {
    return {
      {L"C++ Essentials", L"Think in code. Build your foundations.", L"C++", {
        {L"Which header provides std::cout?", {L"<iostream>",L"<vector>",L"<string>",L"<cmath>"},0,L"The <iostream> header declares the standard input and output streams, including std::cout."},
        {L"Which keyword prevents a variable from being modified?",{L"static",L"const",L"mutable",L"virtual"},1,L"A const-qualified variable cannot be modified through its name after initialization."},
        {L"What is the index of the first element in a C++ array?",{L"1",L"-1",L"0",L"It depends on the size"},2,L"C++ arrays use zero-based indexing: the first element is at index 0."},
        {L"Which container can grow dynamically?",{L"A built-in array",L"std::array",L"An enum",L"std::vector"},3,L"std::vector is a dynamic sequence container that can resize as elements are added."},
        {L"What does a constructor do?",{L"Initializes an object",L"Destroys an object",L"Imports a namespace",L"Catches an exception"},0,L"A constructor initializes an object when it is created. Its name matches the class name."},
        {L"Which operator accesses a member through a pointer?",{L"::",L"->",L".",L"&"},1,L"The -> operator accesses a member through a pointer. The dot operator accesses a member on an object."},
        {L"Which loop always runs its body at least once?",{L"for",L"while",L"do-while",L"None of these"},2,L"A do-while loop checks its condition after executing its body, so the body runs at least once."},
        {L"What does RAII tie resource lifetime to?",{L"Global variables",L"The compiler version",L"A fixed timer",L"Object lifetime"},3,L"Resource Acquisition Is Initialization ties resource ownership to object lifetime, releasing resources in destructors."},
        {L"Which keyword is used to throw an exception?",{L"throw",L"try",L"catch",L"return"},0,L"throw signals an exception; a matching catch block can handle it."},
        {L"Which smart pointer represents exclusive ownership?",{L"std::shared_ptr",L"std::unique_ptr",L"std::weak_ptr",L"A raw pointer"},1,L"std::unique_ptr owns its object exclusively and releases it automatically when the pointer is destroyed."}
      }},
      {L"Science & Space",L"Explore the ideas behind our universe.",L"S",{
        {L"Which planet is known as the Red Planet?",{L"Mars",L"Venus",L"Jupiter",L"Mercury"},0,L"Iron oxide on the surface gives Mars its reddish appearance."},
        {L"What is the chemical symbol for gold?",{L"Ag",L"Au",L"Fe",L"Go"},1,L"Gold's symbol is Au, from the Latin word aurum."},
        {L"Which force keeps planets in orbit around the Sun?",{L"Friction",L"Magnetism",L"Gravity",L"Buoyancy"},2,L"Gravity is the attractive force between masses and keeps planets in orbit."},
        {L"Which organelle is a major producer of ATP in cells?",{L"Nucleus",L"Ribosome",L"Golgi apparatus",L"Mitochondrion"},3,L"Mitochondria produce much of a typical aerobic eukaryotic cell's ATP through cellular respiration."},
        {L"What is the SI unit of electrical resistance?",{L"Ohm",L"Volt",L"Ampere",L"Watt"},0,L"Resistance is measured in ohms. Voltage, current, and power have different units."},
        {L"What gas do plants absorb during photosynthesis?",{L"Helium",L"Carbon dioxide",L"Hydrogen",L"Neon"},1,L"Plants use carbon dioxide and water, with light energy, to form sugars."},
        {L"Which travels fastest in a vacuum?",{L"Sound",L"An airplane",L"Light",L"An ocean wave"},2,L"Light travels through a vacuum at approximately 299,792,458 metres per second."},
        {L"What is the largest planet in our solar system?",{L"Earth",L"Neptune",L"Saturn",L"Jupiter"},3,L"Jupiter is the largest planet in the solar system by both mass and volume."},
        {L"What particle carries a negative electric charge?",{L"Electron",L"Proton",L"Neutron",L"Photon"},0,L"Electrons are negatively charged; protons are positive and neutrons are neutral."},
        {L"At standard atmospheric pressure, water boils at...",{L"0 degrees C",L"100 degrees C",L"50 degrees C",L"200 degrees C"},1,L"At one standard atmosphere of pressure, pure water boils at 100 degrees Celsius."}
      }},
      {L"World Explorer",L"Find your way through a world of knowledge.",L"W",{
        {L"Which is the largest ocean on Earth?",{L"Pacific",L"Atlantic",L"Indian",L"Arctic"},0,L"The Pacific Ocean is the largest and deepest of Earth's ocean basins."},
        {L"What is the capital of Japan?",{L"Kyoto",L"Tokyo",L"Osaka",L"Nagoya"},1,L"Tokyo is Japan's capital. Kyoto was its imperial capital for many centuries."},
        {L"On which continent is the Sahara Desert?",{L"Asia",L"Australia",L"Africa",L"Europe"},2,L"The Sahara stretches across much of North Africa."},
        {L"Which country is home to Machu Picchu?",{L"Mexico",L"Chile",L"Bolivia",L"Peru"},3,L"Machu Picchu is an Inca site in the Andes mountains of Peru."},
        {L"What is the currency of Japan?",{L"Yen",L"Won",L"Yuan",L"Baht"},0,L"Japan's currency is the yen; its currency code is JPY."},
        {L"Which river flows through Paris?",{L"Thames",L"Seine",L"Danube",L"Rhine"},1,L"The Seine flows through Paris and into the English Channel."},
        {L"Which is the smallest continent by land area?",{L"Europe",L"Antarctica",L"Australia",L"South America"},2,L"In the seven-continent model, Australia is the smallest continent by land area."},
        {L"The Great Barrier Reef lies off the coast of...",{L"Brazil",L"India",L"South Africa",L"Australia"},3,L"The Great Barrier Reef lies in the Coral Sea off Queensland, Australia."},
        {L"Which mountain is highest above sea level?",{L"Mount Everest",L"K2",L"Kilimanjaro",L"Mont Blanc"},0,L"Mount Everest has the highest summit above mean sea level."},
        {L"Which country is shaped like a boot?",{L"Greece",L"Italy",L"Spain",L"Portugal"},1,L"The Italian peninsula is famously described as boot-shaped."}
      }}
    };
}
class QuizSession {
public:
    std::vector<int> answers;
    int current = 0;
    explicit QuizSession(size_t count=10): answers(count,-1) {}
    void select(int option) { if(option<0||option>3) throw std::out_of_range("option"); answers.at(current)=option; }
    int answered() const { int n=0; for(int a:answers) n+=a>=0 && a<4; return n; }
    bool complete() const { return !answers.empty() && answered()==int(answers.size()); }
    bool canAdvance() const { return current>=0 && size_t(current)<answers.size() && answers[current]>=0 && answers[current]<4; }
    bool next() { if(!canAdvance() || size_t(current+1)>=answers.size()) return false; ++current; return true; }
    bool previous() { if(current<=0 || size_t(current)>=answers.size()) return false; --current; return true; }
    int score(const std::vector<Question>& qs) const {
        if(qs.size()!=answers.size()) throw std::invalid_argument("question count");
        int n=0; for(size_t i=0;i<qs.size();++i) n+=answers[i]>=0 && answers[i]<4 && answers[i]==qs[i].correct; return n;
    }
    int percent(const std::vector<Question>& qs) const {int points=score(qs);return qs.empty()?0:100*points/int(qs.size());}
};
