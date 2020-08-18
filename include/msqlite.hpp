#pragma once

//This is a helper header that includes all public headers. The usage
//is only recommended to prototype a solution or a demo. In the
//production environment the recommendaton is to only pick the headers
//which are necessary as dependencies.

#include <msqlite/open.hpp>
#include <msqlite/empty_or_error.hpp>
#include <msqlite/exec.hpp>
#include <msqlite/for_each.hpp>
#include <msqlite/onerror.hpp>
#include <msqlite/query.hpp>
#include <msqlite/step.hpp>

#include <msqlite/throws/open.hpp>
#include <msqlite/throws/exec.hpp>
#include <msqlite/throws/for_each.hpp>
#include <msqlite/throws/query.hpp>
#include <msqlite/throws/step.hpp>
