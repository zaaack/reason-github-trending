const path = require('path')

module.exports = {
  entry: './src/client/app.re',
  output: {
    filename: 'bundle.js',
    path: path.resolve(__dirname, 'public'),
    publicPath: '/public'
  },
  module: {
    rules: [
      {
        test: /\.(re|ml)$/,
        use: [
          {
            loader: 'bs-loader',
            // options: {
            //   module: 'es6'
            // }
          }
        ]
      // }, {
      //   test: /\.scss$/,
      //   use: [
      //     {
      //       loader: 'style-loader'
      //     }, {
      //       loader: 'css-loader'
      //     }, {
      //       loader: 'sass-loader',
      //       options: {
      //         includePaths: []
      //       }
      //     }
      //   ]
      }
    ]
  },
  resolve: {
    extensions: ['.re', '.ml', '.js']
  },
  devServer: {
    proxy: {
      "/api": {
        target: "http://localhost:8000",
        secure: false,
        changeOrigin: true,
      }
    }
  }
}
