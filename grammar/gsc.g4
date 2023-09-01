grammar gsc;		
prog: ((function | include) SEMICO)* EOF ;

include: '#include' (IDENTIFIER|PATH);

function: 'function' IDENTIFIER '(' ')' block;

block: '{' '}';

IDENTIFIER: [A-Za-z_][A-Za-z_0-9]*;
PATH: [A-Za-z_0-9/\\.]+;
SEMICO: ';';

NEWLINE: ('\r'? '\n' | '\r') -> skip;
WHITESPACE: ('\t' | ' ') -> skip;