
type node = Node of string
type action =
  | Action of string 
  | Nop
type rule = Rule of node list * node list * action

let string_of_node n = let Node(s) = n in s ;;

let print_escaped_string s =
  let print_escaped_char = function
    | '\t' -> print_string "\\t"
    | '\n' -> print_string "\\n"
    | '\'' -> print_string "\\'"
    | '\\' -> print_string "\\\\"
    | '"' -> print_string "\\\""
    | c -> print_char c
  in
    print_char '"';
    String.iter print_escaped_char s;
    print_char '"';
;;

let print_rule r =
  let print_nodes ns =
    print_string (String.concat ", " (List.map string_of_node ns)) in
  let Rule(targets, sources, action) = r in
  print_nodes targets;
    print_string " <- ";
    print_nodes sources;
    (match action with
      | Action(str) -> print_string ": "; print_escaped_string str;
      | Nop -> ());
    print_endline "";
;;

let rule_makes rule target = 
  let Rule(targets,_,_) = rule in
    Util.list_contains targets target
;;


let rule_phony = function
  | Rule(_,_,Nop) -> true
  | _ -> false
;;


