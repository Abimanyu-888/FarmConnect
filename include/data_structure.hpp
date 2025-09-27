#ifndef USER_DATA_HPP
#define USER_DATA_HPP
#include<string>
#include<vector>
#include "json.hpp"
#include <fstream>
#include <cstdint>
#include <stdexcept>

struct farmer_data {
    std::string name;
    std::string username;
    std::string email;
    std::string password;
    std::vector<std::string> products;
    std::vector<std::string> orders;
    std::string state;
    int Total_Revenue;

    farmer_data(const std::string& name,const std::string& username,const std::string& email,
        const std::string& password,const std::string& state, int Total_Revenue=0,
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
    buyer_data(const std::string& name,const std::string& username,const std::string& email,
        const std::string& password,const std::string& state):
    name(name), 
    username(username),
    email(email),
    password(password),
    state(state)
    {}
    buyer_data(const std::string& thename,const std::string& theusername,const std::string& theemail,
        const std::string& thepassword,const std::string& thestate,const std::vector<std::string>& theorders):
    name(thename), 
    username(theusername),
    email(theemail),
    password(thepassword),
    state(thestate),
    orders(theorders)
    {}
};
struct product_data {
    std::string product_id;
    std::string product_name;
    std::string category;
    std::string owner;
    int price = 0;
    int stock = 0;
    std::string unit;
    std::string about;
    std::string img_extension;
    
    product_data(const std::string& theproduct_id,const std::string& thename,const std::string& thecategory,
        const std::string& theowner,int theprice,int totalstock,
        const std::string& theunit,const std::string& theabout,const std::string& extension):
    product_id(theproduct_id), 
    product_name(thename),
    category(thecategory), 
    owner(theowner),
    price(theprice), 
    stock(totalstock),
    unit(theunit), 
    about(theabout),
    img_extension(extension){}
};
struct order_data {
    std::string order_id;
    std::string product_id;
    int quantity = 0;
    bool isEmpty = true;
    
    order_data(const std::string& theorder_id,const std::string& theproduct_id,int thequantity):
    order_id(theorder_id), product_id(theproduct_id),
    quantity(thequantity), isEmpty(false) {}
};
struct email_data{
    std::string username;
    std::string email;
    email_data(const std::string& username,const std::string email):
    username(username),
    email(email){}
};

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
        return new order_data(data["order_id"],data["product_id"],data["quantity"]);
    }
};
template<>
struct jsonData<email_data>{
    static email_data* create(nlohmann::json& data){
        return new email_data(data["username"],data["email"]);
    }
};

template <class dataType>
class Hash_Table{
    struct link{
        dataType* data;
        link* next=nullptr;
        link(dataType* data):data(data){}
    };
    link** ptr_arr;
    int size;
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
    Hash_Table(int size,std::string path){
        this->size=size;
        ptr_arr=new link*[size];
        for (int i = 0; i < size; ++i) {
            ptr_arr[i] = nullptr;
        }

        load_from_json(path);
    }
    void load_from_json(std::string path){
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + path);
        }
        nlohmann::json data;
        file>>data;
        for(auto& item:data){
            dataType* node_data=jsonData<dataType>::create(item);
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
        for(int i=0;i<size;i++){
            link* curr1=ptr_arr[i];
            while(curr1){
                link* next=curr1->next;
                delete curr1->data;
                delete curr1;
                curr1=next;
            }
        }
        delete[] ptr_arr;
    }
};
#endif