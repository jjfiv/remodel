(* types *)
type argdef =
  | FlagDef of string list * string
  | OptionDef of string list * string

let argdef_keys = function
  | FlagDef(xs, _) -> xs
  | OptionDef(xs, _) -> xs

type argument =
  | Parameter of string
  | Flag of string
  | Option of string * string

(* exceptions *)
exception MultipleDefsForArg of string * argdef list;;
exception NeedArgumentForOption of string;;

(* members *)
let argdef_key def = List.hd (argdef_keys def)

let argdef_matches def s = Util.list_contains (argdef_keys def) s

let string_of_argument = function
  | Parameter(s) -> "P: " ^ s
  | Flag(s) -> "F: " ^ s
  | Option(k,v) -> "O: " ^ k ^ "=" ^ v

let show_help (defs: argdef list) =
  print_endline "help:";
  let tuple_from_argdef = function
    | OptionDef(ds,msg) -> (ds,msg);
    | FlagDef(ds,msg) -> (ds,msg);
  in
    List.iter (fun def -> 
      let (ds, msg) = tuple_from_argdef def in
        print_endline (String.concat ", " ds);
        print_endline ("\t" ^ msg);
    ) defs
;;

let parse (defs: argdef list) =
  let rec rparse args defs accum = match args with
    | [] -> accum
    | cur_arg::rest -> begin
      print_endline ("args: " ^ (String.concat ", " args));
      let (hits, misses) = List.partition (fun x -> argdef_matches x cur_arg ) defs in
      let obj, nargs, ndefs = (match hits with
        | [] -> Parameter cur_arg, rest, defs
        | [FlagDef (d::_,_)] -> Flag d, rest, misses
        | [OptionDef (d::_,_)] ->
            if rest <> [] then
              Option (d, List.hd rest), List.tl rest, misses
            else
              raise (NeedArgumentForOption d)
        | _ -> raise (MultipleDefsForArg (cur_arg, hits)))
      in
        print_endline ("nargs: " ^ (String.concat ", " nargs));
        rparse nargs defs (obj::accum)
    end
  in
    let argv = List.tl (Array.to_list Sys.argv) in
      print_endline (String.concat "\n" (List.rev_map string_of_argument (rparse
      argv defs [])) )

