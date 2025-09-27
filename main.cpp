#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "crow/middlewares/session.h"
using Session = crow::SessionMiddleware<crow::InMemoryStore>;

#include "include/data_structure.hpp"
#include "include/helper.hpp"
#include "include/routes.hpp"

#include <vector>
#include <string>



int main() {
    crow::mustache::set_base("templates");

    Hash_Table<farmer_data> farmerTable(100,"db/json/farmers.json");
    Hash_Table<buyer_data> buyerTable(100,"db/json/buyers.json");
    Hash_Table<product_data> productTable(100,"db/json/products.json");
    Hash_Table<order_data> orderTable(100,"db/json/orders.json");

    Hash_Table<email_data> emialMap(200);
    emialMap.load_from_json("db/json/farmers.json");
    emialMap.load_from_json("db/json/buyers.json");

    crow::App<crow::CookieParser, Session> app;

    app.get_middleware<Session>();

    registerFarmerRoutes(app,farmerTable,emialMap,productTable,orderTable);

    CROW_ROUTE(app, "/db/images/<string>")([](const crow::request&, crow::response& res, const std::string& filename) {
        res.set_static_file_info("db/images/" + filename);
        res.end();
    });

    CROW_ROUTE(app, "/")([&app](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type.empty()) {
            crow::response res(303);
            res.add_header("Location", "/landing_page");
            return res;
        }

        crow::response res(303);
        if (user_type == "Farmer") {
            res.add_header("Location", "/farmer/dashboard");
        } else {
            res.add_header("Location", "/buyer");
        }
        return res;
    });

    CROW_ROUTE(app, "/landing_page")([]() {
        auto page = crow::mustache::load("landing_page.html");
        return page.render();
    });

    CROW_ROUTE(app, "/buyer")([&app](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        auto page = crow::mustache::load("buyer/buyer_home.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app, "/sign_in")([]() {
        auto page = crow::mustache::load("sign_in.html");
        return page.render();
    });

    CROW_ROUTE(app, "/sign_in_post").methods("POST"_method)([&app, &farmerTable,&buyerTable,&emialMap](const crow::request& req, crow::response& res) {
        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string email = req_body.get("email");
        std::string pass = req_body.get("password");
        std::string type = req_body.get("type");

        email_data* username_email=emialMap.find(email);
        if (!username_email) {
            res.redirect("/error");
            res.end();
            return;
        }

        bool login_success = false;
        if (type == "Farmer") {
            farmer_data* data = farmerTable.find(username_email->username);
            if (data && data->password == pass) {
                login_success = true;
            }
        } else {
            buyer_data* data = buyerTable.find(username_email->username);
            if (data && data->password == pass) {
                login_success = true;
            }
        }

        if (login_success) {
            auto& session = app.get_context<Session>(req);
            session.set("username", username_email->username);
            session.set("user_type", type);

            res.code = 303;
            if (type == "Farmer") {
                res.add_header("Location", "/farmer/dashboard");
            } else {
                res.add_header("Location", "/buyer");
            }
        } else {
            res.code = 303;
            res.add_header("Location", "/error");
        }
        res.end();
    });

    CROW_ROUTE(app, "/sign_up")([]() {
        auto page = crow::mustache::load("sign_up.html");
        return page.render();
    });

    CROW_ROUTE(app, "/sign_up_post").methods("POST"_method)([&app, &farmerTable,&buyerTable,&emialMap](const crow::request& req, crow::response& res) {
        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string name = req_body.get("name");
        std::string username = req_body.get("username");
        std::string email = req_body.get("email");
        std::string pass = req_body.get("password");
        std::string state = req_body.get("state");
        std::string type = req_body.get("type");

        auto& session = app.get_context<Session>(req);
        session.set("username", username);
        session.set("user_type", type);
        res.code = 303;
        if (type == "Farmer") {
            farmer_data* new_user = new farmer_data(name, username, email, pass, state);
            farmerTable.add(new_user);
            res.add_header("Location", "/farmer/dashboard");
        } else {
            buyer_data* new_user = new buyer_data(name, username, email, pass, state);
            buyerTable.add(new_user);
            res.add_header("Location", "/buyer");
        }
        email_data* newnode=new email_data(username,email);
        emialMap.add(newnode);
        res.end();
    });

    CROW_ROUTE(app, "/error")([]() {
        auto page = crow::mustache::load("error.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app, "/logout")([&app](const crow::request& req, crow::response& res) {
        auto& session = app.get_context<Session>(req);

        for (const auto& key : session.keys()) {
            session.remove(key);
        }

        res.add_header("Location", "/landing_page");
        res.code = 303;
        res.end();
    });

    app.bindaddr("0.0.0.0").port(18080).multithreaded().run();
}

