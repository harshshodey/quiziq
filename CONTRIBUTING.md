# Contributing to QuizIQ

Bug reports, focused fixes, accessibility improvements and question corrections are welcome.

1. Fork the repository and create a branch for one change.
2. Build on Windows with a C++17 compiler using `build.bat` or CMake.
3. Run all three test programs. With CMake, use `ctest --test-dir build -C Release --output-on-failure`.
4. For interface changes, include a screenshot and check the keyboard controls and smaller window sizes.
5. Open a pull request describing the problem, the resulting behavior and the checks you ran.

Question banks currently contain ten questions per topic. Preserve that count, include four distinct options, use a zero-based correct-answer index, and explain why the answer is correct.

Literary notes must remain verbatim and attributed. Update both `quote-sources.json` and `src/literary_notes.h`, include a direct original-text source, identify excerpts and fictional speakers, and run `tools/verify_sources.ps1`. Do not substitute invented sayings or unsupported attributions.

Do not commit compiled executables, credentials, personal score files or local build directories. Contributions to original project code are provided under the project's MIT license; literary material retains the attribution described in `THIRD_PARTY_NOTICES.md`.
