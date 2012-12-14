
type node = Node of string
type action =
  | Action of string 
  | Nop
type rule = Rule of node list * node list * action

let string_of_node n = let Node(s) = n in s ;;

let print_rule r =
  let print_nodes ns =
    print_string (String.concat ", " (List.map string_of_node ns)) in
  let Rule(targets, sources, action) = r in
  print_nodes targets;
    print_string " <- ";
    print_nodes sources;
    print_endline (match action with
      | Action(str) -> ": " ^ str;
      | Nop -> "");
;;

let rule_makes rule target = 
  let Rule(targets,_,_) = rule in
    try
      ignore(List.find (fun t -> t = target) targets); 
      true
    with
      | Not_found -> false
;;


let rule_phony = function
  | Rule(_,_,Nop) -> true
  | _ -> false
;;


