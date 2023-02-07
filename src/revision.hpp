// Generated revision file

#pragma once

#include <ostream>

const char kProjectName[] = "libzeep-webapp-starter";
const char kVersionNumber[] = "2.0.0";
const char kVersionGitTag[] = "";
const char kBuildInfo[] = "-128-NOTFOUND";
const char kBuildDate[] = "2023-02-07T08:25:30Z";

inline void write_version_string(std::ostream &os, bool verbose)
{
	os << kProjectName << " version " << kVersionNumber << std::endl;
	if (verbose)
	{
		os << "build: " << kBuildInfo << ' ' << kBuildDate << std::endl;
		if (kVersionGitTag[0] != 0)
			os << "git tag: " << kVersionGitTag << std::endl;
	}
}
