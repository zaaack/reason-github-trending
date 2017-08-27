open Shared.Types;
open Actions;

type state = [`Pending | `Success | `Error error];

type repos_state = {
  language: string,
  since: option since,
  repos: option repos,
  state: option state,
};
type app_state = {
  repos: repos_state,
};

let repos_reducer state action =>
  switch action {
  | FetchReposPending (since, language) => ({
    ...state,
    language: language,
    since: Some since,
    state: Some `Pending
  })
  | FetchReposError error => ({
    ...state,
    state: Some (`Error error)
  })
  | FetchReposSuccess (since, language, repos) => ({
    language: language,
    since: Some since,
    repos: Some repos,
    state: Some `Success
  })
  | _ => state
  };

let app_reducer state action =>
  switch action {
  | ReposActions a => {
      ...state,
      repos: (repos_reducer state.repos a)
    }
  | _ => state
  };

let middlewares store next => Middleware.thunk store @@ Middleware.logger store @@ next;

let store =
  Reductive.Store.create
    reducer::app_reducer preloadedState::{repos: {repos: None, state: None, language: "JavaScript", since: Some Daily}} enhancer::middlewares ();
