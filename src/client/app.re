open Actions;
open Option;

Devtools.register ();

let str = ReasonReact.stringToElement;
let style = ReactDOMRe.Style.make;


let module App = {
  let component = ReasonReact.statelessComponent "App";
  let handleChange value => ();

  let fetchData dispatch language since => {
    dispatch @@ ReposActions (FetchReposPending (since, language));
    let request = Axios.getc "/api/trendings" (
      Axios.makeConfig
        params::{
          "language": language,
          "since": since |> since__to_json |> Obj.magic |> String.lowercase
        } ()
    );
    request |> Js.Promise.then_ (fun res => {
        let repos = Shared.Types.repos__from_json (res##data) |? [];
        dispatch @@ ReposActions (FetchReposSuccess (since, language, repos));
        Js.Promise.resolve ()
      }) |> Js.Promise.catch (fun err => {
        Js.log err;
        dispatch @@ ReposActions (FetchReposError ({message: "Fetch Failed", status: 0, stack: None}));
        Js.Promise.resolve ()
      }) |> ignore
  };
  let dispatch = Reductive.Store.dispatch Store.store;
  let initalState = (Reductive.Store.getState Store.store).repos;
  fetchData dispatch initalState.language (switch initalState.since {
    | Some since => since
    | _ => Daily
    });
  exception UnknownSinceOption string;
  let make state::(state: Store.app_state) ::dispatch _children => {
    ...component,
    render: fun {state:_state, update} => {
      let state = state.repos;
      Js.log2 "state" state;
        <View style=(style maxWidth::"500px" margin::"0 auto" ())>
          <Header
            state=state
            onLanguageChange=(fun value => fetchData dispatch (value |? "") (state.since |? Daily))
            onSinceChange=(fun value => fetchData dispatch state.language (switch value {
              | Some "Daily" => Daily
              | Some "Monthly" => Monthly
              | Some "Weekly" => Weekly
              | Some x => raise (UnknownSinceOption x)
              | None => Daily
            }))
          />
        (switch state.state {
        | Some `Pending => (
           <View style=(style alignItems::"center" justifyContent::"center"  width::"100%" height::"400px"())>
             <ActivityIndicator size=32.0 color=(200, 200, 200) />
           </View>
        )
        | Some (`Error err) => <View style=(style color::"red" width::"100%" height::"400px" lineHeight::"400px" textAlign::"center" ())>(str err.message)</View>
        | Some `Success => {
          switch state {
          | {repos: Some repos,
            language,
            since: Some since,
            state} => <RepoList items=repos since=since />
          | _ => ReasonReact.nullElement
          }
        }
        | _ => ReasonReact.nullElement
        })
        </View>
    }
  };
};
module Provider = {
  let make = Reductive.Provider.createMake Store.store;
};

ReactDOMRe.renderToElementWithId <Provider component=App.make /> "app";
