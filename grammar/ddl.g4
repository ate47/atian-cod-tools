grammar ddl;

prog: buffer_data+ EOF;

buffer_data: buffer_flags* 'version' number '{' (enum | struct | struct_def)*  '}';

buffer_flags: '[' IDENTIFIER number? ']';

enum: 'enum' IDENTIFIER '{' (IDENTIFIER (',' IDENTIFIER)*)? '}'  ';';

struct: 'struct' IDENTIFIER '{' struct_def* '}' ';';

struct_def: struct_type IDENTIFIER ('[' (number | IDENTIFIER) ']')? ';';

struct_type: IDENTIFIER | (('uint' | 'int') (':' number)? | ('string' '(' number ')') | ('fixed' '<' number ',' number '>'));

number: INTEGER10 | INTEGER16 | INTEGER8 | INTEGER2 ;

NEWLINE: ('\r'? '\n' | '\r') -> skip;
WHITESPACE: ('\t' | ' ') -> skip;

INTEGER10: '-'? [1-9]([0-9])*;
INTEGER16: '-'? '0' [xX] ([0-9a-fA-F])+;
INTEGER8: '-'? '0' ([0-7])*;
INTEGER2: '-'? '0' [bB] ([01])*;
IDENTIFIER: [a-z_A-Z] ([a-z_A-Z0-9])*;
