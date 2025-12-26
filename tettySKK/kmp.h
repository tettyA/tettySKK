#pragma once
#include <vector>
#include <unordered_map>
#include <string>

//‘÷‰¹
#define SAMEHAND 1
//´‰¹
#define ANOTHERHAND 0


#define ex_smalltu  (540001)
#define ex_nn       (540002)
#define ex_youon    (540003)
#define ex_boin (100008)
#define ex_sakujo (540004)

using  KeyCode = int;
struct KMP {
    enum class Dir {
        None, L, R,
    };
    struct KMPelm {
        enum class Cmd
        {
            Send,
            DSend
        };

        Cmd cmd;
        KMP::Dir dir;
        KeyCode to;
        KeyCode tootherdir;//ˆá‚¤è‚Å‰Ÿ‚µ‚½
        KeyCode secboin;//“ñ“x–Ú‚Ì•ê‰¹
    };

    struct SpecialKeyStates {
        bool sokuon;
        bool youon;
        bool shift;
    }is_key_pushed;

    std::unordered_map<KeyCode, KMPelm>KMPs;
    
};

