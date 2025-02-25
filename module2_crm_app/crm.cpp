#include "crm.h"

#include <iostream>
#include <memory>

CRM::CRM(const std::string& database_path) : database_{database_path} {}

bool CRM::AddCustomer(const std::string& name, const std::string& surname) {
  if (database_.HasCustomer(name, surname)) {
    return false;
  }

  return database_.AddCustomer(name, surname) != INVALID_CUSTOMER_ID;
}

bool CRM::PrintAllCustomers() const {
  bool has_customers = false;

  for (const auto& customer_entry : database_.GetCustomers()) {
    const Customer& customer = customer_entry.second;
    customer.PrintInfo();

    has_customers = true;
  }

  return has_customers;
}

void CRM::PrintCustomersByID(
    const std::vector<Customer::ID>& customer_ids) const {
  for (const auto& id : customer_ids) {
    if (!database_.HasCustomer(id)) {
      continue;
    }

    const auto& customer = database_.GetCustomer(id);
    customer.PrintInfo();
  }
}

bool CRM::FindCustomers(const std::string& id, const std::string& name,
                        const std::string& surname,
                        std::vector<Customer::ID>& found_customers) const {
  Customer::ID customer_id{};

  if (utilities::try_convert(id, customer_id)) {
    if (database_.HasCustomer(customer_id)) {
      found_customers.emplace_back(customer_id);
    }
  } else {
    for (const auto& customer_entry : database_.GetCustomers()) {
      const Customer& customer = customer_entry.second;

      if (!name.empty() && !surname.empty()) {
        if (customer.name_ == name && customer.surname_ == surname) {
          found_customers.emplace_back(customer.id_);
        }
      } else if (!name.empty()) {
        if (customer.name_ == name) {
          found_customers.emplace_back(customer.id_);
        }
      } else if (!surname.empty()) {
        if (customer.surname_ == surname) {
          found_customers.emplace_back(customer.id_);
        }
      }
    }
  }

  return !found_customers.empty();
}

const Customer& CRM::GetCustomer(const Customer::ID id) const {
  return database_.GetCustomer(id);
}

bool CRM::UpdateClientInfo(const Customer::ID id, const std::string& name,
                           const std::string& surname) {
  return database_.UpdateClientInfo(id, name, surname);
}

bool CRM::RemoveCustomer(const Customer::ID id) {
  return database_.RemoveCustomer(id);
}

bool CRM::AddInteraction(const Customer::ID id, const std::string& when,
                         const std::string& what) {
  return database_.AddInteraction(id, when, what);
}

bool CRM::PrintCustomerInteractions(const Customer::ID id,
                                    const std::time_t from_timestamp,
                                    const std::time_t to_timestamp) const {
  // shared_ptr's so we don't make unnecessary copies of objects
  std::vector<std::shared_ptr<Interaction>> interactions{};
  database_.GetCustomerInteractionsInRange(id, from_timestamp, to_timestamp,
                                           interactions);

  for (const auto& interaction : interactions) {
    interaction->Print();
  }

  return !interactions.empty();
}
