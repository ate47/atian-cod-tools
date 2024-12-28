grammar gsc;		

prog: ('/#' | '#/' | function | include | namespace | filenamespace | constexpr | class_def)* EOF;

include: ('#include' | '#using') (IDENTIFIER | PATH) ';';
namespace: '#namespace' IDENTIFIER ';';
filenamespace: '#file' (IDENTIFIER | PATH) ';';
constexpr: ('#constexpr' | '#define') IDENTIFIER '=' expression ';';

class_def: 'class' IDENTIFIER (':' IDENTIFIER (',' IDENTIFIER)* )? '{' class_var* function* '}';
class_var: ('var' IDENTIFIER ('=' expression)? ';');


function
    : ('function')? 
      ('private')? 
	  ('autoexec' ('(' number ')')?)? 
      ('event_handler' '[' IDENTIFIER ']')? 
	  detour_info?
	  IDENTIFIER? '(' param_list ')' 
	  ('=>')? // serious lambda definition
	  statement_block;

detour_info: 'detour' IDENTIFIER ('<' PATH '>' '::' IDENTIFIER)?;

param_list: (param_val (',' param_val)*)?;

param_val: ('*' | '&')? IDENTIFIER ('=' expression)?
         | '...';

expression_list: (expression (',' expression)*)?;

statement_block: '{' (statement)* '}';

statement:
	(IDENTIFIER ':')? (statement_block
	| statement_dev_block
	| statement_for
	| statement_if
	| statement_while
	| statement_foreach
	| statement_inst
	| statement_switch);

statement_dev_block: ('/#' statement* '#/');

statement_for: 'for' '(' expression? ';' expression? ';' expression? ')' statement;

statement_while: 'while' '(' expression ')' statement;

statement_dowhile: 'do' statement 'while' '(' expression ')';

statement_foreach:
	'foreach' '(' IDENTIFIER (',' IDENTIFIER)? 'in' expression ')' statement;

statement_if: 'if' '(' expression ')' statement ('else' statement)?;
statement_switch: 'switch' '(' expression ')' '{' (('case' const_expr | 'default') ':' (statement)*)+'}';
statement_inst: (expression | operator_inst | statement_dowhile | function_call_exp | nop_def | devop_def) ';';

nop_def: ('nop' | 'Nop') ('(' number ')')?;
devop_def: ('DevOp' | 'devop' | 'Devop') '(' number ')';

function_component: 
	( IDENTIFIER '::')? IDENTIFIER 
	| '[' '[' expression ']' ']' 
	| '[' '[' expression ']' ']' '->' IDENTIFIER
	;

operator_inst: BUILTIN (IDENTIFIER | expression)?;


expression:
	set_expression
	| expression0 '?' expression ':' expression
	| expression0
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

expression0: expression0 '??' expression1 | expression1;
expression1: expression1 '||' expression2 | expression2;
expression2: expression2 '&&' expression3 | expression3;
expression3: expression3 '|' expression4 | expression4;
expression4: expression4 '^' expression5 | expression5;
expression5: expression5 '&' expression6 | expression6;
expression6:
	expression6 ('!=' | '==' | '!==' | '===') expression7
	| expression7;
expression7:
	expression7 ('<' | '<=' | '>' | '>=' | '<==' | '>==') expression8
	| expression8;
expression8: expression8 ('<' | '<=') expression9 | expression9;
expression9: expression9 ('<<' | '>>') expression10 | expression10;
expression10: expression10 ('+' | '-') expression11 | expression11;
expression11: expression11 ('*' | '/' | '%') expression12 | expression12;
expression12: ('!' | '~') expression13 | ('++' | '--') left_value | left_value ('++' | '--') | is_expression | expression13;
expression13: function_call_exp | expression14;
expression14: const_expr | expression15 | left_value;
expression15: ('(' expression ')');

is_expression: expression13 'is' ('not')? (IDENTIFIER | BOOL_VALUE | 'function' | UNDEFINED_VALUE);

function_call_exp:
	function_call 
	| expression14 ('thread' | 'childthread' | 'threadendon' | 'builtin')? function_component '(' expression_list ')';


function_call: 
	('thread' | 'childthread' | 'threadendon'  | 'builtin')? function_component '(' expression_list ')'
	| (const_expr | expression15 ) ('thread' | 'childthread' | 'threadendon'  | 'builtin')? function_component '(' expression_list ')'
    | function_call ('thread' | 'childthread' | 'threadendon'  | 'builtin')? function_component '(' expression_list ')';

left_value:
	idf
	| left_value ('.' | '?.') (idf | ('(' expression ')'))
	| left_value '[' expression ']'
	| (function_call | const_expr | expression15) ('.' | '?.') (idf | ('(' expression ')'))
	| (function_call | const_expr | expression15) '[' expression ']';

const_expr:
	  class_init
	| array_def
	| struct_def 
	| function
	| const_expr_static
	| UNDEFINED_VALUE
	| function_ref
	| data_ref
	;

const_expr_static:
	  vector_value
	| number
    | BOOL_VALUE
    | FLOATVAL
    | STRING
    | HASHSTRING
	;

function_ref:
	'&' (IDENTIFIER '::')? IDENTIFIER
	| '@' IDENTIFIER '<' PATH '>' '::' IDENTIFIER
	| '&' left_value
    ;

data_ref:  '@' '[' IDENTIFIER ',' IDENTIFIER ']' ;

number: INTEGER10
	| INTEGER16
	| INTEGER8
	| INTEGER2
	| IDENTIFIER
	;

vector_value: '(' expression ',' expression ',' expression ')';
array_def:
	'[' ((expression ':')? expression ( ',' (expression ':')? expression)* (',')?)? ']';
struct_def: '{' ((STRUCT_IDENTIFIER | expression) ':' expression (',' (STRUCT_IDENTIFIER | expression) ':' expression)* (',')?)? '}';
class_init: 'new' IDENTIFIER '(' (expression (',' expression)*)? ')';

idf: IDENTIFIER;

NEWLINE: ('\r'? '\n' | '\r') -> skip;
WHITESPACE: ('\t' | ' ') -> skip;

INTEGER10: '-'? [1-9]([0-9])*;
INTEGER16: '-'? '0' [xX] ([0-9a-fA-F])+;
INTEGER8: '-'? '0' ([0-7])*;
INTEGER2: '-'? '0' [bB] ([01])*;
FLOATVAL: '-'? ((((([0-9])* '.' ([0-9])+) | (([0-9])+ '.' ([0-9])*)) ([eE] ('+'|'-')? [0-9]+)?)|([0-9]+ ([eE] ('+'|'-')? [0-9]+)));
BUILTIN: 'break' | 'continue' | 'goto' | 'return' | 'wait' | 'jumpdev';
BOOL_VALUE: 'true' | 'false';
UNDEFINED_VALUE: 'undefined';
IDENTIFIER: [a-z_A-Z] ([a-z_A-Z0-9])*;
STRUCT_IDENTIFIER: '#' [a-z_A-Z] ([a-z_A-Z0-9])*;
PATH: [a-z_A-Z0-9\\/]+ ('.gsc' | '.csc')?;
STRING: '"' (~["\\] | ('\\'.))* '"';
HASHSTRING: ('r' | '#' | '@' | 't' | '%' | '&') STRING;
