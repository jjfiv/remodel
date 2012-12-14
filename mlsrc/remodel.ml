open Syntax;;

print_endline "remodel main!" ;;

let filename = "remodelfile" in
let fp = open_in filename in
let buf = Lexing.from_channel fp in
try
  let rules = Parser.start Lexer.lexeme buf in
    List.iter print_rule rules;
with
  | exn -> begin
    let info = buf.Lexing.lex_curr_p in
    let line = info.Lexing.pos_lnum in
    let col = info.Lexing.pos_cnum - info.Lexing.pos_bol in
    let tok = Lexing.lexeme buf in
      print_endline ("remodel:" ^ filename ^ ":" ^ (string_of_int line) ^ ":" ^
      (string_of_int col) ^ ": parse error at token `" ^ tok ^ "'")
  end


