%{
  open Syntax;;
  exception EmptyFile;;
%}

%token <string>NODE
%token <string>ACTION
%token COMMA LEFTARROW COLON
%token EOF

%start start
%type <Syntax.rule list> start

%%

start:
  | EOF                                     { raise EmptyFile }
  | rules EOF                               { $1 }

rules:
  | rule                                    { [$1] }
  | rule rules                              { $1::$2 }

rule:
  | nodes LEFTARROW nodes                   { Rule ($1,$3,Nop) }
  | nodes LEFTARROW nodes COLON ACTION      { Rule ($1,$3,Action $5) } 

nodes:
  | NODE                                    { [Node $1] }
  | NODE COMMA nodes                        { (Node $1)::$3 }

%%

