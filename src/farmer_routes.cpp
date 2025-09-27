#include "routes.hpp"


void registerFarmerRoutes(crow::App<crow::CookieParser, Session>& app,Hash_Table<farmer_data>& farmerTable,Hash_Table<email_data>& emailTable,Hash_Table<product_data>& productTable,Hash_Table<order_data>& orderTable) {
    CROW_ROUTE(app, "/farmer/dashboard")([&app,&farmerTable,&productTable](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        std::string username = session.get<std::string>("username");
        farmer_data* data = farmerTable.find(username);
        crow::mustache::context ctx;
        ctx["username"] = username;
        ctx["revenue"]=data->Total_Revenue;
        ctx["pending_orders_count"]=data->orders.size();
        ctx["products_count"]=data->products.size();

        crow::json::wvalue::list stock_list;
        for(auto& prod_id:data->products){
            product_data* prod_data=productTable.find(prod_id);
            stock_list.emplace_back(crow::json::wvalue{
                {"name", prod_data->product_name}, 
                {"stock", prod_data->stock},
                {"unit",prod_data->unit}
            });
        }
        ctx["stock_list"]=std::move(stock_list);

        auto page = crow::mustache::load("farmer/farmer_dashboard.html");
        return crow::response(page.render(ctx));
    });

    CROW_ROUTE(app, "/farmer/products")([&app,&farmerTable,&productTable](const crow::request& req ) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        farmer_data* data = farmerTable.find(username);
        crow::mustache::context ctx;
        crow::json::wvalue::list product_list;
        for(auto& prod_id:data->products){
            product_data* prod_data=productTable.find(prod_id);
            bool is_in_stock = (prod_data->stock > 0);
            std::string thepath = "/db/images/" + prod_id;
            product_list.emplace_back(crow::json::wvalue{
                {"name", prod_data->product_name}, 
                {"stock", prod_data->stock},
                {"unit",prod_data->unit},
                {"category",prod_data->category},
                {"price",prod_data->price},
                {"id",prod_data->product_id},
                {"is_in_stock", is_in_stock},
                {"file_path",thepath + prod_data->img_extension}
            });
        }
        ctx["product_list"]=std::move(product_list);
        auto page = crow::mustache::load("farmer/farmer_products.html");
        return crow::response(page.render(ctx));
    });

    CROW_ROUTE(app,"/farmer/orders")([&app](const crow::request& req ) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        auto page = crow::mustache::load("farmer/farmer_orders.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app,"/farmer/settings")([&app,&farmerTable](const crow::request& req ) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        farmer_data* data = farmerTable.find(username);
        crow::mustache::context ctx;
        ctx["username"] = username;
        ctx["name"]=data->name;
        ctx["email"]=data->email;
        ctx["phoneNo"]=1234567890;
        auto page = crow::mustache::load("farmer/farmer_settings.html");
        return crow::response(page.render(ctx));
    });

    CROW_ROUTE(app,"/farmer/add_product")([&app](const crow::request& req ) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        auto page = crow::mustache::load("farmer/add_product.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app,"/farmer/new_product").methods("POST"_method)([&app,&farmerTable,&productTable](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        std::string username = session.get<std::string>("username");
        crow::multipart::message msg(req);
        std::string id = generate_product_id();
        std::string name = msg.get_part_by_name("productName").body;
        std::string category = msg.get_part_by_name("category").body;
        int price = std::stoi(msg.get_part_by_name("price").body);
        int stock = std::stoi(msg.get_part_by_name("stock").body);
        std::string unit = msg.get_part_by_name("unit").body;
        std::string about = msg.get_part_by_name("about").body;

        const auto& file_part = msg.get_part_by_name("file-upload");
    
        std::string disposition = "";
        auto it = file_part.headers.find("Content-Disposition");
        if (it != file_part.headers.end()) {
            disposition = it->second.value; 
        }

        std::string original_filename;
        size_t filename_pos = disposition.find("filename=\"");
        if (filename_pos != std::string::npos) {
            filename_pos += 10; // Move past 'filename="'
            size_t filename_end_pos = disposition.find("\"", filename_pos);
            if (filename_end_pos != std::string::npos) {
                original_filename = disposition.substr(filename_pos, filename_end_pos - filename_pos);
            }
        }

        std::string extension = std::filesystem::path(original_filename).extension().string();
        std::string save_path = "db/images/" + id + extension;

        std::ofstream out_file(save_path, std::ios::binary);
        out_file.write(file_part.body.data(), file_part.body.size());
        out_file.close();


        product_data* new_data=new product_data(id,name,category,username,price,stock,unit,about,extension);
        productTable.add(new_data);

        farmer_data* farmerData = farmerTable.find(username);
        farmerData->products.push_back(id);

        crow::response res;
        res.code = 302;
        res.set_header("Location", "/farmer/products");
        return res;
    });
    CROW_ROUTE(app, "/farmer/edit_product/<string>")([&app,&productTable](const crow::request& req,const std::string& id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        product_data* data=productTable.find(id);
        crow::mustache::context ctx;
        ctx["product_name"]=data->product_name;
        ctx["category"]=data->category;
        ctx["about"]=data->about;
        ctx["id"]=id;
        ctx["price"]=data->price;
        ctx["unit"]=data->unit;
        ctx["stock"]=data->stock;
        ctx["extention"]=data->img_extension;

        auto page = crow::mustache::load("farmer/edit_product.html");
        return crow::response(page.render(ctx));
    });
    CROW_ROUTE(app, "/farmer/delete_product/<string>")([&app,&farmerTable,&productTable](const crow::request& req,const std::string& id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }

        std::string username = session.get<std::string>("username");
        farmer_data* farmerData = farmerTable.find(username);

        product_data* product=productTable.find(id);
        if (farmerData) {
            std::string image_filename = product->img_extension;

            // 2. Check if there is an image filename associated with the product
            std::string file_path = "db/images/" + id ;

            // 3. Use std::filesystem::remove to delete the file
            std::error_code ec;
            std::filesystem::remove(file_path, ec);

            if (ec) {
                // Optional: Log an error if the file couldn't be removed
                CROW_LOG_ERROR << "Failed to delete image file: " << file_path << " - " << ec.message();
            }
        }

        farmerData->products.erase(std::remove(farmerData->products.begin(), farmerData->products.end(), id), farmerData->products.end());
        productTable.remove(id);
        crow::response res;
        res.code = 302;
        res.set_header("Location", "/farmer/products");
        return res;
    });
    
    CROW_ROUTE(app, "/farmer/edit_product_post/<string>").methods("POST"_method)([&app,&productTable](const crow::request& req,const std::string& id) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/error");
            return res;
        }
        crow::multipart::message msg(req);
        product_data* data=productTable.find(id);
        data->product_name=msg.get_part_by_name("productName").body;
        data->category=msg.get_part_by_name("category").body;
        data->about=msg.get_part_by_name("about").body;
        data->price= std::stoi(msg.get_part_by_name("price").body);
        data->stock=std::stoi(msg.get_part_by_name("stock").body);
        data->unit=msg.get_part_by_name("unit").body;

        const auto& file_part = msg.get_part_by_name("file-upload");
    
        std::string disposition = "";
        auto it = file_part.headers.find("Content-Disposition");
        if (it != file_part.headers.end()) {
            disposition = it->second.value; 
        }

        std::string original_filename;
        size_t filename_pos = disposition.find("filename=\"");
        if (filename_pos != std::string::npos) {
            filename_pos += 10; // Move past 'filename="'
            size_t filename_end_pos = disposition.find("\"", filename_pos);
            if (filename_end_pos != std::string::npos) {
                original_filename = disposition.substr(filename_pos, filename_end_pos - filename_pos);
            }
        }

        std::string extension = std::filesystem::path(original_filename).extension().string();
        std::string new_filename = id + extension;
        std::string save_path = "db/images/" + new_filename;

        std::ofstream out_file(save_path, std::ios::binary);
        out_file.write(file_part.body.data(), file_part.body.size());
        out_file.close();

        data->img_extension=extension;

        crow::response res(303); 
        res.add_header("Location", "/farmer/products");
        return res;
    });
}
