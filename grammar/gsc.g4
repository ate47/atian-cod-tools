grammar gsc;		

prog: (function | include | namespace)* EOF;

include: '#include' (IDENTIFIER | PATH) ';';
namespace: '#namespace' IDENTIFIER ';';

function
    : ('function')? 
      ('private')? 
      ('autoexec')? 
      ('event_handler' '[' IDENTIFIER ']')? 
	IDENTIFIER '(' param_list ')' statement_block;

param_list: (param_val (',' param_val)*)?;

param_val: IDENTIFIER ('=' expression)?;

expression_list: (expression (',' expression)*)?;

statement_block: '{' (statement)* '}';

statement:
	statement_block
	| statement_for
	| statement_if
	| statement_foreach
	| statement_inst;

statement_for:
	'for' '(' expression? ';' expression? ';' expression? ')' statement;

statement_foreach:
	'foreach' '(' IDENTIFIER (',' IDENTIFIER)? 'in' expression ')' statement;

statement_if:
	'if' '(' expression ')' statement ('else' statement)?;

statement_inst: (function_call | operator_inst)? ';';

function_call: (expression)? ('thread' | 'childthread')? (
		IDENTIFIER '::'
	)? IDENTIFIER '(' expression_list ')';

operator_inst: IDENTIFIER (expression)?;

expression
    : IDENTIFIER 
    | const_expr
    ;

const_expr:
	vector_value
	| array_def
	| struct_def
	| number
    | BOOL_VALUE
    | FLOATVAL
    | STRING
    | HASHSTRING
	| UNDEFINED_VALUE
	| function_ref
;
function_ref:
	'&' (IDENTIFIER '::')? IDENTIFIER
	| '@' IDENTIFIER '<' PATH '>' '::' IDENTIFIER
    ;

number: INTEGER10
	| INTEGER16
	| INTEGER8
	| INTEGER2
    ;

vector_value: '(' expression ',' expression ',' expression ')';
array_def:
	'[' (((HASHSTRING | ('#' number)) ':')? expression (',')?)* ']';
struct_def: '{' (STRUCT_IDENTIFIER ':' expression (',')?)* '}';

NEWLINE: ('\r'? '\n' | '\r') -> skip;
WHITESPACE: ('\t' | ' ') -> skip;

INTEGER10: [1-9]([0-9])*;
INTEGER16: '0x' ([0-9a-f])+;
INTEGER8: '0' ([0-7])*;
INTEGER2: '0b' ([01])*;
FLOATVAL: ((([0-9])* '.' ([0-9])+) | (([0-9])+ '.' ([0-9])*));
IDENTIFIER: [a-z_A-Z] ([a-z_A-Z0-9])*;
STRUCT_IDENTIFIER: '#' [a-z_A-Z] ([a-z_A-Z0-9])*;
PATH: [a-z_A-Z0-9\\/]+ ('.gsc' | '.csc')?;
STRING: '"' (~["\\])* '"';
HASHSTRING: [#] STRING;
BOOL_VALUE: 'true' | 'false';
UNDEFINED_VALUE: 'undefined';
