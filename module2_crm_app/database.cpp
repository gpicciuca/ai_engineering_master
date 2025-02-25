#include "database.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>

#include "utilities.h"

Database::Database(const std::string& database_path)
    : database_path_{database_path} {
  LoadFromFile();
}

Database::~Database() { SaveDatabase(); }

bool Database::LoadFromFile() {
  std::fstream file_stream{database_path_, std::ios::in};

  if (!file_stream.good()) {
    return false;
  }

  std::string line;
  while (std::getline(file_stream, line)) {
    std::stringstream ss{line};

    Customer customer{};
    ss >> customer;

    if (!customer.IsValid()) {
      std::cout << "Found invalid entry: " << std::endl;
      customer.PrintInfo();
      continue;
    }

    customers_[customer.id_] = std::move(customer);
  }

  file_stream.close();
  return true;
}

// Quite inefficient implementation since the file is truncated everytime..
// but simplest approach.
void Database::SaveDatabase() const {
  std::fstream file_stream{database_path_, std::ios::out | std::ios::trunc};

  if (!file_stream.good()) {
    return;
  }

  for (const auto& customer : customers_) {
    file_stream << customer.second;
  }

  file_stream.close();
}

Customer::ID Database::AddCustomer(const std::string& name,
                                   const std::string& surname) {
  Customer::ID customer_id{GetHighestCustomerID()};
  customer_id++;  // New customer, new ID

  customers_.insert(
      std::make_pair(customer_id, Customer{customer_id, name, surname}));

  SaveDatabase();
  return customer_id;
}

bool Database::HasCustomer(const std::string& name,
                           const std::string& surname) const {
  return std::find_if(customers_.cbegin(), customers_.cend(),
                      [&name, &surname](const auto& customer) {
                        return (customer.second.name_ == name) &&
                               (customer.second.surname_ == surname);
                      }) != customers_.cend();
};

bool Database::HasCustomer(Customer::ID customer_id) const {
  return customers_.find(customer_id) != customers_.end();
}

const Customer& Database::GetCustomer(const Customer::ID customer_id) const {
  // Will throw an exception and crash the app if access is attempted
  // without calling HasCustomer first
  return customers_.find(customer_id)->second;
}

Customer::ID Database::GetHighestCustomerID() const {
  Customer::ID last_customer_id{1U};  // Start at 1 not 0

  if (!customers_.empty()) {
    last_customer_id = customers_.rbegin()->first;
  }

  return last_customer_id;
}

bool Database::UpdateClientInfo(const Customer::ID id, const std::string& name,
                                const std::string& surname) {
  if (!HasCustomer(id)) {
    return false;
  }

  auto customer = customers_.find(id);
  customer->second.name_ = name;
  customer->second.surname_ = surname;

  SaveDatabase();
  return true;
}

bool Database::RemoveCustomer(const Customer::ID id) {
  if (!HasCustomer(id)) {
    return false;
  }

  customers_.erase(id);
  SaveDatabase();
  return true;
}

bool Database::AddInteraction(const Customer::ID id, const std::string& when,
                              const std::string& what) {
  if (!HasCustomer(id)) {
    return false;
  }

  auto customer = customers_.find(id);
  customer->second.customer_interactions_.emplace_back(
      std::make_shared<Interaction>(when, what));

  SaveDatabase();
  return true;
}

void Database::GetCustomerInteractionsInRange(
    const Customer::ID id, const std::time_t from_timestamp,
    const std::time_t to_timestamp,
    std::vector<std::shared_ptr<Interaction>>& interactions) const {
  if (!HasCustomer(id)) {
    return;
  }

  auto customer = customers_.find(id);

  for (const auto& interaction : customer->second.customer_interactions_) {
    if (interaction->InRange(from_timestamp, to_timestamp)) {
      interactions.emplace_back(interaction);
    }
  }
}

const std::map<Customer::ID, Customer>& Database::GetCustomers() const {
  return customers_;
}
