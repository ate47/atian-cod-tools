grammar adl;

prog: def* EOF;

def: (struct_def | flag_def |enum_def | root_def | align_def | type_def) ';';

root_def: '#root' idf;
align_def: '#align' number;

type_def: 'typedef' idf idf;
struct_def: 'struct' ('align' '(' number ')')? ('size' '(' number ')')? idf '{' struct_members '}';
flag_def: 'flag' idf ':' idf '{' enum_members '}';
enum_def: 'enum' idf ':' idf '{' enum_members '}';

struct_members: (struct_member ';')*;
enum_members: (enum_member (',' enum_member)* ','?)?;

struct_member: data_member | data_operator;

data_member: idf ('*')? idf ('[' number ']')?;
data_operator: ('$offset' | '$padding') number;
enum_member: (STRING | idf) ('=' number)?;

idf: IDENTIFIER;

NEWLINE: ('\r'? '\n' | '\r') -> skip;
WHITESPACE: ('\t' | ' ') -> skip;

number: INTEGER10 | INTEGER16 | INTEGER8 | INTEGER2;

INTEGER10: '-'? [1-9]([0-9])*;
INTEGER16: '-'? '0x' ([0-9a-f])+;
INTEGER8: '-'? '0' ([0-7])*;
INTEGER2: '-'? '0b' ([01])*;
BOOL_VALUE: 'true' | 'false';
IDENTIFIER: [a-z_A-Z] ([a-z_A-Z0-9])*;
STRING: '"' (~["\\] | ('\\' .))* '"';