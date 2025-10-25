#include "routes.hpp"
bool incart(std::vector<std::string> cart,std::string prod_id){
    for(std::string theitem:cart){
        if(theitem==prod_id) return true;
    }
    return false;
}
void registerBuyerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,
                            Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable,Hash_Table<buyer_data>& buyerTable,inverted_index& searcher) {

    CROW_ROUTE(app, "/buyer/home")([&app,&productTable,&buyerTable](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }   
        std::string username = session.get<std::string>("username");
        buyer_data* user_data=buyerTable.find(username);
        std::vector<product_data*> prods=productTable.getRandomProducts(12);
        crow::mustache::context ctx;
        crow::json::wvalue::list product_list;
        for(product_data* data:prods){
            std::string thepath = "/db/images/" +data->product_id+ data->img_extension;
            product_list.emplace_back(crow::json::wvalue{
                {"name",data->product_name},
                {"product_id",data->product_id},
                {"unit",data->unit},
                {"price",data->price},
                {"about",data->about},
                {"file_path",thepath},
                {"incart",incart(user_data->cart,data->product_id)}
            });
        }
        ctx["product_list"]=std::move(product_list);
        auto page = crow::mustache::load("buyer/buyer_home.html");
        return crow::response(page.render(ctx));
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
        crow::json::wvalue::list cart_list;
        int total=0;
        for(int i=0;i<data->cart.size();i++){
            product_data* prod_data=productTable.find(data->cart[i]);
            std::string thepath = "/db/images/" + data->cart[i];
            cart_list.emplace_back(crow::json::wvalue{
                {"product_name",prod_data->product_name},
                {"product_id",prod_data->product_id},
                {"price",prod_data->price},
                {"quantity",data->quantity[i]},
                {"unit",prod_data->unit},
                {"sub_total",(prod_data->price)*(data->quantity[i])},
                {"file_path",thepath + prod_data->img_extension}
            });
            total+=(prod_data->price)*(data->quantity[i]);
        }
        ctx["product_list"]=std::move(cart_list);
        ctx["total"]=total;

        auto page = crow::mustache::load("buyer/cart.html");
        return crow::response(page.render(ctx));
    });
    CROW_ROUTE(app, "/buyer/remove_from_cart/<string>")([&app,&buyerTable](const crow::request& req,const std::string& prod_id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        buyer_data* data=buyerTable.find(username);
        std::vector<std::string> newCart;
        std::vector<int> newQuantity;
        newCart.reserve(data->cart.size());
        newQuantity.reserve(data->quantity.size());
        for (size_t i = 0; i < data->cart.size(); ++i) {
            // If the item is NOT the one to be removed, we keep it.
            if (data->cart[i] != prod_id) {
                newCart.push_back(data->cart[i]);
                newQuantity.push_back(data->quantity[i]);
            }
        }
        data->cart = std::move(newCart);
        data->quantity = std::move(newQuantity);

        crow::response res;
        res.code = 302;
        res.set_header("Location", "/buyer/cart");
        return res;
    });
    CROW_ROUTE(app, "/buyer/profile")([&app,&buyerTable,&orderTable,&productTable](const crow::request& req) -> crow::response {
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
        ctx["name"]=data->name;
        ctx["email"]=data->email;
        ctx["username"]=data->username;
        crow::json::wvalue::list order_list;
        for(std::string order_id:data->orders){
            order_data* order=orderTable.find(order_id);
            int total=0;
            for(int i=0;i<order->product_ids.size();i++){
                product_data* prod_data=productTable.find(order->product_ids[i]);
                total+=order->quantity[i]*prod_data->price;
            }
            order_list.emplace_back(crow::json::wvalue{
                {"order_id",order_id},
                {"total",total},
                {"status",order->status}
            });
        }
        ctx["order_list"]=std::move(order_list);
        auto page = crow::mustache::load("buyer/profile.html");
        return crow::response(page.render(ctx));
    });
    CROW_ROUTE(app, "/buyer/search").methods("POST"_method,"GET"_method)([&app,&productTable,&searcher,&buyerTable](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        buyer_data* user_data=buyerTable.find(username);
        
        std::string query;
        if (req.method == "GET"_method) {
            char* query_param = req.url_params.get("search");
            if (query_param) {
                query = query_param; 
            }
        } 
        else if (req.method == "POST"_method) {
            auto req_body = crow::query_string(("?" + req.body).c_str());
            char* query_param = req_body.get("search");
            if (query_param) {
                query = query_param; 
            }
        }
        std::vector<std::string> prod_ids=searcher.search(query);
        crow::mustache::context ctx;
        crow::json::wvalue::list product_list;
        for(std::string prod_id:prod_ids){
            product_data* data=productTable.find(prod_id);
            std::string thepath = "/db/images/" + prod_id+data->img_extension;
            product_list.emplace_back(crow::json::wvalue{
                {"product_name",data->product_name},
                {"prod_about",data->about},
                {"product_price",data->price},
                {"product_unit",data->unit},
                {"product_id",data->product_id},
                {"file_path",thepath},
                {"incart",incart(user_data->cart,data->product_id)}
            });
        }
        ctx["query"]=query;
        ctx["product_list"]=std::move(product_list);
        auto page = crow::mustache::load("buyer/search.html");
        return crow::response(page.render(ctx));
    });
    CROW_ROUTE(app, "/buyer/product/<string>")([&app,&productTable,&buyerTable](const crow::request& req,const std::string& id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        buyer_data* user_data=buyerTable.find(username);
        product_data* data=productTable.find(id);
        crow::mustache::context ctx;
        std::string path="/db/images/"+id+data->img_extension;
        ctx["name"]=data->product_name;
        ctx["category"]=data->category;
        ctx["price"]=data->price;
        ctx["unit"]=data->unit;
        ctx["about"]=data->about;
        ctx["file_path"]=path;
        ctx["farmer"]=data->owner;
        ctx["product_id"]=id;
        ctx["incart"]=incart(user_data->cart,id);

        auto page = crow::mustache::load("buyer/product.html");
        return crow::response(page.render(ctx));
    });

    CROW_ROUTE(app, "/buyer/add_to_cart/<string>").methods("POST"_method,"GET"_method)([&app,&productTable,&buyerTable](const crow::request& req,const std::string& id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        int quantity;
        if (req.method == "GET"_method) {
            quantity=1;
        } 
        else if (req.method == "POST"_method) {
            auto req_body = crow::query_string(("?" + req.body).c_str());
            quantity = std::stoi(req_body.get("quantity"));
        
        }
        std::string username = session.get<std::string>("username");
        buyer_data* user_data=buyerTable.find(username);

        user_data->cart.push_back(id);
        user_data->quantity.push_back(quantity);

        crow::response res(303);
        res.add_header("Location", "/buyer/product/"+id);
        return res;
    });

    CROW_ROUTE(app, "/buyer/update_cart").methods("POST"_method)([&app,&buyerTable](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        buyer_data* data=buyerTable.find(username);
        std::vector<std::string> newCart;
        std::vector<int> newQuantity;
        newCart.reserve(data->cart.size());
        newQuantity.reserve(data->quantity.size());


        auto query_parser = crow::query_string(req.body, false);
        std::vector<char*> product_id_chars = query_parser.get_list("product_id",false);
        std::vector<char*> quantity_chars = query_parser.get_list("quantity",false);
        std::vector<std::string> product_ids(product_id_chars.begin(), product_id_chars.end());
        std::vector<std::string> quantities(quantity_chars.begin(), quantity_chars.end());

        for (size_t i = 0; i < product_ids.size(); ++i) {
            newCart.push_back(product_ids[i]);
            newQuantity.push_back(std::stoi(quantities[i]));
        }
        data->cart = std::move(newCart);
        data->quantity = std::move(newQuantity);

        crow::response res;
        res.code = 302;
        res.set_header("Location", "/buyer/cart");
        return res;
    });
    CROW_ROUTE(app, "/buyer/checkout").methods("POST"_method)([&app,&productTable,&orderTable,&buyerTable,&farmerTable](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        buyer_data* data=buyerTable.find(username);

        for (size_t i = 0; i < data->cart.size(); ++i) {
            product_data* prod_data=productTable.find(data->cart[i]);
            farmer_data* farmer=farmerTable.find(prod_data->owner);
            std::string id=generate_product_id();
            data->orders.push_back(id);
            farmer->orders.push_back(id);
            order_data* ord_data=new order_data(id,{data->cart[i]},{data->quantity[i]},data->username,"pending");
            orderTable.add(ord_data);
        }
        data->cart.clear();
        data->quantity.clear();

        crow::response res;
        res.code = 302;
        res.set_header("Location", "/buyer/home");
        return res;
    });
}