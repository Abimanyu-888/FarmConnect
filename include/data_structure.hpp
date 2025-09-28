#ifndef DATA_STRUCTURE_HPP
#define DATA_STRUCTURE_HPP
#include "json.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>
#include <stdexcept>
#include <filesystem>
#include <type_traits>

struct farmer_data {
    std::string name;
    std::string username;
    std::string email;
    std::string password;
    std::vector<std::string> products;
    std::vector<std::string> orders;
    std::string state;
    int Total_Revenue=0;

    farmer_data(const std::string& name,const std::string& username,const std::string& email,
        const std::string& password,const std::string& state,int Total_Revenue=0,
        std::vector<std::string> products={}, std::vector<std::string> orders={}):
    name(name),username(username),
    email(email), 
    password(password),
    state(state),
    Total_Revenue(Total_Revenue),
    products(std::move(products)),
    orders(std::move(orders))
    {}
};
struct buyer_data {
    std::string name;
    std::string username;
    std::string email;
    std::string password;
    std::vector<std::string> orders;
    std::string state;
    std::vector<std::string> cart;
    std::vector<int> quantity;
    buyer_data(const std::string& name,const std::string& username,const std::string& email,
         const std::string& password, const std::string& state, std::vector<std::string> orders={},
        std::vector<std::string> cart={},std::vector<int> quantity={}):
    name(name), 
    username(username),
    email(email),
    password(password),
    state(state),
    orders(std::move(orders)),
    cart(std::move(cart)),
    quantity(std::move(quantity))
    {}
};
struct product_data {
    std::string product_id;
    std::string product_name;
    std::string category;
    std::string owner;
    int price =0;
    int stock =0;
    std::string unit;
    std::string about;
    std::string img_extension;
    
    product_data(const std::string& product_id,const std::string& name,const std::string& category,
        const std::string& owner,const int& price,const int& totalstock,
        const std::string& unit,const std::string& about,const std::string& extension):
    product_id(product_id), 
    product_name(name),
    category(category), 
    owner(owner),
    price(price), 
    stock(totalstock),
    unit(unit), 
    about(about),
    img_extension(extension){}
};
struct order_data {
    std::string order_id;
    std::vector<std::string> product_ids;
    std::vector<int> quantity ;
    std::string buyer;
    std::string status;
    
    order_data(const std::string& order_id,const std::vector<std::string>& product_ids,const std::vector<int>& quantity,
        const std::string& buyer,const std::string& status):
    order_id(order_id), product_ids(product_ids),
    quantity(quantity),buyer(buyer),status(status) {}
};
struct email_data{
    std::string username;
    std::string email;
    email_data(const std::string& username,const std::string& email):
    username(username),
    email(email){}
};
inline void to_json(nlohmann::json& j, const farmer_data& data) {
    j = nlohmann::json{
        {"name", data.name},
        {"username", data.username},
        {"email", data.email},
        {"password", data.password},
        {"products", data.products},
        {"orders", data.orders},
        {"state",data.state},
        {"Total_Revenue",data.Total_Revenue}
    };
}
inline void to_json(nlohmann::json& j, const buyer_data& data) {
    j = nlohmann::json{
        {"name", data.name},
        {"username", data.username},
        {"email", data.email},
        {"password", data.password},
        {"orders", data.orders},
        {"state",data.state},
        {"cart",data.cart},
        {"quantity",data.quantity}
    };
}
inline void to_json(nlohmann::json& j, const product_data& data) {
    j = nlohmann::json{
        {"product_id", data.product_id},
        {"product_name",data.product_name},
        {"category", data.category},
        {"owner", data.owner},
        {"price", data.price},
        {"stock", data.stock},
        {"unit", data.unit},
        {"about",data.about},
        {"img_extension",data.img_extension}
    };
}
inline void to_json(nlohmann::json& j, const order_data& data) {
    j = nlohmann::json{
        {"order_id", data.order_id},
        {"product_id", data.product_ids},
        {"quantity", data.quantity},
        {"buyer",data.buyer},
        {"status",data.status}
    };
}


template<class T>
struct IdType;
template<>
struct IdType<farmer_data>{
    static std::string get(farmer_data& d){
        return d.username;
    }
};
template<>
struct IdType<buyer_data>{
    static std::string get(buyer_data& d){
        return d.username;
    }
};
template<>
struct IdType<product_data>{
    static std::string get(product_data& d){
        return d.product_id;
    }
};
template<>
struct IdType<order_data>{
    static std::string get(order_data& d){
        return d.order_id;
    }
};
template<>
struct IdType<email_data>{
    static std::string get(email_data& d){
        return d.email;
    }
};


template<class T>
struct jsonData;
template<>
struct jsonData<farmer_data>{
    static farmer_data* create(nlohmann::json& data){
        return new farmer_data(data["name"],data["username"],data["email"],
                                data["password"],data["state"],data["Total_Revenue"],
                                data["products"].get<std::vector<std::string>>(),data["orders"].get<std::vector<std::string>>());
    }
};

template<>
struct jsonData<buyer_data>{
    static buyer_data* create(nlohmann::json& data){
        return new buyer_data(data["name"],data["username"],data["email"],
                             data["password"],data["state"],data["orders"].get<std::vector<std::string>>());
    }
};
template<>
struct jsonData<product_data>{
    static product_data* create(nlohmann::json& data){
        return new product_data(data["product_id"],data["product_name"],data["category"],
                                data["owner"],data["price"],data["stock"],
                                data["unit"],data["about"],data["img_extension"]);
    }
};
template<>
struct jsonData<order_data>{
    static order_data* create(nlohmann::json& data){
        return new order_data(data["order_id"],data["product_id"].get<std::vector<std::string>>(),data["quantity"].get<std::vector<int>>(),
                                data["buyer"],data["status"]);
    }
};
template<>
struct jsonData<email_data>{
    static email_data* create(nlohmann::json& data){
        return new email_data(data["username"],data["email"]);
    }
};

template<class T, class... Ts>
constexpr bool is_any_of = (std::is_same_v<T, Ts> || ...);

template <class dataType>
class Hash_Table{
    struct link{
        dataType* data;
        link* next=nullptr;
        link(dataType* data):data(data){}
    };
    link** ptr_arr;
    int size;
    std::string path;
    uint32_t fnv1a(std::string uid){
        const uint32_t bais=2166136261u;
        const uint32_t prime=16777619u;
        uint32_t hash=bais;
        for(unsigned char c:uid){
            hash^=c;
            hash*=prime;
        }
        return hash;
    } 
public:
    Hash_Table(int size=100):size(size){
        ptr_arr=new link*[size];
        for (int i = 0; i < size; ++i) {
            ptr_arr[i] = nullptr;
        }
    }
    Hash_Table(int size,std::string path):size(size),path(path){
        ptr_arr=new link*[size];
        for (int i = 0; i < size; ++i) {
            ptr_arr[i] = nullptr;
        }

        load_from_json(path);
    }
    void load_from_json(std::string path){
        this->path=path;
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + path);
        }
        nlohmann::json data;
        file>>data;
        for(auto& item:data){
            dataType* node_data = jsonData<dataType>::create(item);
            this->add(node_data);
        }
    }
    void add(dataType* node_data){
        std::string uid=IdType<dataType>::get(*node_data);
        
        if(find(uid)){
            delete node_data;
            return;
        }

        uint32_t index=fnv1a(uid)%size;
        link* newnode=new link(node_data);
        if(ptr_arr[index]){
            newnode->next = ptr_arr[index];
            ptr_arr[index] = newnode;
        }
        else{
            ptr_arr[index]=newnode;
        }
    }
    dataType* find(std::string uid){
        uint32_t index=fnv1a(uid)%size;
        link* node=ptr_arr[index];
        while(node){
            if(IdType<dataType>::get(*(node->data))==uid) return node->data;
            node=node->next;
        }
        return nullptr;
    }
    void remove(std::string uid){
        uint32_t index=fnv1a(uid)%size;
        link* node=ptr_arr[index];
        link* parent=nullptr;
        while(node != nullptr && IdType<dataType>::get(*(node->data))!=uid){
            parent=node;
            node=node->next;
        }
        if (node == nullptr) return; 
        if(parent) parent->next=node->next;
        else ptr_arr[index]=node->next;
        delete node->data;
        delete node;
    }  
    ~Hash_Table(){
        nlohmann::json json_array = nlohmann::json::array();
        constexpr bool should_save=is_any_of<dataType, farmer_data, buyer_data, order_data, product_data>;
        for(int i=0;i<size;i++){
            link* curr=ptr_arr[i];
            while(curr){
                if constexpr (should_save) {
                    json_array.push_back(*(curr->data));
                }
                link* nextNode= curr->next;
                nextNode=curr->next;
                delete curr->data;
                delete curr;
                curr=nextNode;
            }
        }
        if constexpr (should_save ) {
            if(!path.empty()){
                std::ofstream jsonfile(path);
                jsonfile << json_array.dump(4);
                jsonfile.close();
            }
        }
        delete[] ptr_arr;
    }
};
#endif