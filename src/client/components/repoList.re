open Actions;

open Bs_webapi;

let str = ReasonReact.stringToElement;

let style = ReactDOMRe.Style.make;

let renderStar () =>
  <svg style=(style marginRight::"5px" ()) height="16" version="1.1" viewBox="0 0 14 16" width="14">
    <path d="M14 6l-4.9-.64L7 1 4.9 5.36 0 6l3.6 3.26L2.67 14 7 11.67 11.33 14l-.93-4.74z" />
  </svg>;


let renderLangIcon name => {
  let color = name
    |> Digest.string
    |> Digest.to_hex
    |> (fun s => String.sub (s ^ "FFFFFF") 0 6) ;
  <View
    style=(style
      width::"15px"
      height::"15px"
      backgroundColor::("#" ^ color)
      borderRadius::"50%"
      marginRight::"5px"
      ()
    )
  >
  </View>
};

let headerStyle =
  style flexDirection::"row" alignItems::"center" justifyContent::"space-between" ();

let colStyle = style flexDirection::"row" alignItems::"center" ;

let renderItem since::(since: since) (item: Shared.Types.repo) =>
  <TouchableOpacity
    onPress=(fun () => Dom.Location.assign (Printf.sprintf "https://github.com/%s" item.name) Dom.location)>
    <View style=(style padding::"5px 10px" ())>
      <View style=(colStyle justifyContent::"space-between" flex::"1" ())>
        <View style=(style fontSize::"17px" flex::"1" ())> (str item.name) </View>
        <View style=(colStyle width::"80px" ())>
          (renderStar ())
          <View>(str @@ string_of_int item.star_total)</View>
        </View>
      </View>
      <View style=(style color::"#aaa" fontSize::"14px" ())> (str item.desc) </View>
      <View style=(colStyle justifyContent::"start" fontSize::"14px" ())>
        <View style=(colStyle marginRight::"10px" ())>
          (renderLangIcon item.name)
          <View>(str @@ item.language)</View>
        </View>
        <View style=(colStyle ())>
          (renderStar ())
          <View>
            (str @@ string_of_int item.star_recent)
            (str @@ (switch since {
              | Daily => " stars today"
              | Weekly => " stars this week"
              | Monthly => " stars this month"
            }))
          </View>
        </View>
      </View>
    </View>
  </TouchableOpacity>;

let component = ReasonReact.statelessComponent "RepoList";

let make ::since items::(items: Shared.Types.repos) _children => {
  ...component,
  render: fun _self => <ListView items renderItem=(renderItem ::since) />
};
