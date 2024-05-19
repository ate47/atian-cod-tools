grammar ddl;

prog: ((data | struct | enum) ';')* EOF;

data: IDENTIFIER (STRING | number);

enum: 'enum' IDENTIFIER '{' (STRING ('=' number)? (',' STRING ('=' number)?)*)? '}';

struct: 'struct' IDENTIFIER '{' struct_def* '}';

struct_def: IDENTIFIER IDENTIFIER ('[' number ']' ('[' number ']')?)? ';' ;

number: INTEGER10 | INTEGER16 | INTEGER8 | INTEGER2 ;

NEWLINE: ('\r'? '\n' | '\r') -> skip;
WHITESPACE: ('\t' | ' ') -> skip;

INTEGER10: '-'? [1-9]([0-9])*;
INTEGER16: '-'? '0' [xX] ([0-9a-fA-F])+;
INTEGER8: '-'? '0' ([0-7])*;
INTEGER2: '-'? '0' [bB] ([01])*;
IDENTIFIER: [a-z_A-Z] ([a-z_A-Z0-9])*;
PATH: [a-z_A-Z0-9\\/.]+;
STRING: '"' (~["\\] | ('\\' .))* '"';
