open! Shared.Types;

let post: string => Js.Json.t => (Js.Json.t => unit) => unit = [%bs.raw
  {|
  function (path, json, onDone) {
    fetch(path, {body: JSON.stringify(json), method: 'POST', headers: {'Content-Type': 'application/json'}})
    .then(function(res) {return res.json()})
    .then(function(res) {return onDone(res)}, function (err) {return console.error('failed', err)})
  }
|}
];

let removeItem id onDone =>
  post
    "/api/todo/remove"
    (int__to_json id)
    (
      fun data =>
        switch (todos__from_json data) {
        | None => Js.log "Failed to parse data"
        | Some todos =>
          Js.log2 "New todos" (todos__to_devtools todos);
          onDone todos
        }
    );

let updateItem item onDone =>
  post
    "/api/todo"
    (todo__to_json item)
    (
      fun data =>
        switch (todos__from_json data) {
        | None => Js.log "Failed to parse data"
        | Some todos =>
          Js.log2 "New todos" (todos__to_devtools todos);
          onDone todos
        }
    );

let addItem text onDone =>
  post
    "/api/todo/add"
    (new_todo__to_json text)
    (
      fun data =>
        switch (todos__from_json data) {
        | None => Js.log "Failed to parse data"
        | Some todos =>
          Js.log2 "New todos" (todos__to_devtools todos);
          onDone todos
        }
    );
