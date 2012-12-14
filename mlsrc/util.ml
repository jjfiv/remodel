
let list_contains lst elt =
  try
    ignore(List.find (fun e -> e = elt) lst);
    true
  with
    | Not_found -> false

