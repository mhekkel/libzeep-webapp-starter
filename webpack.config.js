const { CleanWebpackPlugin } = require('clean-webpack-plugin');
const path = require('path');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const glob = require('glob-all');
const { PurgeCSSPlugin } = require("purgecss-webpack-plugin");
var TerserPlugin = require("terser-webpack-plugin");

const SCRIPTS = path.resolve(__dirname, "webapp");
const DEST = path.resolve(__dirname, "docroot");

module.exports = (env) => {

	const PRODUCTION = env != null && env.PRODUCTION;

	const webpackConf = {
		entry: {
			index: path.resolve(SCRIPTS, "index.js"),
			style: path.resolve(SCRIPTS, "style.scss")
		},

		output: {
			path: DEST,
			crossOriginLoading: 'anonymous',
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
						{
							loader: "sass-loader",
							options: {
								sassOptions: {
									silenceDeprecations: [
										"color-functions",
										"global-builtin",
										"import",
									]
								}
							}
						}
					]
				},

				{
					test: /\.(woff(2)?|ttf)(\?v=[0-9]\.[0-9]\.[0-9])?$/,
					include: [
						path.resolve(__dirname, './node_modules/bootstrap-icons/font/fonts'),
						path.resolve(SCRIPTS, 'fonts')
					],
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
			extensions: ['.js', '.css', '.scss'],
		},

		target: 'web',

		plugins: [
			new MiniCssExtractPlugin({
				filename: "css/[name].css"
			})
		]
	};

	if (PRODUCTION) {
		webpackConf.mode = "production";

		webpackConf.plugins.push(
			new CleanWebpackPlugin({
				verbose: true,
				cleanOnceBeforeBuildPatterns: [
					'css/*',
					'fonts/*',
					'scripts/*',
				]
				
			}),
			new PurgeCSSPlugin({
				paths: glob.sync([
					`${SCRIPTS}/**/*`,
					`${DEST}/**/*`
				], { nodir: true })
			})
		);

		webpackConf.optimization = {
			minimize: true,
			minimizer: [new TerserPlugin()]
		};

	} else {
		webpackConf.mode = "development";
		webpackConf.devtool = 'source-map';
	}

	return webpackConf;
};
