#ifndef INVERTED_INDEX_HPP
#define INVERTED_INDEX_HPP
#include "json.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include "data_structure.hpp"
#include <unordered_set>
#include <iostream>
class inverted_index{
private:
    struct link{
        std::string word;
        std::unordered_set<std::string> data;
        link* next=nullptr;
    };
    link** ptr_arr;
    int size;
    std::string path;
    std::unordered_set<std::string> stopWords = {
        "the", "a", "an", "or", "since", "is", "on", "for", "in",
        "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "you're",
        "you've", "you'll", "you'd", "your", "yours", "yourself", "yourselves", "he",
        "him", "his", "himself", "she", "she's", "her", "hers", "herself", "it", "it's",
        "its", "itself", "they", "them", "their", "theirs", "themselves", "what",
        "which", "who", "whom", "this", "that", "that'll", "these", "those", "am",
        "is", "are", "was", "were", "be", "been", "being", "have", "has", "had",
        "having", "do", "does", "did", "doing", "a", "an", "the", "and", "but", "if",
        "or", "because", "as", "until", "while", "of", "at", "by", "for", "with",
        "about", "against", "between", "into", "through", "during", "before", "after",
        "above", "below", "to", "from", "up", "down", "in", "out", "on", "off", "over",
        "under", "again", "further", "then", "once", "here", "there", "when", "where",
        "why", "how", "all", "any", "both", "each", "few", "more", "most", "other",
        "some", "such", "no", "nor", "not", "only", "own", "same", "so", "than",
        "too", "very", "s", "t", "can", "will", "just", "don", "don't", "should",
        "should've", "now", "d", "ll", "m", "o", "re", "ve", "y", "ain", "aren",
        "aren't", "couldn", "couldn't", "didn", "didn't", "doesn", "doesn't", "hadn",
        "hadn't", "hasn", "hasn't", "haven", "haven't", "isn", "isn't", "ma", "mightn",
        "mightn't", "mustn", "mustn't", "needn", "needn't", "shan", "shan't",
        "shouldn", "shouldn't", "wasn", "wasn't", "weren", "weren't", "won", "won't",
        "wouldn", "wouldn't"
    };
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
    void load_from_json(){
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file to save inverted index: " << path << std::endl;
            return; 
        }
        nlohmann::json data;
        try {
            file >> data;
        } catch (nlohmann::json::parse_error& e) {
            // Handle bad JSON
            throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()));
        }
        for (auto& [word, ids] : data.items()) {
            uint32_t index=fnv1a(word)%size;

            link* current = ptr_arr[index];
            link* found_node = nullptr;
            while (current != nullptr) {
                if (current->word == word) {
                    found_node = current;
                    break;
                }
                current = current->next;
            }

            if(!found_node){
                link* newnode=new link;
                newnode->next=ptr_arr[index];
                ptr_arr[index]=newnode;
                newnode->word=word;
                found_node=newnode;
            }
            for(std::string id:ids)
                found_node->data.insert(id);
        }
    }
    void save_to_json() {
        std::ofstream file(path);
        
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file to save inverted index: " << path << std::endl;
            return; 
        }

        nlohmann::json data_to_save = nlohmann::json::object();

        
        for (int i = 0; i < size; ++i) {
            link* current = ptr_arr[i];
            
            while (current != nullptr) {
               
                if (!current->data.empty()) { 
                    data_to_save[current->word] = current->data;
                }
                current = current->next;
            }
        }

        try {
            file << data_to_save.dump(4);
        } catch (const std::exception& e) {
            std::cerr << "Error: Failed to serialize inverted index to JSON: " << e.what() << std::endl;
        }
    }
    std::string cleanWord(std::string word) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);

        // Remove from back
        while (!word.empty() && std::ispunct(word.back())) {
            word.pop_back();
        }
        // Remove from front
        while (!word.empty() && std::ispunct(word.front())) {
            word.erase(0, 1);
        }
        return word;
    }
    void add_word(std::string word,std::string prod_id){
        uint32_t index=fnv1a(word)%size;

        link* current = ptr_arr[index];
        link* found_node = nullptr;
        while (current != nullptr) {
            if (current->word == word) {
                found_node = current;
                break;
            }
            current = current->next;
        }

        if(!found_node){
            link* newnode=new link;
            newnode->next=ptr_arr[index];
            ptr_arr[index]=newnode;
            newnode->word=word;
            found_node = newnode;
        }
        found_node->data.insert(prod_id);
    }
    void add_string(std::string& str,std::string prod_id){
        std::stringstream ss(str);
        std::string word;
        while (ss >> word) {
            std::string cleanedWord = cleanWord(word);
            if (!cleanedWord.empty() && stopWords.find(cleanedWord) == stopWords.end()) {
                add_word(cleanedWord,prod_id);
            }
        }
    }
    std::unordered_set<std::string> find(std::string& word){
        uint32_t index=fnv1a(word)%size;
        link* current = ptr_arr[index];
        link* found_node = nullptr;
        while (current != nullptr) {
            if (current->word == word) {
                found_node = current;
                break;
            }
            current = current->next;
        }

        if(found_node){
            return found_node->data;
        }
        return {};
    }
    void remove_word(const std::string& word, const std::string& prod_id) {
        uint32_t index = fnv1a(word) % size;
        link* current = ptr_arr[index];
        link* prev = nullptr;

        while (current != nullptr && current->word != word) {
            prev = current;
            current = current->next;
        }

        if (current != nullptr) {
            current->data.erase(prod_id);

            if (current->data.empty()) {
                if (prev == nullptr) {
                    ptr_arr[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current; 
            }
        }
    }
    void remove_string(const std::string& str, const std::string& prod_id) {
        std::stringstream ss(str);
        std::string word;
        while (ss >> word) {
            std::string cleanedWord = cleanWord(word);
            if (!cleanedWord.empty() && stopWords.find(cleanedWord) == stopWords.end()) {
                remove_word(cleanedWord, prod_id);
            }
        }
    }
public:
    inverted_index(std::string path,int size=1000):size(size),path(path){
        ptr_arr=new link*[size];
        for (int i = 0; i < size; ++i) {
            ptr_arr[i] = nullptr;
        }
        load_from_json();
    }
    void add(product_data* data){
        if (data == nullptr) return; 
        add_string(data->about,data->product_id);
        add_word(data->category,data->product_id);
        add_string(data->product_name,data->product_id);
    }

    std::vector<std::string> search(std::string str) {
        std::unordered_set<std::string> intersecting_ids;
        std::stringstream ss(str);
        std::string word;
        bool is_first_valid_word = true;

        while (ss >> word) {
            std::string cleanedWord = cleanWord(word);
            if (cleanedWord.empty() || stopWords.find(cleanedWord) != stopWords.end()) {
                continue;
            }
            std::unordered_set<std::string> current_word_ids = find(cleanedWord);

            if (is_first_valid_word) {
                intersecting_ids = std::move(current_word_ids);
                is_first_valid_word = false;
            } else {
                std::unordered_set<std::string> temp_intersection;
                
                if (intersecting_ids.size() < current_word_ids.size()) {
                    for (const auto& id : intersecting_ids) {
                        if (current_word_ids.count(id)) { 
                            temp_intersection.insert(id);
                        }
                    }
                } else {
                    for (const auto& id : current_word_ids) {
                        if (intersecting_ids.count(id)) { 
                            temp_intersection.insert(id);
                        }
                    }
                }
                intersecting_ids = std::move(temp_intersection);
            }
            if (intersecting_ids.empty()) {
                break;
            }
        }

        return std::vector<std::string>(intersecting_ids.begin(), intersecting_ids.end());
    }
    void remove(product_data* data) {
        if (data == nullptr) return; 
        remove_string(data->about, data->product_id);
        remove_word(data->category, data->product_id);
        remove_string(data->product_name, data->product_id);
    }
    ~inverted_index() {
        save_to_json();
        for (int i = 0; i < size; ++i) {
            link* current = ptr_arr[i];
            while (current != nullptr) {
                link* to_delete = current;
                current = current->next;
                delete to_delete; 
            }
        }
        delete[] ptr_arr;
    }

};

#endif 