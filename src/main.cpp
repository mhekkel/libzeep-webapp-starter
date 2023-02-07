/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * 
 * Copyright (c) 2023 Maarten L. Hekkelman
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if __has_include("mrsrc.hpp") and NDEBUG
#define WEBAPP_USES_RESOURCES 1
#else
#define WEBAPP_USES_RESOURCES 0
#endif

#include "revision.hpp"

#include <utility>

#include <zeep/http/daemon.hpp>
#include <zeep/http/html-controller.hpp>
#include <zeep/http/server.hpp>

#include <zeep/http/error-handler.hpp>
#include <zeep/http/rest-controller.hpp>
#include <zeep/json/parser.hpp>

#include <mcfp.hpp>

#include <functional>
#include <iostream>
#include <tuple>

namespace fs = std::filesystem;

// --------------------------------------------------------------------

class start_controller : public zeep::http::html_controller
{
  public:
	start_controller()
	{
		map_get("", "index");
		map_get("index.html", "index");
		map_get("home", "index");
		map_get("hello", "hello");
		map_post("hello", &start_controller::post_hello, "name");
		
		mount("{css,scripts,fonts}/", &start_controller::handle_file);
	}

	zeep::http::reply post_hello(const zeep::http::scope &scope, std::optional<std::string> name);
};

zeep::http::reply start_controller::post_hello(const zeep::http::scope &scope, std::optional<std::string> name)
{
	zeep::http::scope sub(scope);
	if (name.has_value())
		sub.put("user", *name);
	return get_template_processor().create_reply_from_template("hello", sub);
}

// --------------------------------------------------------------------

int main(int argc, const char *argv[])
{
	int result = 0;

	auto &config = mcfp::config::instance();

	config.init("libzeep-webapp-starter [options] command",
		mcfp::make_option("help,h", "Display help message"),
		mcfp::make_option("verbose,v", "Verbose output"),
		mcfp::make_option("version", "Show version information"),

		mcfp::make_option<std::string>("address", "localhost", "External address"),
		mcfp::make_option<uint16_t>("port", 10336, "Port to listen to"),
		mcfp::make_option("no-daemon,F", "Do not fork into background"),
		mcfp::make_option<std::string>("user,u", "www-data", "User to run the daemon"));

	std::error_code ec;
	config.parse(argc, argv, ec);
	if (ec)
	{
		std::cerr << "Error parsing arguments: " << ec.message() << std::endl;
		return 1;
	}

	if (config.has("version"))
	{
		write_version_string(std::cout, config.has("verbose"));
		return 0;
	}

	if (config.operands().size() != 1 or config.has("help"))
	{
		std::cout << config << std::endl
				  << R"(
Command should be either:

    start     start a new server
    stop      start a running server
    status    get the status of a running server
    reload    restart a running server with new options
				)" << std::endl;

		return config.has("help") ? 0 : 1;
	}

	config.parse_config_file("config", "libzeep-webapp-starter.conf", { ".", "/etc" }, ec);
	if (ec)
	{
		std::cerr << "Error parsing config file: " << ec.message() << std::endl;
		return 1;
	}

	// --------------------------------------------------------------------

	zeep::http::daemon server([]()
		{
			auto s = new zeep::http::server("docroot");

#if WEBAPP_USES_RESOURCES
			s->set_template_processor(new zeep::http::rsrc_based_html_template_processor());
#else
			s->set_template_processor(new zeep::http::file_based_html_template_processor("docroot"));
#endif

			s->add_controller(new start_controller());

			return s; },
		"libzeep-webapp-starter");

	std::string command = config.operands().front();

	if (command == "start")
	{
		std::string address = config.get("address");
		uint16_t port = config.get<uint16_t>("port");

		if (address.find(':') != std::string::npos)
			std::cout << "starting server at http://[" << address << "]:" << port << '/' << std::endl;
		else
			std::cout << "starting server at http://" << address << ':' << port << '/' << std::endl;

		if (config.has("no-daemon"))
			result = server.run_foreground(address, port);
		else
		{
			std::string user = config.get("user");
			result = server.start(address, port, 1, 2, user);
		}
	}
	else if (command == "stop")
		result = server.stop();
	else if (command == "status")
		result = server.status();
	else if (command == "reload")
		result = server.reload();
	else
	{
		std::cerr << "Invalid command" << std::endl;
		result = 1;
	}

	return result;
}