open Actions;
let str = ReasonReact.stringToElement;
let style = ReactDOMRe.Style.make;


let headerStyle = style flexDirection::"row" alignItems::"center" justifyContent::"space-between" padding::"0 10px" ();
let colStyle =  style flexDirection::"row" alignItems::"center" ;


let renderOption option value =>
  <div
    style=(
      style
        padding::"10px"
        borderBottom::"1px solid rgba(0, 0, 0, 0.1)"
        backgroundColor::(
          switch value {
          | Some value => value == option ? "#4A90E2" : ""
          | _ => ""
          }
        )
        color::(
          switch value {
          | Some value => value == option ? "#fff" : ""
          | _ => ""
          }
        )
        ()
    )>
    (str option)
  </div>;

  let component = ReasonReact.statelessComponent "Header";
  let make state::(state: Store.repos_state) ::onLanguageChange ::onSinceChange _children => {
    ...component,
    render: fun _self => {
      <View style=headerStyle>
        <View style=(colStyle flex::"1" ())>
          <svg height="16" version="1.1" viewBox="0 0 12 16" width="12">
            <path d="M5.05.31c.81 2.17.41 3.38-.52 4.31C3.55 5.67 1.98 6.45.9 7.98c-1.45 2.05-1.7 6.53 3.53 7.7-2.2-1.16-2.67-4.52-.3-6.61-.61 2.03.53 3.33 1.94 2.86 1.39-.47 2.3.53 2.27 1.67-.02.78-.31 1.44-1.13 1.81 3.42-.59 4.78-3.42 4.78-5.56 0-2.84-2.53-3.22-1.25-5.61-1.52.13-2.03 1.13-1.89 2.75.09 1.08-1.02 1.8-1.86 1.33-.67-.41-.66-1.19-.06-1.78C8.18 5.31 8.68 2.45 5.05.32L5.03.3l.02.01z">
            </path>
          </svg>
          <View style=(style marginLeft::"10px" ())>(str "Trending")</View>
        </View>
        <View style=(colStyle ())>
          <Picker
            value=(Some state.language)
            renderPicker=(
              fun value =>
                switch value {
                | Some value => str value
                | None => str {js|language|js}
                }
            )
            renderOption=renderOption
            options=["JavaScript", "TypeScript", "F#", "OCaml", "Python", "Rust"]
            onValueChange=onLanguageChange
          />
            <Picker
              value=(Some (switch state.since {
                | Some Daily => "Daily"
                | Some Weekly => "Weekly"
                | Some Monthly => "Monthly"
                | None => "Daily"
              }))
              renderPicker=(
                fun value =>
                  switch value {
                  | Some value => str value
                  | None => str {js|language|js}
                  }
              )
              renderOption=renderOption
              options=["Daily", "Weekly", "Monthly"]
              onValueChange=onSinceChange
            />
        </View>
      </View>
    },
  };
