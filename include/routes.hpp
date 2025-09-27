#ifndef ROUTES_HPP 
#define ROUTES_HPP
#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "crow/middlewares/session.h"
#include "data_structure.hpp"
#include "helper.hpp"
#include <string>
#include <vector>
#include <filesystem>
using Session = crow::SessionMiddleware<crow::InMemoryStore>;

void registerFarmerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,Hash_Table<email_data>& emailTable,Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable);
#endif 