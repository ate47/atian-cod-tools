grammar ddl;

prog: ((begin | version | metatable | struct | enum) ';')* EOF;

begin: 'begin' STRING;
version: 'version' number;
metatable: 'metatable' STRING;

enum: 'enum' IDENTIFIER '{' (STRING ('=' number)? (',' STRING ('=' number)?)*)? '}';

struct: 'struct' IDENTIFIER '{' struct_def* '}';

struct_def: IDENTIFIER IDENTIFIER ('[' number ']' ('[' number ']')?)? ';' ;

number: INTEGER10 | INTEGER16 | INTEGER8 | INTEGER2 ;

NEWLINE: ('\r'? '\n' | '\r') -> skip;
WHITESPACE: ('\t' | ' ') -> skip;

INTEGER10: '-'? [1-9]([0-9])*;
INTEGER16: '-'? '0x' ([0-9a-f])+;
INTEGER8: '-'? '0' ([0-7])*;
INTEGER2: '-'? '0b' ([01])*;
IDENTIFIER: [a-z_A-Z] ([a-z_A-Z0-9])*;
PATH: [a-z_A-Z0-9\\/.]+;
STRING: '"' (~["\\] | ('\\' .))* '"';
