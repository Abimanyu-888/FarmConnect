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

/**
 * @brief Defines a type alias for the Session middleware using in-memory storage.
 * This makes it easier to reference the session type throughout the app.
 */
using Session = crow::SessionMiddleware<crow::InMemoryStore>;

/**
 * @brief Utility function to check if a product is already in a buyer's cart.
 * @param cart A vector of product_ids representing the cart.
 * @param prod_id The product_id to check for.
 * @return true if the product is in the cart, false otherwise.
 */
bool incart(std::vector<std::string> cart,std::string prod_id);

/**
 * @brief Registers all API endpoints (routes) related to Farmer/Buyer accounts and actions.
 *
 * @param app The main Crow application object (passed by reference).
 * @param farmerTable The hash table containing all farmer data.
 * @param productTable The hash table containing all product data.
 * @param orderTable The hash table containing all order data.
 * @param buyerTable The hash table containing all buyer data.
 * @param searcher The inverted index object for product search.
 */
void registerFarmerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,
                            Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable
                            ,Hash_Table<buyer_data>& buyerTable,inverted_index& searcher);
void registerBuyerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,
                            Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable,
                            Hash_Table<buyer_data>& buyerTable,inverted_index& searcher);

/**
 * @brief Calculates the total price of an order.
 *
 * @param order Pointer to the order_data object.
 * @param products The hash table of products (needed to look up prices).
 * @return The total price of the order as an integer.
 */
int calculate_order_total(const order_data* order, Hash_Table<product_data>& products);
#endif 