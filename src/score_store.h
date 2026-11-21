#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <array>
#include <filesystem>
#include <fstream>
#include <algorithm>

using BestScores=std::array<int,3>;
inline BestScores loadScores(const std::filesystem::path& file) {
    BestScores values{-1,-1,-1},parsed{};
    std::ifstream in(file);
    for(int& n:parsed) if(!(in>>n)||n< -1||n>100)return values;
    in>>std::ws;
    return in.eof()?parsed:values;
}
inline bool saveScores(const std::filesystem::path& file,const BestScores& scores) {
    if(file.empty())return false;
    for(int n:scores)if(n< -1||n>100)return false;
    auto temp=file;temp+=L"."+std::to_wstring(GetCurrentProcessId())+L".tmp";
    {
        std::ofstream out(temp,std::ios::trunc);
        if(!out)return false;
        for(int n:scores)out<<n<<' ';
        out<<'\n';out.close();
        if(!out){std::error_code ec;std::filesystem::remove(temp,ec);return false;}
    }
    // Replace only after a complete write, preserving the previous file on failure.
    bool ok=MoveFileExW(temp.c_str(),file.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
    if(!ok){std::error_code ec;std::filesystem::remove(temp,ec);}
    return ok;
}
