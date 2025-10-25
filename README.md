# 🚜 FarmConnect

A **C++ web application** built with the **Crow framework** that connects **farmers directly with buyers**.
Farmers can manage their products, while buyers can search, cart, and purchase goods seamlessly.

🔗 **Live Demo:** [https://farmconnect-ptfe.onrender.com](https://farmconnect-ptfe.onrender.com)

---

## 🌟 Core Features

### 👨‍🌾 For Farmers

* **Product Management:** Full CRUD (Create, Read, Update, Delete) functionality for farm products.
* **Inventory Control:** Add new products with details like name, category, price, stock, unit, and images.
* **Update & Remove:** Easily edit existing product details or remove them from the store.
* **Dashboard:** View key metrics like total revenue, pending orders, and total products listed.
* **Order Management:** View and manage incoming orders from buyers.

### 🛒 For Buyers

* **Product Search:** A custom-built search engine to find products by name, category, or description.
* **Shopping Cart:** Persistent cart to add/remove items and update quantities.
* **Checkout:** Simple checkout process to place orders.
* **Profile:** View personal details and order history.

---

## 🛠️ Tech Stack & Architecture

FarmConnect is a **full-stack web application written entirely in C++**, showcasing modern C++ web development capabilities.

### 🔧 Backend & Templating

* **Backend:** Written in **C++17**, powered by the **Crow Micro-framework** (included in `vendor/Crow`).
  Handles all HTTP routing, requests, and responses.
* **Templating:** Uses **Mustache templates** via `crow::mustache::load`.
  Templates are stored in the `/templates` directory for dynamic HTML rendering.

---

## 💾 Data Persistence

FarmConnect avoids using traditional databases. Instead, it uses lightweight **JSON-based persistence**:

1. **Loading from JSON:**
   On startup, data (farmers, buyers, products, orders) is read from JSON files in `db/json/`.

2. **In-Memory Storage:**
   Data is loaded into **custom C++ data structures** for high-speed operations.

3. **Saving to JSON:**
   On graceful shutdown (e.g., `SIGINT`), in-memory data is serialized back to JSON, ensuring data persistence across sessions.

---

## 🧠 Core Data Structures

All major components are built from scratch to demonstrate **core data structure concepts**.

### 🧩 Custom Hash Table (`include/data_structure.hpp`)

Implements a `Hash_Table` class managing all key models — `farmer_data`, `buyer_data`, `product_data`, and `order_data`.
Provides **O(1)** average complexity for insertion, search, and deletion.

### 🔍 Custom Inverted Index (`include/inverted_index.hpp`)

Implements a **keyword-based search engine** using a hash map.
Maps keywords (from product names, categories, and descriptions) to sets of product IDs, enabling fast multi-word queries via set intersections.

---

## 🧭 Routing Overview

The app follows modular route separation:

| File                    | Responsibility                                                         |
| ----------------------- | ---------------------------------------------------------------------- |
| `main.cpp`              | Server initialization, login, logout, sign-up                          |
| `src/farmer_routes.cpp` | Farmer-specific routes (dashboard, add/edit product, order management) |
| `src/buyer_routes.cpp`  | Buyer routes (home, search, cart, checkout)                            |

---

## 📁 Project Structure

```
/
├── include/
│   ├── data_structure.hpp     # Custom Hash Table
│   ├── inverted_index.hpp     # Search Engine Logic
│   ├── helper.hpp             # Utility functions
│   ├── json.hpp               # nlohmann::json library
│   └── routes.hpp             # Route declarations
│
├── src/
│   ├── buyer_routes.cpp       # Buyer routes
│   ├── farmer_routes.cpp      # Farmer routes
│   └── helper.cpp             # Helper definitions
│
├── db/
│   ├── json/
│   │   ├── buyers.json
│   │   ├── farmers.json
│   │   ├── orders.json
│   │   └── products.json
│   └── images/                # Product images
│
├── templates/
│   ├── buyer/
│   ├── farmer/
│   ├── landing_page.html
│   └── sign_in.html
│
├── static/                    # CSS, JS, and static assets
├── vendor/                    # Third-party dependencies (Crow, Asio)
├── CMakeLists.txt             # Build configuration
└── main.cpp                   # Server entry point
```

---

## 🚀 Building and Running Locally

### 🧩 Prerequisites

* **CMake:** v3.10 or higher
* **C++17 Compiler:** g++ (recommended) or clang

### ⚙️ Build Instructions

```bash
# 1. Clone the repository
git clone https://github.com/abimanyu-888/farmconnect.git
cd farmconnect

# 2. Create a build directory
mkdir build && cd build

# 3. Configure with CMake
cmake ..

# 4. Build the executable
make
```

### ▶️ Running the Server

```bash
# From the 'build' directory
./dsa_project
```

Now visit **[http://localhost:18080](http://localhost:18080)** in your browser.

---

## 🗺️ Future Roadmap

* 🔐 **Secure Password Storage:** Add bcrypt for password hashing.
* 🧠 **Advanced Search:** Upgrade inverted index → Trie (Prefix Tree) for prefix matching & auto-suggestions.
* 📧 **Email Verification:** Add verification during sign-up for authenticity.
* ⭐ **Review System:** Buyers can rate and review products/farmers.

---

👨‍💻 Contributors

* Abimanyu — @abimanyu-888
* Sriram — @ramtej192
* Yuvraj — @kodap2370

> Built entirely in **C++**, FarmConnect demonstrates that modern C++ is more than just a systems language — it’s a powerful full-stack web development tool.
