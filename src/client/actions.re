module ReduxThunk = Middleware.ReduxThunk;

open Shared.Types;

type since =
| Daily
| Weekly
| Monthly;

Js.log Daily;
/* type repos_payload = {
  since: option since, /* hack for ppx_autoserialize bug */
  language: string,
  repos
}; */
/* Js.log {since: Some Daily, language: "js", repos: []}; */
/* {
  since: option since, /* hack for ppx_autoserialize bug: https://github.com/jaredly/ppx_autoserialize/issues/2 */
  language: string,
  /* repos */
}; */


type repos_actions =
  | FetchReposPending (since, string) /* (since, language) */
  | FetchReposSuccess (since, string, repos) /* (since, language, repos) */
  | FetchReposError error;

type actions =
  | ReposActions repos_actions;

type ReduxThunk.thunk _ +=
  | ReposActions repos_actions;
