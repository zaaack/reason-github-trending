module CoServer = Cohttp_lwt_unix.Server;

open Shared.Types;

/* type appState = {
     nextId: int,
     todos
   };

   let appState = ref {nextId: 1, todos: [{id: 0, text: "Add some things to do", completed: None}]}; */
let maybe_parse text =>
  try (Some (Yojson.Safe.from_string text)) {
  | _ => None
  };

let withBody body fn => Lwt.(Cohttp_lwt_body.to_string body >>= fn);

let withJsonBody body fn => withBody body (fun body => fn (maybe_parse body));

let withParsedBody body parse fn =>
  withBody
    body
    (
      fun body =>
        switch (maybe_parse body) {
        | None => CoServer.respond_string status::`Bad_request body::"" ()
        | Some json =>
          switch (parse json) {
          | None => CoServer.respond_string status::`Bad_request body::"" ()
          | Some data => fn data
          }
        }
    );

Server.get_prefix "/public" (Server.serveStatic "./public");

/* Server.get "/api/todos" (fun _ _ _ => Server.json (todos__to_yojson (!appState).todos));

   Server.post
     "/api/todo/add"
     (
       fun _ body _ =>
         withParsedBody
           body
           new_todo__from_yojson
           (
             fun text => {
               let id = (!appState).nextId;
               let todo = {completed: None, id, text};
               let todos = List.append (!appState).todos [todo];
               appState := {nextId: id + 1, todos};
               Server.json (todos__to_yojson todos)
             }
           )
     );

   Server.post
     "/api/todo/remove"
     (
       fun _ body _ =>
         withParsedBody
           body
           int__from_yojson
           (
             fun id => {
               let todos = List.filter (fun item => item.id !== id) (!appState).todos;
               appState := {...!appState, todos};
               Server.json (todos__to_yojson todos)
             }
           )
     ); */
module Let_syntax = {
  let bind value ::f => Lwt.bind value f;
  let map value ::f => Lwt.map f value;
};

/*
 Server.post
   "/api/todo"
   Lwt.(
     fun _ body _ => {
       [%await let body = Cohttp_lwt_body.to_string body];
       [%guard let Some data = maybe_parse body]
       [@else CoServer.respond_string status::`Bad_request body::"" ()];
       [%guard let Some data = todo__from_yojson data]
       [@else CoServer.respond_string status::`Bad_request body::"" ()];
       let m = ref 2;
       m := 3;
       let todos = List.map (fun item => item.id === data.id ? data : item) (!appState).todos;
       appState := {...!appState, todos};
       Server.json (todos__to_yojson todos)
     }
   ); */
let headers =
  Cohttp.Header.of_list [
    (
      "User-Agent",
      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.101 Safari/537.36"
    )
  ];

let rec string_to_chars s => {
  let len = String.length s;
  if (len === 0) {
    []
  } else if (len === 1) {
    [s.[0]]
  } else {
    [s.[0], ...string_to_chars (String.sub s 1 (len - 1))]
  }
};

let cache = Hashtbl.create 10;

let cache_life = ref 10;

Server.get
  "/api/trendings"
  Lwt.(
    fun req req_body path => {
      let query = req |> Cohttp.Request.uri |> Uri.get_query_param;
      let href = req |> Cohttp.Request.uri |> Uri.to_string;
      if (!cache_life < 0) {
        cache_life := 10;
        Hashtbl.clear cache
      };
      cache_life := !cache_life - 1;
      if (Hashtbl.mem cache href) {
        Server.json (repos__to_yojson (Hashtbl.find cache href))
      } else {
        let since =
          switch (query "since") {
          | Some s =>
            let s = String.lowercase s;
            switch s {
            | "daily" as s
            | "weekly" as s
            | "monthly" as s => s
            | s =>
              Printf.printf "Invalid query since: %s\n" s;
              "daily"
            }
          | _ => "daily"
          };
        let language =
          switch (query "language") {
          | Some t => t
          | None => ""
          };
        let str = Printf.sprintf "https://github.com/trending/%s?since=%s" language since;
        print_endline ("request:" ^ str);
        [%await
          let (resp, body) =
            Cohttp_lwt_unix.Client.get
              ::headers
              (
                Uri.of_string @@
                Printf.sprintf "https://github.com/trending/%s?since=%s" language since
              )
        ];
        let code = resp |> Cohttp.Response.status |> Cohttp.Code.code_of_status;
        Printf.printf "Loc: %s code: %d" __LOC__ code;
        Printf.printf "Response code: %d\n" code;
        /* Printf.printf "Headers: %s\n" (resp |> Cohttp.Response.headers |> Cohttp.Header.to_string); */
        print_endline "Some";
        /* flush stdout */
        /* flush stdout; */
        [%await let body = Cohttp_lwt_body.to_string body];
        let repos = ref [];
        let filter_num str =>
          string_to_chars str |> List.filter (fun c => Char.code c >= 48 && Char.code c <= 57) |>
          List.map (String.make 1) |>
          String.concat "";
        Soup.(
          parse body $$ ".repo-list li" |>
          iter (
            fun a => {
              let name =
                a $ "div.col-9 h3 a" |> R.attribute "href" |> (
                  fun s => String.sub s 1 (String.length s - 1)
                );
              print_endline ("name:" ^ name);
              let star_total =
                a $ ".f6 > .muted-link:nth-of-type(1)" |> trimmed_texts |> List.rev |> List.hd |> filter_num |> int_of_string;
              print_endline ("star_total:" ^ string_of_int star_total);
              let fork_total =
                a $ ".f6 > .muted-link:nth-of-type(2)" |> trimmed_texts |> List.rev |> List.hd |> filter_num |> int_of_string;
              print_endline ("fork_total:" ^ string_of_int fork_total);
              let star_recent =
                a $ ".f6 > .d-inline-block.float-sm-right" |> trimmed_texts |> List.hd |> filter_num |> int_of_string;
              let desc = a $ ".py-1 p.text-gray" |> trimmed_texts |> List.hd;
              let language = a $ ".f6 > .d-inline-block:first-child" |> trimmed_texts |> List.hd;
              Printf.printf
                "name: %s, star_total: %d, star_recent: %d, fork_total: %d"
                name
                star_total
                star_recent
                fork_total;
              repos := !repos @ [{name, star_total, star_recent, fork_total, desc, language}];
              ()
            }
          )
        );
        /* flush stdout; */
        Hashtbl.replace cache href !repos;
        Server.json (repos__to_yojson !repos)
      }
    }
  );

/* fallback */
Server.get_prefix
  "/" (fun _ _ _ _ => CoServer.respond_string status::`Not_found body::"Welcome to never land" ());

Server.listen 8000;
