/* let x = 10; */
let register () => [%bs.raw
  {| (function () {

    window.devtoolsFormatters = window.devtoolsFormatters || [];
    window.devtoolsFormatters.push({
      header: function(value) {
        if (!value.$bs) return null
        switch (value.$bs) {
          case "list":
            return ["div", {"style": "background-color: #eef"},
              "List(", value.items.length, ")"]
          case "optional":
            if (value.empty) return ["div", {}, "None"]
            return ["div", {}, "Some(", ["object", {"object": value.value}], ")"]
          case "record":
            return ["div", {style: "background-color: #efe"}, value.type, ["span", {style: "font-style: italic"}, " record"]]
          case "variant":
            return ["div", {}, ["span", {style: "font-style: italic"}, value.type, ": "], value.constructor]
        }
        return null
      },
      hasBody: function(value) {
        return value.$bs === "list" || value.$bs === "record" || (value.$bs === "variant" && value.arguments.length)
      },
      body: function(value) {
        switch (value.$bs) {
          case "list":
            return ["div", {style: "padding-left: 20px"}].concat(value.items.map(function (item) { ["div", {}, ["object", {"object": item}]] }))
          case "record":
            return ["div", {style: "padding-left: 20px"}].concat(Object.keys(value.attributes).map(function (attr) {
              return ["div", {}, attr, ": ", ["object", {"object": value.attributes[attr]}]]
            }))
          case "variant":
            return ["div", {style: "padding-left: 20px"}].concat(value.arguments.map(function (arg) { [
              "div",
              {},
              ["object", {"object": arg}]
            ] }))
          default:return null
        }
      }
    })
  })()
  |}
];
