#include "routes.hpp"

void registerBuyerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,
                            Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable,Hash_Table<buyer_data>& buyerTable) {

    CROW_ROUTE(app, "/buyer/home")([&app](const crow::request& req) -> crow::response {
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
    CROW_ROUTE(app, "/buyer/cart")([&app,&productTable,&buyerTable](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        buyer_data* data=buyerTable.find(username);
        
        crow::mustache::context ctx;
        crow::json::wvalue::list product_list;
        int total=0;
        for(int i=0;i<data->cart.size();i++){
            product_data* prod_data=productTable.find(data->cart[i]);
            std::string thepath = "/db/images/" + data->cart[i];
            product_list.emplace_back(crow::json::wvalue{
                {"product_name",prod_data->product_name},
                {"product_id",prod_data->product_id}
                {"price",prod_data->price},
                {"quantity",data->quantity[i]},
                {"unit",prod_data->unit},
                {"sub_total",(prod_data->price)*(data->quantity[i])},
                {"file_path",thepath + prod_data->img_extension}
            });
            total+=(prod_data->price)*(orderData->quantity[i]);
        }
        ctx["product_list"]=std::move(product_list);
        ctx["total"]=total;

        auto page = crow::mustache::load("buyer/buyer_home.html");
        return crow::response(page.render(ctx));
    });
}