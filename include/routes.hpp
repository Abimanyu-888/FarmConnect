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

void registerFarmerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,
                            Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable,Hash_Table<buyer_data>& buyerTable);
void registerBuyerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,
                            Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable,Hash_Table<buyer_data>& buyerTable);
int calculate_order_total(const order_data* order, Hash_Table<product_data>& products);
#endif 