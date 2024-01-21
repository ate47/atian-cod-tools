begin "test/default.ddl";
version 2;
metatable "test_metatable";


struct root {
    hash_10 hash_1;
    hash_10 hash_2;
};

struct hash_10 {
    byte hash_11;
    char hash_12[0x10];
    char hash_13[0x10][0x10];
    hash_2e hash_14;
};


enum hash_2e {
    "aaa",
    "bbb",
    "ccc"
};

// test structure "old"
begin "test/default.ddl";
version 1;
metatable "test_metatable";


struct root {
    hash_10 hash_1;
    hash_10 hash_2;
};

struct hash_10 {
    byte hash_11;
    char hash_12[0x10];
    char hash_13[0x10][0x10];
};

