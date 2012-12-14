open Syntax;;

let arg_help = Args.FlagDef(["-h";"--help";"-?"],"Show help message") in
let arg_remake = Args.FlagDef(["--remake"],"Delete all generated files and rebuild.") in
let arg_clean = Args.FlagDef(["--clean"],"Delete all generated files.") in
let arg_wait = Args.FlagDef(["-w";"-wait"],"Wait for directory lock if taken.") in
let arg_file = Args.OptionDef(["-f";"--file"],"Read FILE as a remodelfile") in
let arg_graph = Args.OptionDef(["--graph"],"Output a Graphviz DOT file to FILE") in
let arg_dir = Args.OptionDef(["-C";"--directory"],"Change to DIRECTORY before doing anything.") in
let arg_jobs = Args.OptionDef(["-j";"--jobs"],"Specify the maximum number of jobs; unlimited if N <= 0.") in
let my_args = [
  arg_file ;
  arg_dir ;
  arg_clean ;
  arg_remake ;
  arg_jobs ;
  arg_wait;
  arg_graph ;
  arg_help ;
  ]
in
  Args.show_help my_args ;
  Args.parse my_args ;

let filename = "remodelfile" in
let fp = open_in filename in
let buf = Lexing.from_channel fp in
try
  let rules = Parser.start Lexer.lexeme buf in
    List.iter print_rule rules;
with
  | exn -> begin
    let (line, col, tok) = Lexer.lexinfo buf in
      print_endline ("remodel:" ^ filename ^ ":" ^ (string_of_int line) ^ ":" ^
      (string_of_int col) ^ ": parse error at token `" ^ tok ^ "'")
  end


