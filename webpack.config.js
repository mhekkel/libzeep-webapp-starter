const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const { CleanWebpackPlugin } = require('clean-webpack-plugin');
const path = require('path');

const SCRIPTS = __dirname + "/webapp/";
const DEST = __dirname + "/docroot/";

module.exports = (env) => {

	const PRODUCTION = env != null && env.PRODUCTION;

	const webpackConf = {
		entry: {
			index: SCRIPTS + "index.js",
			style: SCRIPTS + "style.scss"
		},

		output: {
			path: DEST,
			filename: "scripts/[name].js"
		},

		module: {
			rules: [
				{
					test: /\.js/,
					exclude: /node_modules/,
					use: {
						loader: "babel-loader",
						options: {
							presets: ['@babel/preset-env']
						}
					}
				},

				{
					test: /\.(sa|sc|c)ss$/i,
					use: [
						MiniCssExtractPlugin.loader,
						"css-loader",
						"postcss-loader",
						"sass-loader"
					]
				},

				{
					test: /\.woff(2)?(\?v=[0-9]\.[0-9]\.[0-9])?$/,
					include: path.resolve(__dirname, './node_modules/bootstrap-icons/font/fonts'),
					type: 'asset/resource',
					generator: {
						filename: 'fonts/[name][ext]'
					}
				},

				{
					test: /\.(png|jpg|gif)$/,
					use: [
						{
							loader: 'file-loader',
							options: {
								outputPath: "css/images",
								publicPath: "images/"
							},
						},
					]
				}
			]
		},

		resolve: {
			extensions: ['.js', '.scss'],
		},

		plugins: [
			new MiniCssExtractPlugin({
				filename: "css/[name].css"
			})
		],

		optimization: { minimizer: [] }
	};

	if (PRODUCTION) {
		webpackConf.mode = "production";

		webpackConf.plugins.push(
			new CleanWebpackPlugin({
				cleanOnceBeforeBuildPatterns: [
					'scripts/**/*',
					'fonts/**/*',
					'css/**'
				]
			}));
	} else {
		webpackConf.mode = "development";
		webpackConf.devtool = 'source-map';
	}

	return webpackConf;
};
