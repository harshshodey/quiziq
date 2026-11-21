#pragma once
// Verified verbatim excerpts. Full provenance is in quote-sources.json.
#include <array>
#include <string>
struct LiteraryNote { std::wstring text,author,work,location,url; };
constexpr size_t LITERARY_NOTE_COUNT=16;
inline const std::array<LiteraryNote,LITERARY_NOTE_COUNT>& literaryNotes(){
    static const std::array<LiteraryNote,LITERARY_NOTE_COUNT> notes{{
        {L"Curiouser and curiouser!",L"Lewis Carroll",L"Alice’s Adventures in Wonderland",L"Chapter II · Alice speaking",L"https://standardebooks.org/ebooks/lewis-carroll/alices-adventures-in-wonderland/john-tenniel/text/chapter-2"},
        {L"Begin at the beginning",L"Lewis Carroll",L"Alice’s Adventures in Wonderland",L"Chapter XII · the King speaking · excerpt",L"https://standardebooks.org/ebooks/lewis-carroll/alices-adventures-in-wonderland/john-tenniel/text/chapter-12"},
        {L"Why is a raven like a writing-desk?",L"Lewis Carroll",L"Alice’s Adventures in Wonderland",L"Chapter VII · the Hatter’s riddle",L"https://standardebooks.org/ebooks/lewis-carroll/alices-adventures-in-wonderland/john-tenniel/text/chapter-7"},
        {L"It’s a poor sort of memory that only works backwards",L"Lewis Carroll",L"Through the Looking-Glass",L"Chapter V · the White Queen speaking · excerpt",L"https://standardebooks.org/ebooks/lewis-carroll/through-the-looking-glass/john-tenniel/text/chapter-5"},
        {L"Why, sometimes I’ve believed as many as six impossible things before breakfast.",L"Lewis Carroll",L"Through the Looking-Glass",L"Chapter V · the White Queen speaking",L"https://standardebooks.org/ebooks/lewis-carroll/through-the-looking-glass/john-tenniel/text/chapter-5"},
        {L"The truth is rarely pure and never simple.",L"Oscar Wilde",L"The Importance of Being Earnest",L"Act I · Algernon speaking",L"https://standardebooks.org/ebooks/oscar-wilde/the-importance-of-being-earnest/text/act-1"},
        {L"It is awfully hard work doing nothing.",L"Oscar Wilde",L"The Importance of Being Earnest",L"Act I · Algernon speaking",L"https://standardebooks.org/ebooks/oscar-wilde/the-importance-of-being-earnest/text/act-1"},
        {L"Training is everything.",L"Mark Twain",L"Pudd’nhead Wilson",L"Chapter V · Pudd’nhead Wilson’s Calendar · excerpt",L"https://standardebooks.org/ebooks/mark-twain/puddnhead-wilson/text/chapter-5"},
        {L"cauliflower is nothing but cabbage with a college education.",L"Mark Twain",L"Pudd’nhead Wilson",L"Chapter V · Pudd’nhead Wilson’s Calendar · excerpt",L"https://standardebooks.org/ebooks/mark-twain/puddnhead-wilson/text/chapter-5"},
        {L"I like work: it fascinates me. I can sit and look at it for hours.",L"Jerome K. Jerome",L"Three Men in a Boat",L"Chapter XV · narrator speaking · excerpt",L"https://standardebooks.org/ebooks/jerome-k-jerome/three-men-in-a-boat/text/chapter-15"},
        {L"Though this be madness, yet there is method in’t.",L"William Shakespeare",L"Hamlet",L"Act II, scene II · Polonius speaking",L"https://standardebooks.org/ebooks/william-shakespeare/hamlet/text/act-2"},
        {L"brevity is the soul of wit",L"William Shakespeare",L"Hamlet",L"Act II, scene II · Polonius speaking · excerpt",L"https://standardebooks.org/ebooks/william-shakespeare/hamlet/text/act-2"},
        {L"Words, words, words.",L"William Shakespeare",L"Hamlet",L"Act II, scene II · Hamlet speaking",L"https://standardebooks.org/ebooks/william-shakespeare/hamlet/text/act-2"},
        {L"I can resist everything except temptation.",L"Oscar Wilde",L"Lady Windermere’s Fan",L"Act I · Lord Darlington speaking",L"https://standardebooks.org/ebooks/oscar-wilde/lady-windermeres-fan/text/act-1"},
        {L"we are all in the gutter, but some of us are looking at the stars.",L"Oscar Wilde",L"Lady Windermere’s Fan",L"Act III · Lord Darlington speaking · excerpt",L"https://standardebooks.org/ebooks/oscar-wilde/lady-windermeres-fan/text/act-3"},
        {L"For what do we live but to make sport for our neighbours, and laugh at them in our turn?",L"Jane Austen",L"Pride and Prejudice",L"Chapter LVII · Mr. Bennet speaking",L"https://standardebooks.org/ebooks/jane-austen/pride-and-prejudice/text/chapter-57"},
    }}; return notes;
}
