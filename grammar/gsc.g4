grammar gsc;		

prog: (function | include | namespace)* EOF;

include: ('#include' | '#using') (IDENTIFIER | PATH) ';';
namespace: '#namespace' IDENTIFIER ';';

function
    : ('function')? 
      ('private')? 
      ('autoexec')? 
      ('event_handler' '[' IDENTIFIER ']')? 
	IDENTIFIER '(' param_list ')' statement_block;

param_list: (param_val (',' param_val)*)?;

param_val: ('*' | '&')? IDENTIFIER ('=' expression)?
         | '...';

expression_list: (expression (',' expression)*)?;

statement_block: '{' (statement)* '}';

statement:
	(IDENTIFIER ':')? (statement_block
	| statement_for
	| statement_if
	| statement_while
	| statement_foreach
	| statement_inst
	| statement_switch);

statement_for: 'for' '(' expression? ';' expression? ';' expression? ')' statement;

statement_while: 'while' '(' expression ')' statement;

statement_dowhile: 'do' statement 'while' '(' expression ')';

statement_foreach:
	'foreach' '(' IDENTIFIER (',' IDENTIFIER)? 'in' expression ')' statement;

statement_if: 'if' '(' expression ')' statement ('else' statement)?;
statement_switch: 'switch' '(' expression ')' '{' (('case' const_expr | 'default') ':' (statement)*)+'}';
statement_inst: (function_call | operator_inst | statement_dowhile)? ';';

function_call: 
	('thread' | 'childthread')? function_component '(' expression_list ')'
	| expression14 ('thread' | 'childthread')? function_component '(' expression_list ')'
    | function_call ('thread' | 'childthread')? function_component '(' expression_list ')';

function_component: ( IDENTIFIER '::')? IDENTIFIER | '[[' expression ']]' | '[[' expression ']]' '->' IDENTIFIER;

operator_inst: IDENTIFIER IDENTIFIER?;


expression:
	set_expression
	| expression1 '?' expression ':' expression
	| expression1
    ;

set_expression:
	left_value (
		'='
		| '+='
		| '-='
		| '/='
		| '*='
		| '%='
		| '&='
		| '|='
		| '^='
		| '<<='
		| '>>='
		| '~='
	) expression
;

expression1: expression1 '||' expression2 | expression2;
expression2: expression2 '&&' expression3 | expression3;
expression3: expression3 '|' expression4 | expression4;
expression4: expression4 '^' expression5 | expression5;
expression5: expression5 '&' expression6 | expression6;
expression6:
	expression6 ('!=' | '==' | '!==' | '===') expression7
	| expression7;
expression7:
	expression7 ('<' | '<=' | '>' | '>=') expression8
	| expression8;
expression8: expression8 ('<' | '<=') expression9 | expression9;
expression9: expression9 ('<<' | '>>') expression10 | expression10;
expression10: expression10 ('+' | '-') expression11 | expression11;
expression11: expression11 ('*' | '/' | '%') expression12 | expression12;
expression12: ('!' | '~') expression13 | ('++' | '--') left_value | left_value ('++' | '--') | expression13;
expression13: function_call | expression14;
expression14: const_expr | ('(' expression ')') | left_value;

left_value:
	IDENTIFIER
	| '[[' expression ']]' '->' IDENTIFIER
	| (const_expr | ('(' expression ')')) '.' (IDENTIFIER | ('(' expression ')'))
	| (const_expr | ('(' expression ')')) '[' expression ']'
	| left_value '.' (IDENTIFIER | ('(' expression ')'))
	| left_value '[' expression ']';


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

INTEGER10: '-'?[1-9]([0-9])*;
INTEGER16: '-'? '0x' ([0-9a-f])+;
INTEGER8: '-'? '0' ([0-7])*;
INTEGER2: '-'? '0b' ([01])*;
FLOATVAL: '-'?((([0-9])* '.' ([0-9])+) | (([0-9])+ '.' ([0-9])*));
IDENTIFIER: [a-z_A-Z] ([a-z_A-Z0-9])*;
STRUCT_IDENTIFIER: '#' [a-z_A-Z] ([a-z_A-Z0-9])*;
PATH: [a-z_A-Z0-9\\/]+ ('.gsc' | '.csc')?;
STRING: '"' (~["\\] | ('\\'.))* '"';
HASHSTRING: [#] STRING;
BOOL_VALUE: 'true' | 'false';
UNDEFINED_VALUE: 'undefined';
