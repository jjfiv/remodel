{
  open Parser ;;
}

let filename = ['A'-'Z' 'a'-'z' '0'-'9' '/' '.']+
let comment = '#' [^ '\n']* '\n'
let whitespace = [' ' '\t' '\r' '\n']

rule lexeme = parse
  | whitespace           { lexeme lexbuf }
  | comment              { lexeme lexbuf }
  | "<-"                 { LEFTARROW }
  | ':'                  { COLON }
  | ','                  { COMMA }
  | eof                  { EOF }
  | '"'                  { let buf = Buffer.create 10 in ACTION (quoted buf lexbuf) }
  | filename             { NODE (Lexing.lexeme lexbuf) }
  | _ as x               { Printf.printf ("error: unhandled character '%c'\n")
  x; EOF }
and quoted buf = parse
  | '"'   { Buffer.contents buf } (* terminating quote *)
  | "\\t" { Buffer.add_char buf '\t'; quoted buf lexbuf }
  | "\\n" { Buffer.add_char buf '\n'; quoted buf lexbuf }
  | "\\\"" { Buffer.add_char buf '"'; quoted buf lexbuf }
  | "\\'" { Buffer.add_char buf '\''; quoted buf lexbuf }
  | "\\\\" { Buffer.add_char buf '\\'; quoted buf lexbuf }
  | eof    { print_endline ("error: eof found within quoted string"); "" }
  | _ as c { Buffer.add_char buf c; quoted buf lexbuf }


