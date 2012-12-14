{
  open Parser ;;
  
  let lexinfo buf =
    let info = buf.Lexing.lex_curr_p in
    let line = info.Lexing.pos_lnum in
    let col = info.Lexing.pos_cnum - info.Lexing.pos_bol in
    let tok = Lexing.lexeme buf in
      (line, col, tok)

  exception LexError of string
  let error s = raise (LexError s)
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
  | _ as x               { error (Printf.sprintf ("bad character '%c'\n") x) }

and quoted buf = parse
  | '"'   { Buffer.contents buf } (* terminating quote *)
  | "\\t" { Buffer.add_char buf '\t'; quoted buf lexbuf }
  | "\\n" { Buffer.add_char buf '\n'; quoted buf lexbuf }
  | "\\\"" { Buffer.add_char buf '"'; quoted buf lexbuf }
  | "\\'" { Buffer.add_char buf '\''; quoted buf lexbuf }
  | "\\\\" { Buffer.add_char buf '\\'; quoted buf lexbuf }
  | eof    { error "eof found within quoted string" }
  | _ as c { Buffer.add_char buf c; quoted buf lexbuf }


