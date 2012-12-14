{
  open Parser ;;

  let ignore buf = Lexing.lexeme buf ;;
  let keepstr buf = Lexing.lexeme buf ;;
}

let filename = ['A'-'Z' 'a'-'z' '0'-'9' '/' '.']+

rule lexeme = parse
  | [' ' '\t' '\r' '\n'] { lexeme lexbuf }
  | '#' [^ '\n']* '\n'   { lexeme lexbuf }
  | "<-"                 { LEFTARROW }
  | ':'                  { COLON }
  | ','                  { COMMA }
  | eof                  { EOF }
  | '"' [^ '"']* '"'     { ACTION (Lexing.lexeme lexbuf) }
  | filename             { NODE (Lexing.lexeme lexbuf) }
  | _ as x               { Printf.printf ("error: unhandled character '%c'\n")
  x; EOF }

