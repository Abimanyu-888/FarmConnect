#ifndef ROUTES_HPP 
#define ROUTES_HPP
#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "crow/middlewares/session.h"
#include "data_structure.hpp"
#include "helper.hpp"
#include "inverted_index.hpp"
#include <string>
#include <vector>
#include <filesystem>
using Session = crow::SessionMiddleware<crow::InMemoryStore>;
bool incart(std::vector<std::string> cart,std::string prod_id);
void registerFarmerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,
                            Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable
                            ,Hash_Table<buyer_data>& buyerTable,inverted_index& searcher);
void registerBuyerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,
                            Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable,
                            Hash_Table<buyer_data>& buyerTable,inverted_index& searcher);
int calculate_order_total(const order_data* order, Hash_Table<product_data>& products);
#endif 