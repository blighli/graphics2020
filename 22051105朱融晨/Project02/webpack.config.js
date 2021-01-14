const HtmlWebpackPlugin = require("html-webpack-plugin");
const path = require("path");

module.exports = {
  devtool: "source-map",
  output: {
    path: path.resolve(__dirname, "dist", "project02")
  },

  plugins: [
    new HtmlWebpackPlugin({
      template: path.resolve(__dirname, "../Project02/public", "index.html")
    })
  ],

  module: {
    rules: [
      {
        test: /\.glsl$/,
        use: ['webpack-glsl-loader']
      },
      {
        test: /\.(png|jpe?g|gif)$/i,
        use: [
          {
            loader: 'file-loader',
          },
        ],
      },
      {
        test: /\.css$/,
        use: ["style-loader", "css-loader"]
      },
      {
        test: /\.js$/,
        exclude: /node_modules/,
        use: ["babel-loader"]
      }
    ]
  },
  devServer: {
    open: true,
    port: 8080,
    compress: true,
    hot: true,
  }
};