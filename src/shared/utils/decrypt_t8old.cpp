#include <includes_shared.hpp>
#include "decrypt_t8old.hpp"

#define HIDWORD(x) (((x) >> 32) & 0xFFFFFFFF)

// idc
#pragma warning(push)
#pragma warning(disable:6385)
#pragma warning(disable:4244)
#pragma warning(disable:4101)

static byte* DecryptStringT8Old0(byte* str) {
#pragma region Vars
    byte* v1; // r15
    __int64 v2; // rcx
    byte* v3; // r11
    unsigned __int64 v4; // r9
    unsigned __int64 v5; // r8
    char v6; // r10
    unsigned int v7; // eax
    unsigned int v8; // edx
    int v9; // eax
    byte* v10; // r11
    unsigned __int64 v11; // r9
    unsigned __int64 v12; // r8
    char v13; // r10
    unsigned int v14; // eax
    unsigned int v15; // edx
    int v16; // eax
    byte* v17; // r11
    unsigned __int64 v18; // r9
    unsigned __int64 v19; // r8
    char v20; // r10
    unsigned int v21; // eax
    unsigned int v22; // edx
    int v23; // eax
    byte* v24; // r11
    unsigned __int64 v25; // r9
    char v26; // r10
    unsigned int v27; // eax
    unsigned __int64 v28; // r8
    unsigned int v29; // edx
    int v30; // eax
    byte* v31; // r11
    unsigned __int64 v32; // r9
    char v33; // r10
    unsigned int v34; // eax
    unsigned __int64 v35; // r8
    unsigned int v36; // edx
    int v37; // eax
    byte* v38; // r11
    unsigned __int64 v39; // r9
    char v40; // r10
    unsigned int v41; // eax
    unsigned __int64 v42; // r8
    unsigned int v43; // edx
    int v44; // eax
    byte* v45; // r11
    unsigned __int64 v46; // r9
    unsigned __int64 v47; // r8
    char v48; // r10
    unsigned int v49; // eax
    unsigned int v50; // edx
    int v51; // eax
    byte* v52; // r11
    unsigned __int64 v53; // r9
    unsigned __int64 v54; // r8
    char v55; // r10
    unsigned int v56; // eax
    unsigned int v57; // edx
    int v58; // eax
    char v59; // r8
    __int64 v60; // rax
    byte* v61; // r11
    unsigned __int64 v62; // r9
    char v63; // r10
    unsigned int v64; // eax
    unsigned __int64 v65; // r8
    unsigned int v66; // edx
    int v67; // eax
    unsigned __int64 v68; // r8
    byte* v69; // r11
    unsigned __int64 v70; // r9
    char v71; // r10
    unsigned int v72; // eax
    unsigned __int64 v73; // r8
    unsigned int v74; // edx
    int v75; // eax
    char v76; // r8
    byte* v77; // r11
    unsigned __int64 v78; // r9
    unsigned __int64 v79; // r8
    char v80; // r10
    unsigned int v81; // eax
    unsigned int v82; // edx
    int v83; // eax
    byte* v84; // r11
    unsigned __int64 v85; // r9
    char v86; // r10
    unsigned int v87; // eax
    unsigned __int64 v88; // r8
    unsigned int v89; // edx
    int v90; // eax
    byte* v91; // r11
    unsigned __int64 v92; // r9
    unsigned __int64 v93; // r8
    char v94; // r10
    unsigned int v95; // eax
    unsigned int v96; // edx
    int v97; // eax
    char v98; // r8
    byte* v99; // rdx
    byte* v100; // r11
    unsigned __int64 v101; // r9
    char v102; // r10
    unsigned int v103; // eax
    unsigned __int64 v104; // r8
    unsigned int v105; // edx
    int v106; // eax
    byte* v107; // r11
    unsigned __int64 v108; // r9
    char v109; // r10
    unsigned int v110; // eax
    unsigned __int64 v111; // r8
    unsigned int v112; // edx
    int v113; // eax
    char v114; // r8
    byte* v115; // rdx
    byte* v116; // r11
    unsigned __int64 v117; // r9
    char v118; // r10
    unsigned int v119; // eax
    unsigned __int64 v120; // r8
    unsigned int v121; // edx
    int v122; // eax
    unsigned __int64 v123; // r8
    byte* v124; // rbx
    signed __int64 v125; // rcx
    signed __int64 v127; // rcx

#pragma endregion

    v1 = str;
    if ((*str & 0xC0) != 0x80)
        return v1;
    //sub_232DFE0(&qword_10748F50);
    switch (*v1)
    {
    case 0x80:
    case 0x85:
    case 0x8C:
    case 0x9F:
    case 0xA4:
        goto LABEL_1190;
    case 0x82:
        *v1 = 0x80;
        v3 = v1 + 1;
        v4 = 0i64;
        v5 = 0i64;
        v6 = 0;
        v7 = -931419029;
        v2 = 2176146861i64;
        do
        {
            if (v7 > 0x883B8359)
            {
                switch (v7)
                {
                case 0xAEAE34E0:
                    v4 = 0x9F314572A2178897ui64;
                    v2 = 535442350i64;
                    break;
                case 0xC25520EE:
                    v4 = 0xC14C9BB62DBAD0Ci64;
                    v2 = 817621575i64;
                    break;
                case 0xC87BAC6B:
                    v4 = 0xE336CFFA4D690CD6ui64;
                    v2 = 154653372i64;
                    break;
                case 0xF7448E90:
                    v4 = 0x44AA694EA8C4234Fi64;
                    v2 = 895562544i64;
                    break;
                }
            }
            else
            {
                switch (v7)
                {
                case 0x883B8359:
                    v4 = 0x4000BDDFBE0C2F6i64;
                    v2 = 2994962370i64;
                    break;
                case 0xD7CF59u:
                    v4 = 0xDEFCE48D4F06228i64;
                    v2 = 3783252980i64;
                    break;
                case 0xD1B3E45u:
                    v4 = 0x42D1DB70C656FFEi64;
                    v2 = 1340534035i64;
                    break;
                case 0x1CE0A886u:
                    v4 = 0xB15933E21A8F56D5ui64;
                    v2 = 3451920053i64;
                    break;
                case 0x441312E9u:
                    v4 = 0xA66E5BD4F54B24B6ui64;
                    v2 = 857896840i64;
                    break;
                case 0x499F86D4u:
                    v4 = 0xAEF6721DCFFCC0ABui64;
                    v2 = 74327999i64;
                    break;
                }
            }
            v7 ^= v2;
        } while (v7 != -1051951401);
        v1 = str;
        if (!*v3)
            goto LABEL_1190;
        v2 = 0x8FDA4E3CB979DEFi64;
        do
        {
            v8 = -76377883;
            v9 = 233675035;
            do
            {
                if (v8 > 0x62AD5CE0)
                {
                    if (v8 > 0xA874CFA9)
                    {
                        switch (v8)
                        {
                        case 0xBD13E2E1:
                            v6 = (v5 ^ (v5 >> 5)) & 0xE;
                            v9 = -1078355130;
                            break;
                        case 0xC8665E2E:
                            v5 = 0xBF58476D1CE4E5B9ui64 * ((v5 >> 30) ^ v5);
                            v9 = -1050493731;
                            break;
                        case 0xD0B7384C:
                            v4 += 0x8FDA4E3CB979DEFi64;
                            v5 = v4;
                            v9 = 1553898958;
                            break;
                        case 0xD672D0A1:
                            v5 = 0xEDE86169219670BFui64 * ((v5 >> 15) ^ v5);
                            v9 = 1902091748;
                            break;
                        case 0xFB7290E5:
                            v4 -= 0x61C8864680B583EBi64;
                            v5 = v4;
                            v9 = 857001675;
                            break;
                        }
                    }
                    else
                    {
                        switch (v8)
                        {
                        case 0xA874CFA9:
                            v5 = 0x9E3779B97F4A7C15ui64 * ((v5 >> 27) ^ v5);
                            v9 = 448576545;
                            break;
                        case 0x6FB9CDDEu:
                            v6 = v5 ^ (v5 >> 31);
                            v9 = -879872685;
                            break;
                        case 0x84307A51:
                            v6 = (v5 ^ (v5 >> 31)) & 0x24;
                            v9 = 85628730;
                            break;
                        case 0x95DA9F96:
                            v6 = v5 ^ (v5 >> 31);
                            v9 = 207362343;
                            break;
                        case 0x9748A5BF:
                            v5 = 0xD0A786A65D9D0F2i64 * ((v5 >> 27) ^ v5);
                            v9 = -1162892232;
                            break;
                        case 0x998686B1:
                            *v3 = RotateRight8(*v3, v6);
                            v9 = -81012143;
                            v2 = 0x8FDA4E3CB979DEFi64;
                            break;
                        }
                    }
                }
                else if (v8 == 1655528672)
                {
                    ++v3;
                    v9 = -1598668233;
                }
                else if (v8 > 0x165C815B)
                {
                    switch (v8)
                    {
                    case 0x3D5BCAEFu:
                        --v3;
                        v9 = -570944893;
                        break;
                    case 0x56545113u:
                        v5 = 0xA343F5DE4EC297FDui64 * ((v5 >> 43) ^ v5);
                        v9 = 1405329268;
                        break;
                    case 0x59344317u:
                        v4 -= 0x6D0592BBCC59D2E5i64;
                        v5 = v4;
                        v9 = -782582612;
                        break;
                    }
                }
                else
                {
                    switch (v8)
                    {
                    case 0x165C815Bu:
                        *v3 = RotateLeft8(*v3, v6);
                        v9 = -1280872253;
                        v2 = 0x8FDA4E3CB979DEFi64;
                        break;
                    case 0x85A01Cu:
                        v4 -= 0x40A7B892E31B1A47i64;
                        v5 = v4;
                        v9 = -1107614337;
                        break;
                    case 0x900531Au:
                        v5 = 0x94D049BB133111EBui64 * ((v5 >> 30) ^ v5);
                        v9 = -1515136564;
                        break;
                    case 0x904E2F3u:
                        v5 = 0x94D049BB133111EBui64 * ((v5 >> 27) ^ v5);
                        v9 = -1663140507;
                        break;
                    case 0xC44C3A0u:
                        v5 = 0xA887A1B3492DEE1i64 * ((v5 >> 30) ^ v5);
                        v9 = 236694821;
                        break;
                    }
                }
                v8 ^= v9;
            } while (v8 != -1038410025);
        } while (*v3);
        goto LABEL_1189;
    case 0x86:
        *v1 = -123;
        v10 = v1 + 1;
        v11 = 0i64;
        v12 = 0i64;
        v13 = 0;
        v14 = 3925385;
        v2 = 107663353i64;
        do
        {
            if (v14 > 0x431792DC)
            {
                switch (v14)
                {
                case 0x8F2375B6:
                    v11 = 0x24F95C879E71C72Ei64;
                    v2 = 130653061i64;
                    break;
                case 0xD4E1E0F4:
                    v11 = 0x3CA183D44E8BB2DDi64;
                    v2 = 3314089290i64;
                    break;
                case 0xE2A48176:
                    v11 = 0x20DC3C9DCFFE386Di64;
                    v2 = 4213917097i64;
                    break;
                case 0xF1F7D318:
                    v11 = 0x42CDD6C535B93C4Di64;
                    v2 = 2563345169i64;
                    break;
                }
            }
            else
            {
                switch (v14)
                {
                case 0x431792DCu:
                    v11 = 0x282107D42F4AC8DFi64;
                    v2 = 4030761645i64;
                    break;
                case 0x3BE589u:
                    v11 = 0xE18735E5D0C44133ui64;
                    v2 = 1366432599i64;
                    break;
                case 0xE9332FDu:
                    v11 = 0x5811F94E6E544BAFi64;
                    v2 = 110970654i64;
                    break;
                case 0x21A73C78u:
                    v11 = 0x2F144FAE613A40EEi64;
                    v2 = 757540304i64;
                    break;
                case 0x22B1E497u:
                    v11 = 0xAF74376082480783ui64;
                    v2 = 1758405307i64;
                    break;
                case 0x259CE9E9u:
                    v11 = 0x7B705DED5E70F59Fi64;
                    v2 = 485925865i64;
                    break;
                }
            }
            v14 ^= v2;
        } while (v14 != 1363803870);
        v1 = str;
        if (!*v10)
            goto LABEL_1190;
        v2 = 0xD2265A27524C6F3i64;
        while (1)
        {
            v15 = -713050806;
            v16 = 1721079514;
            do
            {
                if (v15 > 0x70A9598B)
                {
                    if (v15 <= 0xA94D62FA)
                    {
                        switch (v15)
                        {
                        case 0xA94D62FA:
                            v12 = 0xB779BEDE97B23Fi64 * ((v12 >> 34) ^ v12);
                            v16 = -1319504595;
                            goto LABEL_145;
                        case 0x72A57FCFu:
                            ++v10;
                            v16 = -1976269572;
                            goto LABEL_145;
                        case 0x828FEB5A:
                            v13 = (v12 ^ (v12 >> 45)) & 5;
                            v16 = 606712102;
                            goto LABEL_145;
                        case 0x82DADEEF:
                            v12 = 0x9E3779B97F4A7C15ui64 * ((v12 >> 27) ^ v12);
                            v16 = -1259240631;
                            goto LABEL_145;
                        case 0x9BEA4D82:
                            --v10;
                            v16 = 851607910;
                            goto LABEL_145;
                        }
                        if (v15 != -1589028017)
                            goto LABEL_145;
                        v11 -= 0x40A7B892E31B1A47i64;
                        v16 = -68438974;
                        goto LABEL_144;
                    }
                    switch (v15)
                    {
                    case 0xB3F79A7F:
                        v11 += 0xD2265A27524C6F3i64;
                        v16 = 888808851;
                        goto LABEL_144;
                    case 0xB98F3078:
                        v13 = v12 ^ (v12 >> 31);
                        v16 = -1344167694;
                        break;
                    case 0xD3105BCA:
                        v12 = 0xBF58476D1CE4E5B9ui64 * ((v12 >> 30) ^ v12);
                        v16 = -6765661;
                        break;
                    case 0xD57FB54A:
                        v11 -= 0x61C8864680B583EBi64;
                        v16 = 107998848;
                        goto LABEL_144;
                    case 0xF5318D2E:
                        v13 = (v12 ^ (v12 >> 31)) & 0xF;
                        v16 = 240084261;
                        break;
                    }
                }
                else
                {
                    if (v15 == 1890146699)
                    {
                        v12 = 0xF89696CA5355C33i64 * ((v12 >> 30) ^ v12);
                        v16 = 1690473147;
                        goto LABEL_145;
                    }
                    if (v15 > 0x2C889869)
                    {
                        switch (v15)
                        {
                        case 0x5471DDD1u:
                            v11 += 0x329CAB636CA9789Ci64;
                            v16 = 2079540768;
                        LABEL_144:
                            v12 = v11;
                            break;
                        case 0x62EDDABFu:
                            *v10 = RotateLeft8(*v10, v13);
                            v16 = 265757373;
                            v2 = 0xD2265A27524C6F3i64;
                            break;
                        case 0x6EF99710u:
                            v12 = 0xDA782E8B485E667Cui64 * ((v12 >> 33) ^ v12);
                            v16 = -151375374;
                            break;
                        }
                    }
                    else
                    {
                        switch (v15)
                        {
                        case 0x2C889869u:
                            v12 = 0x94D049BB133111EBui64 * ((v12 >> 27) ^ v12);
                            v16 = 598184820;
                            break;
                        case 0x8BC736Fu:
                            v12 = 0x5229A71C5507AB8i64 * ((v12 >> 27) ^ v12);
                            v16 = 12288102;
                            break;
                        case 0xBFC1D3Cu:
                            *v10 = RotateRight8(*v10, v13);
                            v16 = 2035901171;
                            v2 = 0xD2265A27524C6F3i64;
                            break;
                        case 0xF2F0B1Du:
                            v13 = v12 ^ (v12 >> 31);
                            v16 = 80942625;
                            break;
                        case 0xFFD2FD5u:
                            v12 = 0x94D049BB133111EBui64 * ((v12 >> 30) ^ v12);
                            v16 = -1496109207;
                            break;
                        }
                    }
                }
            LABEL_145:
                v15 ^= v16;
            } while (v15 != -124650701);
            if (!*v10)
                goto LABEL_1189;
        }
    case 0x89:
        *v1 = -116;
        v17 = v1 + 1;
        v18 = 0i64;
        v19 = 0i64;
        v20 = 0;
        v21 = -551500727;
        v2 = 1540563594i64;
        do
        {
            if (v21 > 0x5E6343C6)
            {
                switch (v21)
                {
                case 0x72E19960u:
                    v18 = 0xAD13EC8772126BA8ui64;
                    v2 = 3040326504i64;
                    break;
                case 0x8BEDFC77:
                    v18 = 0x29406C76840B842Di64;
                    v2 = 3413582837i64;
                    break;
                case 0xC88CAB76:
                    v18 = 0x315F25A280DB4E25i64;
                    v2 = 1768596334i64;
                    break;
                case 0xDF20C449:
                    v18 = 0x71F24E234B4AE5C1i64;
                    v2 = 3178008743i64;
                    break;
                }
            }
            else
            {
                switch (v21)
                {
                case 0x5E6343C6u:
                    v18 = 0x25BFBE5DD7089A00i64;
                    v2 = 2705825371i64;
                    break;
                case 0x457DE4u:
                    v18 = 0x2CD9251F75A28F7Ei64;
                    v2 = 130860769i64;
                    break;
                case 0x622C3DDu:
                    v18 = 0x240E7D6318AFD1EBi64;
                    v2 = 198724257i64;
                    break;
                case 0x77DD954u:
                    v18 = 0x95876DE106B1C0D1ui64;
                    v2 = 606727200i64;
                    break;
                case 0x4594DCD8u:
                    v18 = 0x251E32DCBF69DC92i64;
                    v2 = 83461094i64;
                    break;
                case 0x5D872DCAu:
                    v18 = 0x23CF6EA50428FE78i64;
                    v2 = 3747239488i64;
                    break;
                }
            }
            v21 ^= v2;
        } while (v21 != 1649169646);
        v1 = str;
        if (!*v17)
            goto LABEL_1190;
        v2 = 0x9CB81477B161416i64;
        do
        {
            v22 = 253934731;
            v23 = -1030963178;
            do
            {
                if (v22 > 0x5BF7FE46)
                {
                    if (v22 > 0x6BAC7148)
                    {
                        switch (v22)
                        {
                        case 0x8F2375A5:
                            v19 = 0xD84C1A36101682FDui64 * ((v19 >> 30) ^ v19);
                            v23 = 208938875;
                            break;
                        case 0xD740E219:
                            v20 = (v19 ^ (v19 >> 31)) & 9;
                            v23 = -928756590;
                            break;
                        case 0xE4C666E4:
                            v18 -= 0x40A7B892E31B1A47i64;
                            v19 = v18;
                            v23 = 682011842;
                            break;
                        case 0xF73DD25C:
                            v19 = 0x94D049BB133111EBui64 * ((v19 >> 27) ^ v19);
                            v23 = -1668177132;
                            break;
                        }
                    }
                    else
                    {
                        switch (v22)
                        {
                        case 0x6BAC7148u:
                            v20 = v19 ^ (v19 >> 31);
                            v23 = 984259460;
                            break;
                        case 0x5CFF54F0u:
                            v19 = 0xFB8E9060CB688E86ui64 * ((v19 >> 38) ^ v19);
                            v23 = -1532565133;
                            break;
                        case 0x5F40F87Bu:
                            ++v17;
                            v23 = -668488685;
                            break;
                        case 0x63A9A26Bu:
                            v20 = (v19 ^ (v19 >> 50)) & 0x3E;
                            v23 = 195553769;
                            break;
                        case 0x66EF3477u:
                            v19 = 0x94D049BB133111EBui64 * ((v19 >> 30) ^ v19);
                            v23 = -756263740;
                            break;
                        }
                    }
                }
                else if (v22 == 1542979142)
                {
                    v19 = 0xBF58476D1CE4E5B9ui64 * ((v19 >> 30) ^ v19);
                    v23 = -1396036582;
                }
                else if (v22 > 0xE301330)
                {
                    switch (v22)
                    {
                    case 0xF22BC8Bu:
                        v18 -= 0x61C8864680B583EBi64;
                        v19 = v18;
                        v23 = 1423262413;
                        break;
                    case 0x1E2AA08Bu:
                        v18 += 0x9CB81477B161416i64;
                        v19 = v18;
                        v23 = -834636385;
                        break;
                    case 0x5106EACCu:
                        *v17 = RotateRight8(*v17, v20);
                        v23 = 239473335;
                        v2 = 0x9CB81477B161416i64;
                        break;
                    case 0x5249A8C7u:
                        *v17 = RotateLeft8(*v17, v20);
                        v23 = -1412068589;
                        v2 = 0x9CB81477B161416i64;
                        break;
                    }
                }
                else
                {
                    switch (v22)
                    {
                    case 0xE301330u:
                        v18 += 0x5101CCBD3B72D72i64;
                        v19 = v18;
                        v23 = 719613277;
                        break;
                    case 0x2DDF645u:
                        v19 = 0xE2D960D3B6181F99ui64 * ((v19 >> 27) ^ v19);
                        v23 = 253893427;
                        break;
                    case 0x66A592Fu:
                        v19 = 0x9E3779B97F4A7C15ui64 * ((v19 >> 27) ^ v19);
                        v23 = -2017019049;
                        break;
                    case 0x6A977C3u:
                        --v17;
                        v23 = -87950823;
                        break;
                    case 0x828BD12u:
                        v20 = v19 ^ (v19 >> 31);
                        v23 = 877209012;
                        break;
                    case 0x9C7D17Du:
                        v19 = 0xF4B4E98F3FD11C1Fui64 * ((v19 >> 30) ^ v19);
                        v23 = -411148070;
                        break;
                    }
                }
                v22 ^= v23;
            } while (v22 != -2023271320);
        } while (*v17);
        goto LABEL_1189;
    case 0x8E:
        *v1 = -97;
        v24 = v1 + 1;
        v25 = 0i64;
        v26 = 0;
        v27 = 752357553;
        v2 = 4248434306i64;
        do
        {
            if (v27 > 0x6474FD67)
            {
                switch (v27)
                {
                case 0x847DCC71:
                    v25 = 0x760726386EAC733Ai64;
                    v2 = 1007971419i64;
                    break;
                case 0xAF49FACD:
                    v25 = 0x709F09E352C0BA1Fi64;
                    v2 = 4292097312i64;
                    break;
                case 0xEC91E74A:
                    v25 = 0x9A5FA9FAB07A5BACui64;
                    v2 = 181068419i64;
                    break;
                case 0xF72437FC:
                    v25 = 0x81BA2ACAD28B0DCAui64;
                    v2 = 179959896i64;
                    break;
                }
            }
            else
            {
                switch (v27)
                {
                case 0x6474FD67u:
                    v25 = 0x315B115C6745D51Fi64;
                    v2 = 2595865i64;
                    break;
                case 0xF30193Fu:
                    v25 = 0x82F83F6111C0F2B3ui64;
                    v2 = 3394433575i64;
                    break;
                case 0x1E1EDF11u:
                    v25 = 0x4A132ACECF5BA632i64;
                    v2 = 2211200332i64;
                    break;
                case 0x23CDF3BCu:
                    v25 = 0xD0F0F6A2FA2F7AEDui64;
                    v2 = 2949130992i64;
                    break;
                case 0x2CD810B1u:
                    v25 = 0xB4A57B0F6DEDABEDui64;
                    v2 = 2789338308i64;
                    break;
                case 0x5E239035u:
                    v25 = 0xDFB411E01B814EB0ui64;
                    v2 = 16472135i64;
                    break;
                }
            }
            v27 ^= v2;
        } while (v27 != -1969619851);
        v28 = 0i64;
        v1 = str;
        if (!*v24)
            goto LABEL_1190;
        v2 = 0x4381848D5AF2933i64;
        do
        {
            v29 = 118923631;
            v30 = 1708732887;
            do
            {
                if (v29 > 0x4D5DDD89)
                {
                    if (v29 > 0xB8A015AD)
                    {
                        if (v29 > 0xEE286F1A)
                        {
                            if (v29 == -237223806)
                            {
                                v28 = 0xD4119085E7221B28ui64 * ((v28 >> 27) ^ v28);
                                v30 = 195913167;
                            }
                            else if (v29 == -198369830)
                            {
                                v26 = v28 ^ (v28 >> 31);
                                v30 = -1183793069;
                            }
                        }
                        else
                        {
                            switch (v29)
                            {
                            case 0xEE286F1A:
                                *v24 = RotateLeft8(*v24, v26);
                                v30 = -1921559572;
                                v2 = 0x4381848D5AF2933i64;
                                break;
                            case 0xB8EA78EE:
                                v25 += 0x4381848D5AF2933i64;
                                v28 = v25;
                                v30 = -822730105;
                                break;
                            case 0xBE6063DA:
                                v28 = 0x94D049BB133111EBui64 * ((v28 >> 30) ^ v28);
                                v30 = -2117176079;
                                break;
                            case 0xD78D7CA6:
                                v26 = (v28 ^ (v28 >> 28)) & 0x3F;
                                v30 = 167133850;
                                break;
                            }
                        }
                    }
                    else if (v29 == -1197468243)
                    {
                        v26 = (v28 ^ (v28 >> 31)) & 0x30;
                        v30 = 229880412;
                    }
                    else if (v29 > 0x8BE0283B)
                    {
                        if (v29 == -1414703491)
                        {
                            v28 = 0x9E3779B97F4A7C15ui64 * ((v28 >> 27) ^ v28);
                            v30 = 241351381;
                        }
                        else if (v29 == -1271946727)
                        {
                            v25 -= 0x40A7B892E31B1A47i64;
                            v28 = v25;
                            v30 = -828233518;
                        }
                    }
                    else
                    {
                        switch (v29)
                        {
                        case 0x8BE0283B:
                            v28 = 0x94D049BB133111EBui64 * ((v28 >> 27) ^ v28);
                            v30 = 2144155105;
                            break;
                        case 0x4ED07CFEu:
                            ++v24;
                            v30 = -1264119106;
                            break;
                        case 0x827DFF5C:
                            v25 += 0x225643FFE955806Bi64;
                            v28 = v25;
                            v30 = 1379175647;
                            break;
                        case 0x8762622A:
                            --v24;
                            v30 = 1827782744;
                            break;
                        }
                    }
                }
                else if (v29 == 1297997193)
                {
                    *v24 = RotateRight8(*v24, v26);
                    v30 = 59613559;
                    v2 = 0x4381848D5AF2933i64;
                }
                else if (v29 > 0x15BA90D4)
                {
                    if (v29 == 860605378)
                    {
                        v26 = v28 ^ (v28 >> 31);
                        v30 = 180933661;
                    }
                    else if (v29 == 1165943240)
                    {
                        v28 = 0x8938D993B83B1CFDui64 * ((v28 >> 49) ^ v28);
                        v30 = 202902915;
                    }
                }
                else
                {
                    switch (v29)
                    {
                    case 0x15BA90D4u:
                        v28 = 0x824BEC454199E73Cui64 * ((v28 >> 13) ^ v28);
                        v30 = -822495423;
                        break;
                    case 0x6558C94u:
                        v28 = 0x767D31CB3E4DCE3i64 * ((v28 >> 30) ^ v28);
                        v30 = -1217044601;
                        break;
                    case 0x716A16Fu:
                        v25 -= 0x61C8864680B583EBi64;
                        v28 = v25;
                        v30 = 6018995;
                        break;
                    case 0x74D76DCu:
                        v28 = 0xBF58476D1CE4E5B9ui64 * ((v28 >> 30) ^ v28);
                        v30 = -1934795033;
                        break;
                    }
                }
                v29 ^= v30;
            } while (v29 != -92836288);
        } while (*v24);
        goto LABEL_1189;
    case 0x91:
        goto LABEL_1116;
    case 0x94:
        *v1 = -92;
        v31 = v1 + 1;
        v32 = 0i64;
        v33 = 0;
        v34 = -496002011;
        v2 = 2014592924i64;
        do
        {
            if (v34 > 0x83D26B39)
            {
                switch (v34)
                {
                case 0xE02113BF:
                    v32 = 0xE365B4D910E3FA2Cui64;
                    v2 = 2314527010i64;
                    break;
                case 0xE26F9C25:
                    v32 = 0xF35E494A4A6EB8D5ui64;
                    v2 = 148395184i64;
                    break;
                case 0xE94ED551:
                    v32 = 0xD508298764C425AFui64;
                    v2 = 1534879181i64;
                    break;
                case 0xF0166B2F:
                    v32 = 0x43CF54F0AE0FEBEAi64;
                    v2 = 2687381186i64;
                    break;
                }
            }
            else
            {
                switch (v34)
                {
                case 0x83D26B39:
                    v32 = 0x716F2D628649C85Ai64;
                    v2 = 1071951775i64;
                    break;
                case 0x395CC8Eu:
                    v32 = 0x99FAC39E16B72A07ui64;
                    v2 = 662802879i64;
                    break;
                case 0x8DE3352u:
                    v32 = 0xBA2D6F1D74D8AA4Cui64;
                    v2 = 2803203875i64;
                    break;
                case 0xBB4AE58u:
                    v32 = 0xD4F523C7DD9A9ECAui64;
                    v2 = 2717284413i64;
                    break;
                case 0xF534883u:
                    v32 = 0xD2ECB939268F64B3ui64;
                    v2 = 1235452271i64;
                    break;
                case 0x3CE8ACCCu:
                    v32 = 0xDF1A164F706C5690ui64;
                    v2 = 2509345372i64;
                    break;
                }
            }
            v34 ^= v2;
        } while (v34 != -357054315);
        v35 = 0i64;
        v1 = str;
        if (!*v31)
            goto LABEL_1190;
        v2 = 0x3F65D85C935DD998i64;
        while (1)
        {
            v36 = 1508192851;
            v37 = 1832427028;
            do
            {
                if (v36 > 0x6E924AA6)
                {
                    if (v36 > 0xD041D7A6)
                    {
                        switch (v36)
                        {
                        case 0xD25C7B46:
                            v37 = -1536323606;
                        LABEL_363:
                            v33 = v35 ^ (v35 >> 31);
                            break;
                        case 0xD68FF04F:
                            v35 = 0xBF9E9F60FC8E2Di64 * ((v35 >> 27) ^ v35);
                            v37 = 462275722;
                            break;
                        case 0xF35AFEE9:
                            v32 -= 0x40A7B892E31B1A47i64;
                            v35 = v32;
                            v37 = 766172115;
                            break;
                        case 0xF75B7A8E:
                            v32 += 0x3F65D85C935DD998i64;
                            v35 = v32;
                            v37 = -1050743188;
                            break;
                        case 0xFF6AD3F0:
                            v35 = 0xE2DABC35C72BF9F1ui64 * ((v35 >> 2) ^ v35);
                            v37 = 1623544943;
                            break;
                        }
                    }
                    else
                    {
                        switch (v36)
                        {
                        case 0xD041D7A6:
                            v35 = 0x94D049BB133111EBui64 * ((v35 >> 27) ^ v35);
                            v37 = 35499232;
                            break;
                        case 0x7631F4ACu:
                            *v31 = RotateRight8(*v31, v33);
                            v37 = -401481427;
                            v2 = 0x3F65D85C935DD998i64;
                            break;
                        case 0x9E201581:
                            ++v31;
                            v37 = 1739023773;
                            break;
                        case 0x9EBF6ACC:
                            *v31 = RotateLeft8(*v31, v33);
                            v37 = 259406745;
                            v2 = 0x3F65D85C935DD998i64;
                            break;
                        case 0xA17ECA75:
                            v33 = (v35 ^ (v35 >> 31)) & 0x22;
                            v37 = 1998346414;
                            break;
                        case 0xC4DF8C13:
                            v35 = 0xD99B18ABFE5A6F96ui64 * ((v35 >> 18) ^ v35);
                            v37 = 1314602793;
                            break;
                        }
                    }
                }
                else
                {
                    if (v36 == 1855081126)
                    {
                        --v31;
                        v37 = -1089420212;
                        goto LABEL_364;
                    }
                    if (v36 > 0x166E7585)
                    {
                        if (v36 == 1370367667)
                        {
                            v35 = 0x9E3779B97F4A7C15ui64 * ((v35 >> 27) ^ v35);
                            v37 = -1120305509;
                            goto LABEL_364;
                        }
                        if (v36 == 1508192851)
                        {
                            v32 -= 0x61C8864680B583EBi64;
                            v35 = v32;
                            v37 = 1334527958;
                            goto LABEL_364;
                        }
                        if (v36 != 1730626792)
                            goto LABEL_364;
                        v37 = 188380219;
                        goto LABEL_363;
                    }
                    switch (v36)
                    {
                    case 0x166E7585u:
                        v35 = 0xBF58476D1CE4E5B9ui64 * ((v35 >> 30) ^ v35);
                        v37 = -969956829;
                        break;
                    case 0xB51B62u:
                        v32 -= 0x13836514E05BBC32i64;
                        v35 = v32;
                        v37 = 95115877;
                        break;
                    case 0x4F9CC47u:
                        v35 = 0x686A7F43B5EFEFi64 * ((v35 >> 30) ^ v35);
                        v37 = -2078050272;
                        break;
                    case 0xD642276u:
                        v33 = (v35 ^ (v35 >> 21)) & 0x24;
                        v37 = 371884581;
                        break;
                    case 0xF0E926Bu:
                        v35 = 0x94D049BB133111EBui64 * ((v35 >> 30) ^ v35);
                        v37 = -1255871586;
                        break;
                    }
                }
            LABEL_364:
                v36 ^= v37;
            } while (v36 != -108564452);
            if (!*v31)
                goto LABEL_1189;
        }
    case 0x97:
        *v1 = 0x80;
        v38 = v1 + 1;
        v39 = 0i64;
        v40 = 0;
        v41 = 1833549073;
        v2 = 3715072166i64;
        do
        {
            if (v41 > 0x832A80F3)
            {
                switch (v41)
                {
                case 0x973FA955:
                    v39 = 0x44F338054CB28178i64;
                    v2 = 4220637891i64;
                    break;
                case 0xB12E5D22:
                    v39 = 0x3DF68D39571355Di64;
                    v2 = 1232965720i64;
                    break;
                case 0xC1462046:
                    v39 = 0x9CD043553F5426D7ui64;
                    v2 = 2721706899i64;
                    break;
                case 0xC312EDA2:
                    v39 = 0xF042E5DDFFC171A0ui64;
                    v2 = 449077881i64;
                    break;
                }
            }
            else
            {
                switch (v41)
                {
                case 0x832A80F3:
                    v39 = 0x5A39A4C2A2D329C8i64;
                    v2 = 1669092925i64;
                    break;
                case 0xF0AA6A0u:
                    v39 = 0xCACE004E9C76C76i64;
                    v2 = 3750273618i64;
                    break;
                case 0x1D446A7Fu:
                    v39 = 0xCC219E35C058D7B4ui64;
                    v2 = 3965298094i64;
                    break;
                case 0x3A474C41u:
                    v39 = 0xCFAA312091992A35ui64;
                    v2 = 1423621845i64;
                    break;
                case 0x63EA5F4Du:
                    v39 = 0xCF4E073FE8CB3463ui64;
                    v2 = 3850530999i64;
                    break;
                case 0x6D49BD11u:
                    v39 = 0x7C097652E5B709DEi64;
                    v2 = 2059944115i64;
                    break;
                }
            }
            v41 ^= v2;
        } while (v41 != 394395042);
        v42 = 0i64;
        v1 = str;
        if (!*v38)
            goto LABEL_1190;
        v2 = 0x689E82D9D2C6275i64;
        while (1)
        {
            v43 = 39801023;
            v44 = 1425913246;
            do
            {
                if (v43 > 0x5AB9B9A8)
                {
                    if (v43 > 0x81968229)
                    {
                        switch (v43)
                        {
                        case 0xA71B55CF:
                            *v38 = RotateLeft8(*v38, v40);
                            v44 = -1572000749;
                            goto LABEL_436;
                        case 0xABACE3A7:
                            v40 = (v42 ^ (v42 >> 28)) & 0x37;
                            v44 = 1198469011;
                            break;
                        case 0xF4A81ECA:
                            *v38 = RotateRight8(*v38, v40);
                            v44 = -1013896524;
                        LABEL_436:
                            v2 = 0x689E82D9D2C6275i64;
                            break;
                        case 0xF513862E:
                            v40 = (v42 ^ (v42 >> 31)) & 0x16;
                            v44 = 207253586;
                            break;
                        }
                    }
                    else
                    {
                        switch (v43)
                        {
                        case 0x81968229:
                            v42 = 0xFB62C2D72D1C3AD6ui64 * ((v42 >> 27) ^ v42);
                            v44 = 1782863337;
                            break;
                        case 0x5D4EA4A4u:
                            v40 = v42 ^ (v42 >> 31);
                            v44 = -1444496786;
                            break;
                        case 0x71EB2D1Bu:
                            v39 -= 0x40A7B892E31B1A47i64;
                            v42 = v39;
                            v44 = -124220541;
                            break;
                        case 0x72E6599Du:
                            v42 = 0x94D049BB133111EBui64 * ((v42 >> 27) ^ v42);
                            v44 = 799604025;
                            break;
                        case 0x74FBFF11u:
                            v42 = 0xBF58476D1CE4E5B9ui64 * ((v42 >> 30) ^ v42);
                            v44 = 102606476;
                            break;
                        }
                    }
                }
                else if (v43 == 1522121128)
                {
                    v42 = 0x94D049BB133111EBui64 * ((v42 >> 30) ^ v42);
                    v44 = -696076716;
                }
                else if (v43 > 0x17CAC865)
                {
                    switch (v43)
                    {
                    case 0x1B71F6B7u:
                        v42 = 0x97CDA3BCB7C8E32i64 * ((v42 >> 34) ^ v42);
                        v44 = 2118567422;
                        break;
                    case 0x2BAAA482u:
                        v39 -= 0x689E82D9D2C6275i64;
                        v42 = v39;
                        v44 = 458059798;
                        break;
                    case 0x3739347Eu:
                        ++v38;
                        v44 = 1885386075;
                        break;
                    case 0x4D582D19u:
                        v42 = 0x9E3779B97F4A7C15ui64 * ((v42 >> 27) ^ v42);
                        v44 = -856798833;
                        break;
                    }
                }
                else
                {
                    switch (v43)
                    {
                    case 0x17CAC865u:
                        --v38;
                        v44 = -119342831;
                        break;
                    case 0x25F50BFu:
                        v39 -= 0x61C8864680B583EBi64;
                        v42 = v39;
                        v44 = 1990504366;
                        break;
                    case 0x9CC23B8u:
                        v42 = 0x7CA60D3CA2E7F81i64 * ((v42 >> 23) ^ v42);
                        v44 = 617282669;
                        break;
                    case 0xB495334u:
                        v39 -= 0x73E94B387995BDA4i64;
                        v42 = v39;
                        v44 = 691174748;
                        break;
                    case 0xCA17A83u:
                        v42 = 0x1655D3395A8027DBi64 * ((v42 >> 30) ^ v42);
                        v44 = 258419079;
                        break;
                    case 0xDE123BBu:
                        v40 = v42 ^ (v42 >> 31);
                        v44 = -294673771;
                        break;
                    }
                }
                v43 ^= v44;
            } while (v43 != 1197048101);
            if (!*v38)
                goto LABEL_1189;
        }
    case 0x9B:
        *v1 = -123;
        v45 = v1 + 1;
        v46 = 0i64;
        v47 = 0i64;
        v48 = 0;
        v49 = 256482108;
        v2 = 2132630961i64;
        do
        {
            if (v49 > 0x5F766D1E)
            {
                switch (v49)
                {
                case 0x60A5CEA9u:
                    v46 = 0x4290209F43CD2D41i64;
                    v2 = 1517341095i64;
                    break;
                case 0x6BB62388u:
                    v46 = 0x1835FE2E6951F342i64;
                    v2 = 1531960432i64;
                    break;
                case 0xAA543C38:
                    v46 = 0x1A3C8121C3941E19i64;
                    v2 = 1078164969i64;
                    break;
                case 0xD8AE5329:
                    v46 = 0xE53F8133001D2C74ui64;
                    v2 = 2845798134i64;
                    break;
                }
            }
            else
            {
                switch (v49)
                {
                case 0x5F766D1Eu:
                    v46 = 0x122CC42501318E87i64;
                    v2 = 1323474149i64;
                    break;
                case 0xB543633u:
                    v46 = 0x4501097B39D8EB02i64;
                    v2 = 2868078720i64;
                    break;
                case 0xC8FCCEDu:
                    v46 = 0x21870BCB2BFF4C2Di64;
                    v2 = 1876815398i64;
                    break;
                case 0xF499B3Cu:
                    v46 = 0xAE5DE055D528FFEDui64;
                    v2 = 1612492258i64;
                    break;
                case 0x303449B0u:
                    v46 = 0x4E529CA1FCBBA1D3i64;
                    v2 = 2308470144i64;
                    break;
                case 0x5EF7CF34u:
                    v46 = 0x42B6DF11385C2317i64;
                    v2 = 3871283628i64;
                    break;
                }
            }
            v49 ^= v2;
        } while (v49 != 1867855582);
        v1 = str;
        if (!*v45)
            goto LABEL_1190;
        v2 = 0x7C7862ACBFDE728Ci64;
        while (1)
        {
            v50 = 266409416;
            v51 = 1320456643;
            do
            {
                if (v50 > 0x65E3E622)
                {
                    if (v50 > 0xC311097F)
                    {
                        switch (v50)
                        {
                        case 0xCF2D55FF:
                            v51 = 1860036729;
                            goto LABEL_509;
                        case 0xE4D1F711:
                            v47 = 0x94D049BB133111EBui64 * ((v47 >> 30) ^ v47);
                            v51 = 222786586;
                            break;
                        case 0xF0F16243:
                            v47 = 0x659A8989A47C7C65i64 * ((v47 >> 33) ^ v47);
                            v51 = 254851042;
                            break;
                        case 0xFF679FA5:
                            v48 = (v47 ^ (v47 >> 31)) & 0xB;
                            v51 = -1979991742;
                            break;
                        }
                    }
                    else
                    {
                        switch (v50)
                        {
                        case 0xC311097F:
                            v47 = 0xBF58476D1CE4E5B9ui64 * ((v47 >> 30) ^ v47);
                            v51 = -191883346;
                            break;
                        case 0x7EF19A8Eu:
                            v47 = 0x9E3779B97F4A7C15ui64 * ((v47 >> 27) ^ v47);
                            v51 = -2034871172;
                            break;
                        case 0x9E3E7869:
                            v47 = 0xDB10CBED51492E3i64 * ((v47 >> 30) ^ v47);
                            v51 = 431658695;
                            break;
                        case 0xAB3FE554:
                            *v45 = RotateLeft8(*v45, v48);
                            v51 = 1520942164;
                            v2 = 0x7C7862ACBFDE728Ci64;
                            break;
                        case 0xB7C72DD4:
                            v47 = 0xBAFA66774EB1583i64 * ((v47 >> 52) ^ v47);
                            v51 = -1286525607;
                            break;
                        case 0xC00A2748:
                            ++v45;
                            v51 = -17282204;
                            break;
                        }
                    }
                }
                else
                {
                    if (v50 == 1709434402)
                    {
                        *v45 = RotateRight8(*v45, v48);
                        v51 = -1511407254;
                        v2 = 0x7C7862ACBFDE728Ci64;
                        goto LABEL_510;
                    }
                    if (v50 > 0x1A6841A0)
                    {
                        switch (v50)
                        {
                        case 0x1F152264u:
                            v51 = 2062992454;
                        LABEL_509:
                            v48 = v47 ^ (v47 >> 31);
                            break;
                        case 0x2BFD9B49u:
                            v46 -= 0x7C7862ACBFDE728Ci64;
                            v47 = v46;
                            v51 = -92612848;
                            break;
                        case 0x337B53AEu:
                            --v45;
                            v51 = -1350069084;
                            break;
                        case 0x37811ED1u:
                            v47 = 0x94D049BB133111EBui64 * ((v47 >> 27) ^ v47);
                            v51 = 680803509;
                            break;
                        }
                    }
                    else
                    {
                        switch (v50)
                        {
                        case 0x1A6841A0u:
                            v48 = (v47 ^ (v47 >> 21)) & 0x25;
                            v51 = -1448024722;
                            break;
                        case 0xBB9F93u:
                            v47 = 0x27CA2C11DFC142E5i64 * ((v47 >> 27) ^ v47);
                            v51 = -1247910186;
                            break;
                        case 0xE94E16u:
                            v46 += 0xDCAFBBF59557281i64;
                            v47 = v46;
                            v51 = 4466845;
                            break;
                        case 0xFE115C8u:
                            v46 -= 0x61C8864680B583EBi64;
                            v47 = v46;
                            v51 = -856679241;
                            break;
                        case 0x16D9C770u:
                            v46 -= 0x40A7B892E31B1A47i64;
                            v47 = v46;
                            v51 = -1495896901;
                            break;
                        }
                    }
                }
            LABEL_510:
                v50 ^= v51;
            } while (v50 != 1056074796);
            if (!*v45)
                goto LABEL_1189;
        }
    case 0xA3:
        *v1 = -116;
        v52 = v1 + 1;
        v53 = 0i64;
        v54 = 0i64;
        v55 = 0;
        v56 = 40785073;
        v2 = 1088198438i64;
        do
        {
            if (v56 > 0x83A8D1CD)
            {
                switch (v56)
                {
                case 0xA22DBE56:
                    v53 = 0x4E3587932FFDBB7Di64;
                    v2 = 594730866i64;
                    break;
                case 0xAFA2A510:
                    v53 = 0xD81B1B459697622Fui64;
                    v2 = 801286164i64;
                    break;
                case 0xC441C88D:
                    v53 = 0xB29FE285F1D63A41ui64;
                    v2 = 232273519i64;
                    break;
                case 0xCC9EFAAD:
                    v53 = 0xB61AFDA29656792Aui64;
                    v2 = 197611758i64;
                    break;
                }
            }
            else
            {
                switch (v56)
                {
                case 0x83A8D1CD:
                    v53 = 0xB4BD70EB93AADC27ui64;
                    v2 = 164659801i64;
                    break;
                case 0x26E54B1u:
                    v53 = 0xF00C425F77C65810ui64;
                    v2 = 154403858i64;
                    break;
                case 0xC0B2BE2u:
                    v53 = 0x87DEC7E5DEE86601ui64;
                    v2 = 1271760218i64;
                    break;
                case 0xD05AFE7u:
                    v53 = 0xD730B6CB64852844ui64;
                    v2 = 3358351892i64;
                    break;
                case 0x242A42E0u:
                    v53 = 0xBC9E38D9A5A4F323ui64;
                    v2 = 1156085715i64;
                    break;
                case 0x4D8F5E74u:
                    v53 = 0xB138E04976140A2Aui64;
                    v2 = 825497367i64;
                    break;
                }
            }
            v56 ^= v2;
        } while (v56 != 190468259);
        v1 = str;
        if (!*v52)
            goto LABEL_1190;
        v2 = 0x3AC5C4F1F90B35i64;
        do
        {
            v57 = 494124569;
            v58 = -785491034;
            do
            {
                if (v57 > 0x58467BCE)
                {
                    if (v57 > 0xAA71A623)
                    {
                        switch (v57)
                        {
                        case 0xBCA74C96:
                            --v52;
                            v58 = 10745627;
                            break;
                        case 0xCD2E5AFF:
                            v54 = 0xBF58476D1CE4E5B9ui64 * ((v54 >> 30) ^ v54);
                            v58 = 1274105912;
                            break;
                        case 0xF824A32C:
                            v55 = v54 ^ (v54 >> 31);
                            v58 = -109185803;
                            break;
                        case 0xFD62E1FE:
                            v54 = 0x9E3779B97F4A7C15ui64 * ((v54 >> 27) ^ v54);
                            v58 = 159775648;
                            break;
                        }
                    }
                    else
                    {
                        switch (v57)
                        {
                        case 0xAA71A623:
                            v54 = 0x5B73AFF6DD141896i64 * ((v54 >> 27) ^ v54);
                            v58 = 236819108;
                            break;
                        case 0x7BDD43BDu:
                            v54 = 0x94D049BB133111EBui64 * ((v54 >> 30) ^ v54);
                            v58 = 952037820;
                            break;
                        case 0x86DF0AC7:
                            v54 = 0x94D049BB133111EBui64 * ((v54 >> 27) ^ v54);
                            v58 = -1569118992;
                            break;
                        case 0x9A6E106A:
                            v54 = 0x78C1CE90F335CBCBi64 * ((v54 >> 30) ^ v54);
                            v58 = -2099860114;
                            break;
                        case 0xA827BF46:
                            v53 += 0x3AC5C4F1F90B35i64;
                            v54 = v53;
                            v58 = -1872019479;
                            break;
                        }
                    }
                }
                else if (v57 == 1481014222)
                {
                    v54 = 0xD8F0BC4B95F6FA94ui64 * ((v54 >> 53) ^ v54);
                    v58 = 442892225;
                }
                else if (v57 > 0x1D8BD55D)
                {
                    switch (v57)
                    {
                    case 0x227CC749u:
                        ++v52;
                        v58 = -1315018185;
                        break;
                    case 0x23CAF065u:
                        *v52 = RotateRight8(*v52, v55);
                        v58 = 28718892;
                        v2 = 0x3AC5C4F1F90B35i64;
                        break;
                    case 0x24A62E37u:
                        v55 = v54 ^ (v54 >> 31);
                        v58 = 124575314;
                        break;
                    case 0x427FEC33u:
                        *v52 = RotateLeft8(*v52, v55);
                        v58 = 183684957;
                        v2 = 0x3AC5C4F1F90B35i64;
                        break;
                    }
                }
                else
                {
                    switch (v57)
                    {
                    case 0x1D8BD55Du:
                        v55 = (v54 ^ (v54 >> 18)) & 2;
                        v58 = -1622738146;
                        break;
                    case 0x147BB91u:
                        v53 += 0x27DD95929DC61B2i64;
                        v54 = v53;
                        v58 = 201723671;
                        break;
                    case 0x15A6F76u:
                        v55 = (v54 ^ (v54 >> 31)) & 0x2D;
                        v58 = 87721464;
                        break;
                    case 0x16E1219u:
                        v53 -= 0x40A7B892E31B1A47i64;
                        v54 = v53;
                        v58 = -8408351;
                        break;
                    case 0x6113115u:
                        v54 = 0x41E05A1F82CFDE57i64 * ((v54 >> 29) ^ v54);
                        v58 = 78000711;
                        break;
                    case 0x1D73BE19u:
                        v53 -= 0x61C8864680B583EBi64;
                        v54 = v53;
                        v58 = -799152922;
                        break;
                    }
                }
                v57 ^= v58;
            } while (v57 != -1813860994);
        } while (*v52);
        goto LABEL_1189;
    case 0xA5:
        *v1 = -111;
        v2 = 0i64;
        if ((unsigned __int8)v1[1] - 1 <= 0)
            goto LABEL_1116;
        do
        {
            v59 = v1[(int)v2 + 2];
            if (v59 != -91 - (byte)v2)
                v1[(int)v2 + 2] = v59 ^ (-91 - v2);
            v2 = (unsigned int)(v2 + 1);
        } while ((int)v2 < (unsigned __int8)v1[1] - 1);
        v60 = 2i64;
        goto LABEL_1191;
    case 0xA8:
        *v1 = -97;
        v61 = v1 + 1;
        v62 = 0i64;
        v63 = 0;
        v64 = -1948946138;
        v2 = 2367463449i64;
        do
        {
            if (v64 > 0x5B27F5AE)
            {
                switch (v64)
                {
                case 0x85F66098:
                    v62 = 0xC7B3F8CBD5AF0EAAui64;
                    v2 = 1423250238i64;
                    break;
                case 0x8BD57126:
                    v62 = 0x403E3CB3345EE751i64;
                    v2 = 1411039943i64;
                    break;
                case 0x8E6FF139:
                    v62 = 0xF19F420093ADA3EDui64;
                    v2 = 1140006587i64;
                    break;
                case 0xCE6B32C2:
                    v62 = 0x98C55FD954F2CCEEui64;
                    v2 = 1537525171i64;
                    break;
                }
            }
            else
            {
                switch (v64)
                {
                case 0x5B27F5AEu:
                    v62 = 0x1A980CE2B089ECEFi64;
                    v2 = 2403540760i64;
                    break;
                case 0x4F9AFC7u:
                    v62 = 0xB414B0F65FF3753i64;
                    v2 = 2842602775i64;
                    break;
                case 0x7932F61u:
                    v62 = 0x3A44F7A32986D7F6i64;
                    v2 = 2671720486i64;
                    break;
                case 0x17687634u:
                    v62 = 0xF3CA0CA49C8064E7ui64;
                    v2 = 127378576i64;
                    break;
                case 0x4C8AC16Bu:
                    v62 = 0x560DE6D2E979A073i64;
                    v2 = 183223249i64;
                    break;
                case 0x5A665C35u:
                    v62 = 0xD5E4040E5722B49Bui64;
                    v2 = 2890419319i64;
                    break;
                }
            }
            v64 ^= v2;
        } while (v64 != -540036127);
        v65 = 0i64;
        v1 = str;
        if (!*v61)
            goto LABEL_1190;
        v2 = 0x6614AE11995502i64;
        while (1)
        {
            v66 = -527083371;
            v67 = 230941981;
            do
            {
                if (v66 > 0x56DB7928)
                {
                    if (v66 > 0xE15F9E72)
                    {
                        switch (v66)
                        {
                        case 0xE59C3F2C:
                            v68 = (v65 >> 30) ^ v65;
                            v67 = 1807951407;
                            goto LABEL_660;
                        case 0xE837185B:
                            v65 = 0xC67F4F6624F97FC0ui64 * ((v65 >> 27) ^ v65);
                            v67 = 630504869;
                            break;
                        case 0xF2F58E12:
                            v62 += 0x6614AE11995502i64;
                            v65 = v62;
                            v67 = 1038768960;
                            break;
                        case 0xFC52D9DA:
                            v65 = 0x7AAF55EDC425770i64 * ((v65 >> 15) ^ v65);
                            v67 = 127602583;
                            break;
                        case 0xFE7BF1F7:
                            v63 = v65 ^ (v65 >> 31);
                            v67 = -977938203;
                            break;
                        }
                    }
                    else
                    {
                        if (v66 == -513827214)
                        {
                            v65 = 0xBF58476D1CE4E5B9ui64 * ((v65 >> 30) ^ v65);
                            v67 = 751246452;
                            goto LABEL_661;
                        }
                        if (v66 > 0xCD988206)
                        {
                            if (v66 == -669763075)
                            {
                                v65 = 0x9E3779B97F4A7C15ui64 * ((v65 >> 27) ^ v65);
                                v67 = -1519619693;
                            }
                            else if (v66 == -527083371)
                            {
                                v62 -= 0x61C8864680B583EBi64;
                                v65 = v62;
                                v67 = 30066407;
                            }
                        }
                        else
                        {
                            switch (v66)
                            {
                            case 0xCD988206:
                                v68 = (v65 >> 27) ^ v65;
                                v67 = 870544369;
                            LABEL_660:
                                v65 = 0x94D049BB133111EBui64 * v68;
                                break;
                            case 0x7BDF4FDFu:
                                v62 -= 0x40A7B892E31B1A47i64;
                                v65 = v62;
                                v67 = 945142560;
                                break;
                            case 0x94C714F4:
                                v63 = (v65 ^ (v65 >> 31)) & 0x28;
                                v67 = 191045082;
                                break;
                            case 0xC910AE5F:
                                *v61 = RotateLeft8(*v61, v63);
                                v67 = 1652943542;
                                v2 = 0x6614AE11995502i64;
                                break;
                            }
                        }
                    }
                }
                else if (v66 == 1457223976)
                {
                    ++v61;
                    v67 = 412958586;
                }
                else if (v66 > 0x28648899)
                {
                    switch (v66)
                    {
                    case 0x3BCE2912u:
                        *v61 = RotateRight8(*v61, v63);
                        v67 = 1830113338;
                        v2 = 0x6614AE11995502i64;
                        break;
                    case 0x44786938u:
                        v63 = v65 ^ (v65 >> 31);
                        v67 = 1457154249;
                        break;
                    case 0x4D851DA4u:
                        v65 = 0x8161E7F730A7F3A6ui64 * ((v65 >> 52) ^ v65);
                        v67 = 32673623;
                        break;
                    }
                }
                else
                {
                    switch (v66)
                    {
                    case 0x28648899u:
                        v65 = 0xC4706BBDCB391BC2ui64 * ((v65 >> 30) ^ v65);
                        v67 = -1666881128;
                        break;
                    case 0x36091DAu:
                        --v61;
                        v67 = 1678908643;
                        break;
                    case 0x85BCA2Cu:
                        v62 -= 0x110F2E1A181BEABDi64;
                        v65 = v62;
                        v67 = -303371175;
                        break;
                    case 0x12149F54u:
                        v63 = (v65 ^ (v65 >> 38)) & 0x38;
                        v67 = -1995315339;
                        break;
                    }
                }
            LABEL_661:
                v66 ^= v67;
            } while (v66 != 1313228370);
            if (!*v61)
                goto LABEL_1189;
        }
    case 0xAD:
        *v1 = -92;
        v69 = v1 + 1;
        v70 = 0i64;
        v71 = 0;
        v72 = 590120983;
        v2 = 4236448200i64;
        do
        {
            if (v72 > 0xAF68A91B)
            {
                switch (v72)
                {
                case 0xBC1AD21D:
                    v70 = 0x1C9FDF5370E6673Di64;
                    v2 = 4059650618i64;
                    break;
                case 0xD84F832F:
                    v70 = 0x520D7A5D4D9DA38Bi64;
                    v2 = 3520014379i64;
                    break;
                case 0xDBAEC55D:
                    v70 = 0x12DF3371B01EA3A0i64;
                    v2 = 21501998i64;
                    break;
                case 0xFB32893A:
                    v70 = 0xBEDD006D5999A4BDui64;
                    v2 = 167686592i64;
                    break;
                }
            }
            else
            {
                switch (v72)
                {
                case 0xAF68A91B:
                    v70 = 0x164B2E5525DB057Fi64;
                    v2 = 228745979i64;
                    break;
                case 0x4DB3D80u:
                    v70 = 0x4979E0F121BE9B88i64;
                    v2 = 340251606i64;
                    break;
                case 0x232C8817u:
                    v70 = 0xF602E1E10F29C79i64;
                    v2 = 3155260433i64;
                    break;
                case 0x3D895AC0u:
                    v70 = 0x2AA70D08AB72485Fi64;
                    v2 = 2571665175i64;
                    break;
                case 0x6B3ED985u:
                    v70 = 0x27C0F08CE714A543i64;
                    v2 = 2015253791i64;
                    break;
                case 0x6FD42AE0u:
                    v70 = 0x50B0CB2A219BB911i64;
                    v2 = 954816681i64;
                    break;
                }
            }
            v72 ^= v2;
        } while (v72 != -1623327738);
        v73 = 0i64;
        v1 = str;
        if (!*v69)
            goto LABEL_1190;
        v2 = 0x6412416FB593CF90i64;
        while (1)
        {
            v74 = -1382582623;
            v75 = -578933597;
            do
            {
                if (v74 > 0x17CDF804)
                {
                    if (v74 > 0xA4B04784)
                    {
                        switch (v74)
                        {
                        case 0xAD9776A1:
                            v70 -= 0x61C8864680B583EBi64;
                            v75 = -1168470363;
                            goto LABEL_733;
                        case 0xAF9252E0:
                            v70 += 0x6412416FB593CF90i64;
                            v75 = 833332586;
                            goto LABEL_733;
                        case 0xCE3A192C:
                            *v69 = RotateRight8(*v69, v71);
                            v75 = 1451483698;
                            v2 = 0x6412416FB593CF90i64;
                            break;
                        case 0xD1ED7019:
                            v73 = 0x9E3779B97F4A7C15ui64 * ((v73 >> 27) ^ v73);
                            v75 = -1050752986;
                            break;
                        }
                    }
                    else
                    {
                        switch (v74)
                        {
                        case 0xA4B04784:
                            v71 = (v73 ^ (v73 >> 31)) & 0x15;
                            v75 = -518993615;
                            break;
                        case 0x68EA35DAu:
                            v70 += 0x14A886715CD13F71i64;
                            v75 = 23601646;
                            goto LABEL_733;
                        case 0x6C1BAE14u:
                            v73 = 0x5F4D88E4A18A591i64 * ((v73 >> 27) ^ v73);
                            v75 = 120319058;
                            break;
                        case 0x82DE9640:
                            v73 = 0x94D049BB133111EBui64 * ((v73 >> 30) ^ v73);
                            v75 = -10794963;
                            break;
                        case 0x98B9FB1E:
                            ++v69;
                            v75 = 1900260219;
                            break;
                        }
                    }
                }
                else
                {
                    if (v74 == 399374340)
                    {
                        v73 = 0xBF58476D1CE4E5B9ui64 * ((v73 >> 30) ^ v73);
                        v75 = 95749233;
                        goto LABEL_734;
                    }
                    if (v74 > 0xD83E2C3)
                    {
                        switch (v74)
                        {
                        case 0xE872B06u:
                            v70 -= 0x40A7B892E31B1A47i64;
                            v75 = 1923148625;
                        LABEL_733:
                            v73 = v70;
                            break;
                        case 0x1278FC75u:
                            v73 = 0x94D049BB133111EBui64 * ((v73 >> 27) ^ v73);
                            v75 = 75474111;
                            break;
                        case 0x1392A0ECu:
                            v71 = v73 ^ (v73 >> 31);
                            v75 = -1115562778;
                            break;
                        case 0x160758CAu:
                            v71 = v73 ^ (v73 >> 31);
                            v75 = -667074074;
                            break;
                        }
                    }
                    else
                    {
                        switch (v74)
                        {
                        case 0xD83E2C3u:
                            v73 = 0x96EC2BEC23F49BB4ui64 * ((v73 >> 30) ^ v73);
                            v75 = 1172741088;
                            break;
                        case 0x3755ACu:
                            v73 = 0x1BE793B6EF50209Fi64 * ((v73 >> 20) ^ v73);
                            v75 = -520919001;
                            break;
                        case 0x4D2B645u:
                            *v69 = RotateLeft8(*v69, v71);
                            v75 = 58444364;
                            v2 = 0x6412416FB593CF90i64;
                            break;
                        case 0x839744Du:
                            v71 = (v73 ^ (v73 >> 21)) & 0x29;
                            v75 = -1255703561;
                            break;
                        case 0x86BF7CEu:
                            --v69;
                            v75 = 919843446;
                            break;
                        case 0x92DDAE7u:
                            v73 = 0x4A42985CDB82BCDi64 * ((v73 >> 53) ^ v73);
                            v75 = -1989311699;
                            break;
                        }
                    }
                }
            LABEL_734:
                v74 ^= v75;
            } while (v74 != -369471387);
            if (!*v69)
                goto LABEL_1189;
        }
    case 0xAF:
        *v1 = -111;
        v2 = 0i64;
        if ((unsigned __int8)v1[1] - 1 <= 0)
            goto LABEL_1116;
        do
        {
            v76 = v1[(int)v2 + 2];
            if (v76 != (byte)v2 - 81)
                v1[(int)v2 + 2] = v76 ^ (v2 - 81);
            v2 = (unsigned int)(v2 + 1);
        } while ((int)v2 < (unsigned __int8)v1[1] - 1);
        v60 = 2i64;
        goto LABEL_1191;
    case 0xB1:
        *v1 = 0x80;
        v77 = v1 + 1;
        v78 = 0i64;
        v79 = 0i64;
        v80 = 0;
        v81 = 1569604019;
        v2 = 2749655138i64;
        do
        {
            if (v81 > 0x868BA6BB)
            {
                switch (v81)
                {
                case 0x9B7F4B91:
                    v78 = 0xC9267F3C557FCD22ui64;
                    v2 = 4192580925i64;
                    break;
                case 0xC02A8FAF:
                    v78 = 0xCEB94DDBDB2E4FFBui64;
                    v2 = 3802798402i64;
                    break;
                case 0xEE88A676:
                    v78 = 0x75931C2ED851C69Bi64;
                    v2 = 3884327448i64;
                    break;
                case 0xF9CD5A14:
                    v78 = 0xC5B0ED64E823CB4Bui64;
                    v2 = 2031136148i64;
                    break;
                }
            }
            else
            {
                switch (v81)
                {
                case 0x868BA6BB:
                    v78 = 0xC4EAFDE7908E6320ui64;
                    v2 = 2907953303i64;
                    break;
                case 0x13FBC20u:
                    v78 = 0x5CFE54241DFF411Ci64;
                    v2 = 1187278358i64;
                    break;
                case 0x93F394Cu:
                    v78 = 0x3B5B5E4609979E61i64;
                    v2 = 3880937604i64;
                    break;
                case 0x1388BDACu:
                    v78 = 0x1FAB8D1379913B4i64;
                    v2 = 219432087i64;
                    break;
                case 0x318A2B44u:
                    v78 = 0xCD54B93C3BFC1111ui64;
                    v2 = 2813858897i64;
                    break;
                case 0x5D8E41B3u:
                    v78 = 0x6C6889ED7BB7BA73i64;
                    v2 = 3028336179i64;
                    break;
                }
            }
            v81 ^= v2;
        } while (v81 != -384893056);
        v1 = str;
        if (!*v77)
            goto LABEL_1190;
        v2 = 0x24B71D6314DC6B17i64;
        do
        {
            v82 = 1574360;
            v83 = 266897268;
            do
            {
                if (v82 > 0x5AC4AC18)
                {
                    if (v82 > 0xACF4F8D1)
                    {
                        switch (v82)
                        {
                        case 0xB5D05176:
                            ++v77;
                            v83 = 128950258;
                            break;
                        case 0xD0264B29:
                            v79 = 0xBF58476D1CE4E5B9ui64 * ((v79 >> 30) ^ v79);
                            v83 = 850296531;
                            break;
                        case 0xD612779D:
                            v80 = (v79 ^ (v79 >> 31)) & 0x3D;
                            v83 = -869550593;
                            break;
                        case 0xE28835FA:
                            v79 = 0x94D049BB133111EBui64 * ((v79 >> 27) ^ v79);
                            v83 = 1155580641;
                            break;
                        }
                    }
                    else
                    {
                        switch (v82)
                        {
                        case 0xACF4F8D1:
                            v79 = 0x94D049BB133111EBui64 * ((v79 >> 30) ^ v79);
                            v83 = 1600099422;
                            break;
                        case 0x5D8B1F33u:
                            *v77 = RotateRight8(*v77, v80);
                            v83 = -396669371;
                            v2 = 0x24B71D6314DC6B17i64;
                            break;
                        case 0x7E48A88Eu:
                            v79 = 0x2A46F2806BAF22CCi64 * ((v79 >> 27) ^ v79);
                            v83 = 849714337;
                            break;
                        case 0xA36112BB:
                            v79 = 0x9E3779B97F4A7C15ui64 * ((v79 >> 27) ^ v79);
                            v83 = 91590422;
                            break;
                        case 0xA668F71B:
                            v80 = v79 ^ (v79 >> 31);
                            v83 = -68949976;
                            break;
                        }
                    }
                }
                else if (v82 == 1522838552)
                {
                    v78 += 0x24B71D6314DC6B17i64;
                    v79 = v78;
                    v83 = 1585723890;
                }
                else if (v82 > 0x1CB4E969)
                {
                    switch (v82)
                    {
                    case 0x1F7734C0u:
                        v79 = 0x9D76DCD12E37899i64 * ((v79 >> 33) ^ v79);
                        v83 = 1665077706;
                        break;
                    case 0x40A3B047u:
                        --v77;
                        v83 = -860881253;
                        break;
                    case 0x5060392Fu:
                        v79 = 0x7683FF2EBB4EE51i64 * ((v79 >> 22) ^ v79);
                        v83 = 77191196;
                        break;
                    case 0x5AC49633u:
                        v79 = 0xB04F587B65FFF11i64 * ((v79 >> 30) ^ v79);
                        v83 = -1082231191;
                        break;
                    }
                }
                else
                {
                    switch (v82)
                    {
                    case 0x1CB4E969u:
                        v80 = (v79 ^ (v79 >> 55)) & 0x3C;
                        v83 = -1125415559;
                        break;
                    case 0x1805D8u:
                        v78 -= 0x61C8864680B583EBi64;
                        v79 = v78;
                        v83 = -801222927;
                        break;
                    case 0x327292u:
                        v78 += 0xC58871036E22DC8i64;
                        v79 = v78;
                        v83 = -1135564042;
                        break;
                    case 0x1048E22u:
                        *v77 = RotateLeft8(*v77, v80);
                        v83 = 429832131;
                        v2 = 0x24B71D6314DC6B17i64;
                        break;
                    case 0xCD5C99Du:
                        v80 = v79 ^ (v79 >> 31);
                        v83 = 8073958;
                        break;
                    case 0xEF98E63u:
                        v78 -= 0x40A7B892E31B1A47i64;
                        v79 = v78;
                        v83 = -1146502531;
                        break;
                    }
                }
                v82 ^= v83;
            } while (v82 != -1300246908);
        } while (*v77);
        goto LABEL_1189;
    case 0xB4:
        *v1 = -123;
        v84 = v1 + 1;
        v85 = 0i64;
        v86 = 0;
        v87 = -720331145;
        v2 = 2984848294i64;
        do
        {
            if (v87 > 0xABC4C849)
            {
                switch (v87)
                {
                case 0xAC498014:
                    v85 = 0xC6CBB7FA0CF690C6ui64;
                    v2 = 166375324i64;
                    break;
                case 0xC646A271:
                    v85 = 0xC3DD868C29E56F2Eui64;
                    v2 = 3215893911i64;
                    break;
                case 0xD5109E77:
                    v85 = 0x51B5D60B9871AEABi64;
                    v2 = 1004650348i64;
                    break;
                case 0xF886C2A9:
                    v85 = 0x8B4795C1594F91BAui64;
                    v2 = 3133789275i64;
                    break;
                }
            }
            else
            {
                switch (v87)
                {
                case 0xABC4C849:
                    v85 = 0x26531466E8003B7Fi64;
                    v2 = 228584197i64;
                    break;
                case 0x3991543u:
                    v85 = 0xDC1B1B86B1B829F8ui64;
                    v2 = 3616367200i64;
                    break;
                case 0x4FA5F68u:
                    v85 = 0x33DDD78EAF460DD5i64;
                    v2 = 1878444183i64;
                    break;
                case 0x44C4AC77u:
                    v85 = 0x267DEC049DF1513Di64;
                    v2 = 2191384796i64;
                    break;
                case 0x49842789u:
                    v85 = 0x296717BEDAB04F16i64;
                    v2 = 2816727766i64;
                    break;
                case 0x5CE14CB2u:
                    v85 = 0x20217AFF5D23F077i64;
                    v2 = 3332971825i64;
                    break;
                }
            }
            v87 ^= v2;
        } while (v87 != -286187237);
        v88 = 0i64;
        v1 = str;
        if (!*v84)
            goto LABEL_1190;
        v2 = 0xBA623075F071EABi64;
        while (1)
        {
            v89 = -672113044;
            v90 = 1190474386;
            do
            {
                if (v89 > 0x6AC1CDB2)
                {
                    if (v89 <= 0xD0B22558)
                    {
                        switch (v89)
                        {
                        case 0xD0B22558:
                            v88 = 0x9E3779B97F4A7C15ui64 * ((v88 >> 27) ^ v88);
                            v90 = 1939521616;
                            goto LABEL_884;
                        case 0x71347F46u:
                            v88 = 0xFC91B84BDED38DBi64 * ((v88 >> 30) ^ v88);
                            v90 = -1919163174;
                            goto LABEL_884;
                        case 0x8CDA33C6:
                            --v84;
                            v90 = 73863477;
                            goto LABEL_884;
                        case 0x94408DAA:
                            v88 = 0xEA94DA20924A33i64 * ((v88 >> 27) ^ v88);
                            v90 = 1948004819;
                            goto LABEL_884;
                        }
                        if (v89 != -1784429874)
                            goto LABEL_884;
                        v85 -= 0x40A7B892E31B1A47i64;
                        v90 = -701468375;
                        goto LABEL_883;
                    }
                    switch (v89)
                    {
                    case 0xD7F05E6C:
                        v85 -= 0x61C8864680B583EBi64;
                        v90 = 766437527;
                        goto LABEL_883;
                    case 0xE4154924:
                        *v84 = RotateLeft8(*v84, v86);
                        v90 = 11626748;
                        v2 = 0xBA623075F071EABi64;
                        break;
                    case 0xF1159E93:
                        v86 = (v88 ^ ((unsigned __int8)v88 >> 2)) & 0x1C;
                        v90 = -2146190751;
                        break;
                    case 0xFA5EB6FB:
                        v88 = 0xBF58476D1CE4E5B9ui64 * ((v88 >> 30) ^ v88);
                        v90 = -867287673;
                        break;
                    }
                }
                else
                {
                    if (v89 == 1791086002)
                    {
                        v85 += 0xBA623075F071EABi64;
                        v90 = -885743394;
                        goto LABEL_883;
                    }
                    if (v89 > 0x2696EC3F)
                    {
                        switch (v89)
                        {
                        case 0x36108B7Cu:
                            v88 = 0x94D049BB133111EBui64 * ((v88 >> 27) ^ v88);
                            v90 = 342604250;
                            break;
                        case 0x4D9E8DF3u:
                            v85 -= 0x4A615D4A21C725DEi64;
                            v90 = -1583205324;
                        LABEL_883:
                            v88 = v85;
                            break;
                        case 0x646ADCEDu:
                            v86 = v88 ^ (v88 >> 31);
                            v90 = 1169591198;
                            break;
                        case 0x69FE45BFu:
                            v88 = 0x94D049BB133111EBui64 * ((v88 >> 30) ^ v88);
                            v90 = 4992639;
                            break;
                        }
                    }
                    else
                    {
                        switch (v89)
                        {
                        case 0x2696EC3Fu:
                            v86 = (v88 ^ (v88 >> 31)) & 0x21;
                            v90 = 458795342;
                            break;
                        case 0x2A45E0Fu:
                            ++v84;
                            v90 = 1988520373;
                            break;
                        case 0x393D4DEu:
                            v88 = 0xD2935E8470B8CD9Cui64 * ((v88 >> 46) ^ v88);
                            v90 = 758437455;
                            break;
                        case 0xB36E383u:
                            v88 = 0x2C70FEC9EE656E3Ei64 * ((v88 >> 63) ^ v88);
                            v90 = 1660315740;
                            break;
                        case 0xECE99F1u:
                            *v84 = RotateRight8(*v84, v86);
                            v90 = 208324606;
                            v2 = 0xBA623075F071EABi64;
                            break;
                        case 0x227B32A6u:
                            v86 = v88 ^ (v88 >> 31);
                            v90 = 750103383;
                            break;
                        }
                    }
                }
            LABEL_884:
                v89 ^= v90;
            } while (v89 != 1948399546);
            if (!*v84)
                goto LABEL_1189;
        }
    case 0xB7:
        *v1 = -116;
        v91 = v1 + 1;
        v92 = 0i64;
        v93 = 0i64;
        v94 = 0;
        v95 = 2072509062;
        v2 = 3743988976i64;
        do
        {
            if (v95 > 0x73ED4E61)
            {
                switch (v95)
                {
                case 0x7B87FA86u:
                    v92 = 0xF0F151E56813C624ui64;
                    v2 = 363557606i64;
                    break;
                case 0x9563A7FD:
                    v92 = 0xFECC8B86ABBB7DCDui64;
                    v2 = 193928490i64;
                    break;
                case 0xB114688D:
                    v92 = 0x598ADE5FE0DD16C7i64;
                    v2 = 622782205i64;
                    break;
                case 0xE3B121FD:
                    v92 = 0x564211B391FB5B53i64;
                    v2 = 36999782i64;
                    break;
                }
            }
            else
            {
                switch (v95)
                {
                case 0x73ED4E61u:
                    v92 = 0xB75AB0670334A98Aui64;
                    v2 = 15285923i64;
                    break;
                case 0x75E055Au:
                    v92 = 0xF34C2AD323FE8021ui64;
                    v2 = 2600069107i64;
                    break;
                case 0xAF4FB86u:
                    v92 = 0xE9AECE4C8954B6F7ui64;
                    v2 = 2528661348i64;
                    break;
                case 0xEBFD6DDu:
                    v92 = 0xF3E80E7FDFEA18E3ui64;
                    v2 = 2820179420i64;
                    break;
                case 0x1830E990u:
                    v92 = 0x6B6B734D5C8E24C9i64;
                    v2 = 3969296340i64;
                    break;
                case 0x185B43EEu:
                    v92 = 0xFCD665BA5A2118C8ui64;
                    v2 = 4166171044i64;
                    break;
                }
            }
            v95 ^= v2;
        } while (v95 != 1848412256);
        v1 = str;
        if (!*v91)
            goto LABEL_1190;
        v2 = 0x48AC152B1907C8C0i64;
        do
        {
            v96 = -786248412;
            v97 = -229841500;
            do
            {
                if (v96 > 0x732292F0)
                {
                    if (v96 > 0xD122CD24)
                    {
                        switch (v96)
                        {
                        case 0xE6A999B4:
                            v93 = 0xBF58476D1CE4E5B9ui64 * ((v93 >> 30) ^ v93);
                            v97 = 1320292763;
                            break;
                        case 0xE813DF99:
                            v94 = (v93 ^ (v93 >> 31)) & 0x13;
                            v97 = -649320134;
                            break;
                        case 0xF3B6E59D:
                            *v91 = RotateLeft8(*v91, v94);
                            v97 = -1529660721;
                            v2 = 0x48AC152B1907C8C0i64;
                            break;
                        case 0xF45D363B:
                            v93 = 0x9E3779B97F4A7C15ui64 * ((v93 >> 27) ^ v93);
                            v97 = 1887121783;
                            break;
                        case 0xFA9BD0EB:
                            v93 = 0x94D049BB133111EBui64 * ((v93 >> 30) ^ v93);
                            v97 = 96561236;
                            break;
                        }
                    }
                    else if (v96 == -786248412)
                    {
                        v92 -= 0x61C8864680B583EBi64;
                        v93 = v92;
                        v97 = 931878032;
                    }
                    else if (v96 > 0xA5D0793E)
                    {
                        if (v96 == -1511428012)
                        {
                            v94 = (v93 ^ (v93 >> 13)) & 0x24;
                            v97 = 975196444;
                        }
                        else if (v96 == -1474590673)
                        {
                            v93 = 0x94D049BB133111EBui64 * ((v93 >> 27) ^ v93);
                            v97 = 789819613;
                        }
                    }
                    else
                    {
                        switch (v96)
                        {
                        case 0xA5D0793E:
                            ++v91;
                            v97 = -1952131928;
                            break;
                        case 0x7A193440u:
                            v93 = 0xD67222592EC0F7BBui64 * ((v93 >> 30) ^ v93);
                            v97 = -531171127;
                            break;
                        case 0x8014FB2C:
                            v92 -= 0x48AC152B1907C8C0i64;
                            v93 = v92;
                            v97 = 2032631424;
                            break;
                        case 0x870838F2:
                            v94 = v93 ^ (v93 >> 31);
                            v97 = -456297245;
                            break;
                        }
                    }
                }
                else if (v96 == 1931645680)
                {
                    v94 = v93 ^ (v93 >> 31);
                    v97 = -185376054;
                }
                else if (v96 > 0x17BA4043)
                {
                    switch (v96)
                    {
                    case 0x185A18BCu:
                        --v91;
                        v97 = 397693495;
                        break;
                    case 0x42D1CA7Bu:
                        v92 -= 0x40A7B892E31B1A47i64;
                        v93 = v92;
                        v97 = -2021130885;
                        break;
                    case 0x63C54C11u:
                        *v91 = RotateRight8(*v91, v94);
                        v97 = -971688657;
                        v2 = 0x48AC152B1907C8C0i64;
                        break;
                    }
                }
                else
                {
                    switch (v96)
                    {
                    case 0x17BA4043u:
                        v93 = 0xCCEF5D168DC5B69Eui64 * ((v93 >> 61) ^ v93);
                        v97 = -768796870;
                        break;
                    case 0x1D453D8u:
                        v93 = 0x11FFE6E125DB4837i64 * ((v93 >> 34) ^ v93);
                        v97 = -231927232;
                        break;
                    case 0x399603Au:
                        v92 -= 0xF06C0640C22E681i64;
                        v93 = v92;
                        v97 = 1067936496;
                        break;
                    case 0x475B65Eu:
                        v93 = 0x58333AFA6DD6838Di64 * ((v93 >> 27) ^ v93);
                        v97 = -942429879;
                        break;
                    }
                }
                v96 ^= v97;
            } while (v96 != 779398550);
        } while (*v91);
        goto LABEL_1189;
    case 0xB9:
        *v1 = -111;
        v98 = 0;
        v2 = 0i64;
        if ((unsigned __int8)v1[1] - 1 <= 0)
            goto LABEL_1116;
        v99 = v1 + 2;
        do
        {
            v98 += v2;
            if (*v99 != -71 - v98)
                *v99 ^= -71 - v98;
            v2 = (unsigned int)(v2 + 1);
            ++v99;
        } while ((int)v2 < (unsigned __int8)v1[1] - 1);
        v60 = 2i64;
        goto LABEL_1191;
    case 0xBA:
        *v1 = -97;
        v100 = v1 + 1;
        v101 = 0i64;
        v102 = 0;
        v103 = 1005169899;
        v2 = 119450667i64;
        do
        {
            if (v103 > 0x8424B51F)
            {
                switch (v103)
                {
                case 0x9F1A54EF:
                    v101 = 0x458C73BB912F0DFCi64;
                    v2 = 267793702i64;
                    break;
                case 0xDCA0315E:
                    v101 = 0xB4A395B96D15F4FDui64;
                    v2 = 4100878412i64;
                    break;
                case 0xE9E6C9DC:
                    v101 = 0x56A679ED99E57A7Di64;
                    v2 = 1774560382i64;
                    break;
                case 0xEEDE8833:
                    v101 = 0xDBBFB5EFA898FEC2ui64;
                    v2 = 3404121852i64;
                    break;
                }
            }
            else
            {
                switch (v103)
                {
                case 0x8424B51F:
                    v101 = 0x5DF9E5E9BBCDE548i64;
                    v2 = 3766371013i64;
                    break;
                case 0x52F979u:
                    v101 = 0x5D77357A62C4CA88i64;
                    v2 = 1134079559i64;
                    break;
                case 0x966499Fu:
                    v101 = 0x5503AD3088780D2Ei64;
                    v2 = 2360793319i64;
                    break;
                case 0x153F281Bu:
                    v101 = 0xF940FFF78C84D030ui64;
                    v2 = 2616651666i64;
                    break;
                case 0x3BE9ACEBu:
                    v101 = 0x49CD125C1B79E441i64;
                    v2 = 2016999463i64;
                    break;
                case 0x74A690E6u:
                    v101 = 0x5C6B67004909FA8Di64;
                    v2 = 3023660910i64;
                    break;
                }
            }
            v103 ^= v2;
        } while (v103 != 1137792204);
        v104 = 0i64;
        v1 = str;
        if (!*v100)
            goto LABEL_1190;
        v2 = 0x3260DEAC281D7C95i64;
        do
        {
            v105 = -1247531378;
            v106 = 1900985024;
            do
            {
                if (v105 > 0x69D3C912)
                {
                    if (v105 > 0xC1837EF8)
                    {
                        switch (v105)
                        {
                        case 0xC3AF4D3E:
                            v104 = 0xA60B6154DEC4157i64 * (HIDWORD(v104) ^ v104);
                            v106 = 1574384286;
                            break;
                        case 0xC48CD675:
                            v102 = v104 ^ (v104 >> 31);
                            v106 = -1260941075;
                            break;
                        case 0xC9BA955F:
                            v104 = 0x4A1F121872D1CD6i64 * ((v104 >> 27) ^ v104);
                            v106 = 216187603;
                            break;
                        case 0xECEBE4F1:
                            v101 -= 0x3260DEAC281D7C95i64;
                            v104 = v101;
                            v106 = 1093321271;
                            break;
                        case 0xFB336328:
                            v102 = (v104 ^ (v104 >> 31)) & 2;
                            v106 = -801525621;
                            break;
                        }
                    }
                    else if (v105 == -1048346888)
                    {
                        --v100;
                        v106 = 1070297887;
                    }
                    else if (v105 > 0x9F65B820)
                    {
                        if (v105 == -1388695124)
                        {
                            v104 = 0xBF58476D1CE4E5B9ui64 * ((v104 >> 30) ^ v104);
                            v106 = 845121932;
                        }
                        else if (v105 == -1247531378)
                        {
                            v101 -= 0x61C8864680B583EBi64;
                            v104 = v101;
                            v106 = 413015842;
                        }
                    }
                    else
                    {
                        switch (v105)
                        {
                        case 0x9F65B820:
                            v104 = 0x94D049BB133111EBui64 * ((v104 >> 27) ^ v104);
                            v106 = 1542024789;
                            break;
                        case 0x705B4698u:
                            *v100 = RotateRight8(*v100, v102);
                            v106 = 1552918218;
                            v2 = 0x3260DEAC281D7C95i64;
                            break;
                        case 0x874C2AC5:
                            v101 -= 0x40A7B892E31B1A47i64;
                            v104 = v101;
                            v106 = 211321227;
                            break;
                        case 0x9B5F688F:
                            v102 = (v104 ^ ((unsigned __int8)v104 >> 3)) & 0xE;
                            v106 = -1099302952;
                            break;
                        }
                    }
                }
                else if (v105 == 1775487250)
                {
                    v104 = 0x94D049BB133111EBui64 * ((v104 >> 30) ^ v104);
                    v106 = 1801028085;
                }
                else if (v105 > 0x578736C)
                {
                    switch (v105)
                    {
                    case 0xCA4B067u:
                        v102 = v104 ^ (v104 >> 31);
                        v106 = 686686123;
                        break;
                    case 0xF69C3B4u:
                        v101 -= 0x2621DDB0D95EB577i64;
                        v104 = v101;
                        v106 = 1088983451;
                        break;
                    case 0x2CD4E052u:
                        ++v100;
                        v106 = -1180167464;
                        break;
                    }
                }
                else
                {
                    switch (v105)
                    {
                    case 0x578736Cu:
                        v104 = 0x4BC4F7EACF27BE59i64 * ((v104 >> 57) ^ v104);
                        v106 = -1436896699;
                        break;
                    case 0x47CCEEu:
                        v104 = 0x53DD48DF10A2CE54i64 * ((v104 >> 30) ^ v104);
                        v106 = -879134297;
                        break;
                    case 0x1BCF490u:
                        *v100 = RotateLeft8(*v100, v102);
                        v106 = 1528544228;
                        v2 = 0x3260DEAC281D7C95i64;
                        break;
                    case 0x296E63Bu:
                        v104 = 0x9E3779B97F4A7C15ui64 * ((v104 >> 27) ^ v104);
                        v106 = -2059042784;
                        break;
                    }
                }
                v105 ^= v106;
            } while (v105 != -1786973558);
        } while (*v100);
        goto LABEL_1189;
    case 0xBC:
        *v1 = -92;
        v107 = v1 + 1;
        v108 = 0i64;
        v109 = 0;
        v110 = 194008443;
        v2 = 680340319i64;
        do
        {
            if (v110 > 0xB0292653)
            {
                switch (v110)
                {
                case 0xB3985B2F:
                    v108 = 0xFB6EE49552A67DEBui64;
                    v2 = 3484647773i64;
                    break;
                case 0xBE94B4A5:
                    v108 = 0xBC73B3D42D9D7ECi64;
                    v2 = 191637108i64;
                    break;
                case 0xF052AD89:
                    v108 = 0xBC1FAED49271241Aui64;
                    v2 = 1590555931i64;
                    break;
                case 0xF6E4ECCD:
                    v108 = 0xD5BF2CB2331B8C0Fui64;
                    v2 = 1943286648i64;
                    break;
                }
            }
            else
            {
                switch (v110)
                {
                case 0xB0292653:
                    v108 = 0x6DC606FF309BDAE7i64;
                    v2 = 4256091642i64;
                    break;
                case 0x8971880u:
                    v108 = 0xAFF0F28170C7FCB5ui64;
                    v2 = 3505080474i64;
                    break;
                case 0xB90557Bu:
                    v108 = 0xAACCB8377A4707FFui64;
                    v2 = 100222023i64;
                    break;
                case 0x119FD867u:
                    v108 = 0x3258D32F380640E9i64;
                    v2 = 426348479i64;
                    break;
                case 0x53D015A9u:
                    v108 = 0x3D4045DECB0A849i64;
                    v2 = 187919747i64;
                    break;
                case 0x65DBDE9Cu:
                    v108 = 0xFAEC84A136A550DFui64;
                    v2 = 569559731i64;
                    break;
                }
            }
            v110 ^= v2;
        } while (v110 != 241766716);
        v111 = 0i64;
        v1 = str;
        if (!*v107)
            goto LABEL_1190;
        v2 = 0x2571B2978DD26D68i64;
        do
        {
            v112 = -826402112;
            v113 = -1007740820;
            do
            {
                if (v112 > 0x3F37C43A)
                {
                    if (v112 > 0xA58CAA26)
                    {
                        switch (v112)
                        {
                        case 0xA8378D74:
                            v111 = 0x9E3779B97F4A7C15ui64 * ((v111 >> 27) ^ v111);
                            v113 = 1565254254;
                            break;
                        case 0xBE58EB88:
                            v111 = 0x6ECB6DB4E7F8671Bi64 * ((v111 >> 30) ^ v111);
                            v113 = -437939853;
                            break;
                        case 0xCEBE1AC0:
                            v108 -= 0x61C8864680B583EBi64;
                            v111 = v108;
                            v113 = -1977203071;
                            break;
                        case 0xE0362863:
                            v109 = v111 ^ (v111 >> 31);
                            v113 = 2125174366;
                            break;
                        }
                    }
                    else
                    {
                        switch (v112)
                        {
                        case 0xA58CAA26:
                            v111 = 0x94D049BB133111EBui64 * ((v111 >> 27) ^ v111);
                            v113 = -1698992612;
                            break;
                        case 0x3FC992F0u:
                            v111 = 0x90FA259219292730ui64 * ((v111 >> 9) ^ v111);
                            v113 = 1719181259;
                            break;
                        case 0x41E35DE7u:
                            *v107 = RotateRight8(*v107, v109);
                            v113 = 2025531758;
                            v2 = 0x2571B2978DD26D68i64;
                            break;
                        case 0x44985C41u:
                            v111 = 0xBF58476D1CE4E5B9ui64 * ((v111 >> 30) ^ v111);
                            v113 = -518719897;
                            break;
                        case 0x5D6670B2u:
                            v111 = 0x76464DF0ECB78E16i64 * ((v111 >> 27) ^ v111);
                            v113 = 1851815588;
                            break;
                        }
                    }
                }
                else if (v112 == 1060619322)
                {
                    v109 = v111 ^ (v111 >> 31);
                    v113 = 2127862237;
                }
                else if (v112 > 0x2D1B3818)
                {
                    switch (v112)
                    {
                    case 0x33804DA8u:
                        --v107;
                        v113 = -227225929;
                        break;
                    case 0x384F981Cu:
                        v108 -= 0x40A7B892E31B1A47i64;
                        v111 = v108;
                        v113 = 1467714115;
                        break;
                    case 0x39587489u:
                        ++v107;
                        v113 = 179279152;
                        break;
                    case 0x3C7035DAu:
                        v109 = (v111 ^ (v111 >> 11)) & 0x16;
                        v113 = -428789103;
                        break;
                    }
                }
                else
                {
                    switch (v112)
                    {
                    case 0x2D1B3818u:
                        v111 = 0x8E6A9338E8324DCi64 * ((v111 >> 63) ^ v111);
                        v113 = 708819522;
                        break;
                    case 0xE80A77u:
                        v109 = (v111 ^ (v111 >> 31)) & 0x25;
                        v113 = 1289701968;
                        break;
                    case 0x93D333Du:
                        v111 = 0x94D049BB133111EBui64 * ((v111 >> 30) ^ v111);
                        v113 = -700396846;
                        break;
                    case 0xA429FE1u:
                        v108 += 0x2571B2978DD26D68i64;
                        v111 = v108;
                        v113 = 1021979457;
                        break;
                    case 0xBFEC59Bu:
                        *v107 = RotateLeft8(*v107, v109);
                        v113 = 60511872;
                        v2 = 0x2571B2978DD26D68i64;
                        break;
                    case 0xCFA7592u:
                        v108 += 0x40519BB8409E775i64;
                        v111 = v108;
                        v113 = -1910674209;
                        break;
                    }
                }
                v112 ^= v113;
            } while (v112 != 871883193);
        } while (*v107);
        goto LABEL_1189;
    case 0xBD:
        *v1 = -111;
        v114 = 0;
        v2 = 0i64;
        if ((unsigned __int8)v1[1] - 1 > 0)
        {
            v115 = v1 + 2;
            do
            {
                v114 += v2;
                if (*v115 != v114 - 67)
                    *v115 ^= v114 - 67;
                v2 = (unsigned int)(v2 + 1);
                ++v115;
            } while ((int)v2 < (unsigned __int8)v1[1] - 1);
        }
    LABEL_1116:
        v60 = 2i64;
        goto LABEL_1191;
    case 0xBE:
        *v1 = 0x80;
        v116 = v1 + 1;
        v117 = 0i64;
        v118 = 0;
        v119 = -731327823;
        v2 = 383669752i64;
        do
        {
            if (v119 > 0xAE399097)
            {
                switch (v119)
                {
                case 0xC06FD533:
                    v117 = 0x1CB3F9CA3636DD95i64;
                    v2 = 857977489i64;
                    break;
                case 0xCEFD2FA1:
                    v117 = 0x13E1F75EA1D6282Di64;
                    v2 = 2469350020i64;
                    break;
                case 0xD468D2B1:
                    v117 = 0x839990996A1F5F92ui64;
                    v2 = 3079923191i64;
                    break;
                case 0xEF874138:
                    v117 = 0x1A695AFCBABCA560i64;
                    v2 = 3640398362i64;
                    break;
                }
            }
            else
            {
                switch (v119)
                {
                case 0xAE399097:
                    v117 = 0xBEF574393EE78112ui64;
                    v2 = 10908914i64;
                    break;
                case 0x5D20C65Du:
                    v117 = 0x1D9B207AA29A15BDi64;
                    v2 = 2107778953i64;
                    break;
                case 0x78D78B66u:
                    v117 = 0x1E3E83EB1CE434DAi64;
                    v2 = 149282777i64;
                    break;
                case 0x90CC91B5:
                    v117 = 0xC66F64EAEF5DE849ui64;
                    v2 = 518780288i64;
                    break;
                case 0x95D0C8ED:
                    v117 = 0x61816A5B11E7A7C9i64;
                    v2 = 2195703539i64;
                    break;
                case 0x9B455C8C:
                    v117 = 0x885419CBAE788A2i64;
                    v2 = 3500153902i64;
                    break;
                }
            }
            v119 ^= v2;
        } while (v119 != 1677408070);
        v120 = 0i64;
        v1 = str;
        if (!*v116)
            goto LABEL_1190;
        v2 = 0xF0016A4A789F0i64;
        break;
    default:
        //v127 = sub_2D4C170(v2);
        //if (!--dword_10748F58)
        //{
        //    while (v127 != _InterlockedCompareExchange64(&qword_10748F50, 0i64, v127))
        //        ;
        //}
        return v1;
    }
    do
    {
        v121 = 56496386;
        v122 = 183339377;
        do
        {
            if (v121 > 0x2E133DFE)
            {
                if (v121 > 0xBFC32CC2)
                {
                    switch (v121)
                    {
                    case 0xC5C22CD7:
                        v123 = (v120 >> 27) ^ v120;
                        v122 = 2054233881;
                        goto LABEL_1186;
                    case 0xC6C04873:
                        v120 = 0xA823312DFC52CA6i64 * ((v120 >> 20) ^ v120);
                        v122 = 196935580;
                        break;
                    case 0xCEF4CB37:
                        v120 = 0xBF58476D1CE4E5B9ui64 * ((v120 >> 30) ^ v120);
                        v122 = 188147680;
                        break;
                    case 0xEDC48D7E:
                        --v116;
                        v122 = 250982505;
                        break;
                    }
                }
                else
                {
                    switch (v121)
                    {
                    case 0xBFC32CC2:
                        v120 = 0x33BC575CE3A9F64i64 * ((v120 >> 30) ^ v120);
                        v122 = 287095326;
                        break;
                    case 0x3105CB1Cu:
                        *v116 = RotateRight8(*v116, v118);
                        v122 = 817665472;
                        v2 = 0xF0016A4A789F0i64;
                        break;
                    case 0x3BDA5DB9u:
                        v120 = 0xA14CE9F23CD33B39ui64 * ((v120 >> 27) ^ v120);
                        v122 = -1644299470;
                        break;
                    case 0x785737D5u:
                        *v116 = RotateLeft8(*v116, v118);
                        v122 = 1653260630;
                        v2 = 0xF0016A4A789F0i64;
                        break;
                    case 0xBFB333CE:
                        v118 = v120 ^ (v120 >> 31);
                        v122 = -1900611374;
                        break;
                    }
                }
            }
            else
            {
                if (v121 == 773012990)
                {
                    v120 = 0x9E3779B97F4A7C15ui64 * ((v120 >> 27) ^ v120);
                    v122 = 1196524492;
                    goto LABEL_1187;
                }
                if (v121 > 0xCEFA467)
                {
                    switch (v121)
                    {
                    case 0xE46CE3Bu:
                        v118 = (v120 ^ (v120 >> 12)) & 0x1F;
                        v122 = -407063973;
                        break;
                    case 0xEBB4269u:
                        v117 += 0xF0016A4A789F0i64;
                        v120 = v117;
                        v122 = 737441135;
                        break;
                    case 0x1F7D6690u:
                        v117 -= 0x40A7B892E31B1A47i64;
                        v120 = v117;
                        v122 = -1315834371;
                        break;
                    case 0x22865BD1u:
                        v117 += 0x89DA5BE9A6A1697i64;
                        v120 = v117;
                        v122 = -1321553058;
                        break;
                    }
                }
                else
                {
                    switch (v121)
                    {
                    case 0xCEFA467u:
                        v120 = 0xE95F131950146F75ui64 * ((v120 >> 27) ^ v120);
                        v122 = -1863752661;
                        break;
                    case 0x1B95EDCu:
                        ++v116;
                        v122 = 1135416871;
                        break;
                    case 0x35E1102u:
                        v117 -= 0x61C8864680B583EBi64;
                        v120 = v117;
                        v122 = -844441035;
                        break;
                    case 0x763A3D1u:
                        v118 = v120 ^ (v120 >> 31);
                        v122 = 1608752895;
                        break;
                    case 0xC7F882Bu:
                        v123 = (v120 >> 30) ^ v120;
                        v122 = -889659062;
                    LABEL_1186:
                        v120 = 0x94D049BB133111EBui64 * v123;
                        break;
                    case 0xCB5D7E2u:
                        v118 = (v120 ^ (v120 >> 31)) & 0x28;
                        v122 = -1064973159;
                        break;
                    }
                }
            }
        LABEL_1187:
            v121 ^= v122;
        } while (v121 != 1108625659);
    } while (*v116);
LABEL_1189:
    v1 = str;
LABEL_1190:
    v60 = 1i64;
LABEL_1191:
    v124 = &v1[v60];
    return v124;
}
char* decrypt::DecryptStringT8Old(char* str) {
    return (char*)DecryptStringT8Old0((byte*)str);
}

#pragma warning(pop)